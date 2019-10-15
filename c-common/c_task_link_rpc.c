#include "c_task_link_rpc.h"
#include "mem_tool.h"
#include "syslog.h"
#include "c_closure_c89.h"

#define STEP_RETRY          1
#define STEP_GET_SOCKET     2
#define STEP_GET_SOCKET_OK  3

TASK_VIRTUAL_FUNCTIONS_DEFINE(struct task_link_rpc,tasklinkrpc)

status_t tasklinkrpc_on_get_socket(struct task_link_rpc *self);
bool_t tasklinkrpc_can_read_next_package(struct task_link_rpc *self);
status_t tasklinkrpc_on_got_package_header(struct task_link_rpc *self,LINKRPC_HEADER *header,struct mem *header_data);
status_t tasklinkrpc_on_got_package_data(struct task_link_rpc *self,LINKRPC_HEADER *header,struct mem *header_data,struct file_base *data);
status_t tasklinkrpc_on_prepare_package_to_send(struct task_link_rpc *self);
status_t tasklinkrpc_on_package_send_ok(struct task_link_rpc *self);
/*********************************************/
void* tasklinkrpc_get_this_pointer(struct task_link_rpc *self)
{
    return (void*)self;
}

status_t tasklinkrpc_init_basic(struct task_link_rpc *self)
{
    task_init_basic(&self->base_task);
    self->task_reader = 0;
    self->task_writer = 0;
    self->socket = NULL;
    self->reader = NULL;
    self->writer = NULL;
    self->data_buf = NULL;
    self->header_buf = NULL;
    self->step = 0;
    self->retries = 0;
    self->max_retries = 1;
    self->socket_rw = NULL;
    closure_init_basic(&self->callback);
    return OK;
}

C_BEGIN_CLOSURE_FUNC(on_socketrwer_event)
{
    int event;
    const char* err_str;
    struct task_link_rpc *self;

    C89_CLOSURE_PARAM_INT(event,0);

    if(event == C_SOCKET_READER_WRITER_EVENT_ERROR)
    {
        C89_CLOSURE_PARAM_PTR(const char*,err_str,2);
        C89_CLOSURE_PARAM_PTR(struct task_link_rpc*,self,10);

        ASSERT(self);
        XLOG(LOG_MODULE_MESSAGEPEER,LOG_LEVEL_ERROR,
            "struct task_link_rpc(%d) socket error: %s",
            task_get_id(&self->base_task),err_str
        );
        tasklinkrpc_retry(self,C_TASK_LINKRPC_ERROR_SOCKET_ERROR);
        tasklinkrpc_on_socket_error(self);
    } 
    return OK;
}
C_END_CLOSURE_FUNC(on_socketrwer_event)

status_t tasklinkrpc_init(struct task_link_rpc *self,struct taskmgr *mgr)
{
    struct closure *callback;
    tasklinkrpc_init_basic(self);
    task_init(&self->base_task,mgr);

    closure_init(&self->callback);

    X_MALLOC(self->socket,struct socket,1);
    socket_init(self->socket);
    
    X_MALLOC(self->socket_rw,struct socket_reader_writer,1);
    socketreaderwriter_init(self->socket_rw);
    socketreaderwriter_set_task_mgr(self->socket_rw, 
        task_get_taskmgr(&self->base_task));
    socketreaderwriter_set_socket(self->socket_rw,
        self->socket);

    callback = socketreaderwriter_get_callback(self->socket_rw);

    closure_set_func(callback,on_socketrwer_event);
    closure_set_param_pointer(callback,10,self);

    TASK_INIT_VIRTUAL_FUNCTIONS(tasklinkrpc);
    return OK;
}

status_t tasklinkrpc_destroy(struct task_link_rpc *self)
{
    struct taskmgr *task_mgr = task_get_taskmgr(&self->base_task);
    
    closure_destroy(&self->callback);
    taskmgr_quit_task(task_mgr,&self->task_reader);
    taskmgr_quit_task(task_mgr,&self->task_writer);

    if(self->socket_rw)
    {
        socketreaderwriter_destroy(self->socket_rw);
        X_FREE(self->socket_rw);
    }

    if(self->socket)
    {
        socket_destroy(self->socket);
        X_FREE(self->socket);
    }

    task_base_destroy(&self->base_task);
    tasklinkrpc_init_basic(self);
    return OK;
}

/////////////////////////////////////////////
C_BEGIN_CLOSURE_FUNC(on_reader_event)
{
    int event;
    struct task_link_rpc *self;
    LINKRPC_HEADER *header;
  
    C89_CLOSURE_PARAM_INT(event,0);
    C89_CLOSURE_PARAM_PTR(struct task_link_rpc*,self,10);

    ASSERT(self && tasklinkrpc_is_alive(self));

    if(event == C_TASK_LINKRPC_READER_EVENT_GOT_HEADER)
    {
        struct mem *data;
        C89_CLOSURE_PARAM_PTR(LINKRPC_HEADER*,header,1);
        C89_CLOSURE_PARAM_PTR(struct mem*,data,2);

        ASSERT(data);
        if(header->cmd != LINKRPC_CMD_HEARTBEAT)
        {
            tasklinkrpc_on_got_package_header(self,header,data);
        }
    }
    else if(event == C_TASK_LINKRPC_READER_EVENT_GOT_DATA)
    {
        struct file_base *data;
        struct mem *header_data;

        C89_CLOSURE_PARAM_PTR(LINKRPC_HEADER*,header,1);
        C89_CLOSURE_PARAM_PTR(struct mem*,header_data,2);
        C89_CLOSURE_PARAM_PTR(struct file_base*,data,3);

        ASSERT(data && header);
        if(header->cmd != LINKRPC_CMD_HEARTBEAT)
        {
            tasklinkrpc_on_got_package_data(self,header,header_data,data);
        }
    }

    else if(event == C_TASK_LINKRPC_READER_EVENT_CAN_READ_NEXT_PACKAGE)
    {
        return tasklinkrpc_can_read_next_package(self);
    }
    else if(event == C_TASK_LINKRPC_READER_EVENT_STOP)
    {
        tasklinkrpc_retry(self,C_TASK_LINKRPC_ERROR_LINKRPC_READER_STOPPED);
        self->reader = NULL;
    }
    return OK;
}
C_END_CLOSURE_FUNC(on_reader_event)

status_t tasklinkrpc_create_reader(struct task_link_rpc *self)
{
    struct task_link_rpc_reader *reader;
    struct taskmgr *taskmgr = task_get_taskmgr(&self->base_task);
    ASSERT(!taskmgr_is_task(taskmgr,self->task_reader));
   
    X_MALLOC(reader,struct task_link_rpc_reader,1);
    tasklinkrpcreader_init(reader,taskmgr);    
    reader->socket_rwer = self->socket_rw;

    closure_set_func(reader->callback,on_reader_event);
    closure_set_param_pointer(reader->callback,10,self);
    
    tasklinkrpcreader_set_header_buf(reader,self->header_buf);
    tasklinkrpcreader_set_data_buf(reader,self->data_buf);
    tasklinkrpcreader_start(reader);

    self->task_reader = task_get_id(&reader->base_task);
    self->reader = reader;
    return OK;
}

C_BEGIN_CLOSURE_FUNC(on_writer_event)
{
    int event;
    struct task_link_rpc *self;

    C89_CLOSURE_PARAM_INT(event,0);
    C89_CLOSURE_PARAM_PTR(struct task_link_rpc*,self,10);

    ASSERT(self && tasklinkrpc_is_alive(self));

    if(event == C_TASK_LINKRPC_WRITER_EVENT_GET_NEXT_PACKAGE)
    {
        return tasklinkrpc_on_prepare_package_to_send(self);
    }
    else if(event == C_TASK_LINKRPC_WRITER_EVENT_PACKAGE_SEND_OK)
    {
        tasklinkrpc_on_package_send_ok(self);
    }
    else if(event == C_TASK_LINKRPC_WRITER_EVENT_STOP)
    {
        tasklinkrpc_retry(self,C_TASK_LINKRPC_ERROR_LINKRPC_WRITER_STOPPED);
        self->writer = NULL;
    }
    return OK;
}
C_END_CLOSURE_FUNC(on_writer_event)

status_t tasklinkrpc_create_writer(struct task_link_rpc *self)
{
    struct task_link_rpc_writer *writer;
    struct taskmgr *taskmgr = task_get_taskmgr(&self->base_task);
  
    ASSERT(!taskmgr_is_task(taskmgr,self->task_writer));
    
    X_MALLOC(writer,struct task_link_rpc_writer,1);
    tasklinkrpcwriter_init(writer,taskmgr);
    writer->socket_rwer = self->socket_rw;

    closure_set_func(writer->callback,on_writer_event);
    closure_set_param_pointer(writer->callback,10,self);
    tasklinkrpcwriter_start(writer);

    self->task_writer = task_get_id(&writer->base_task);
    self->writer = writer;
    return OK;
}

status_t tasklinkrpc_start_sending(struct task_link_rpc *self)
{
    return taskmgr_resume_task(
        task_get_taskmgr(&self->base_task),
        self->task_writer);
}

status_t tasklinkrpc_set_header_buf(struct task_link_rpc *self,struct mem *i_buf)
{
    self->header_buf = i_buf;
    if(self->reader)
    {
        tasklinkrpcreader_set_header_buf(self->reader,i_buf);
    }
    return OK;
}

status_t tasklinkrpc_set_data_buf(struct task_link_rpc *self,struct file_base *i_buf)
{
    self->data_buf = i_buf;
    if(self->reader)
    {
        tasklinkrpcreader_set_data_buf(self->reader,i_buf);
    }
    return OK;
}
status_t tasklinkrpc_set_socket(struct task_link_rpc *self,int32_t fd)
{
    socket_set_socket_num(self->socket,fd);
    socket_set_blocking(self->socket,0);
    return OK;
}

status_t tasklinkrpc_transfer_socket(struct task_link_rpc *self,struct socket *from)
{
    return socket_transfer_socket_fd(self->socket,from);
}

status_t tasklinkrpc_start_reader_and_writer(struct task_link_rpc *self)
{
    ASSERT(!tasklinkrpc_is_alive(self));
    ASSERT(socket_is_connected(self->socket));
    tasklinkrpc_create_reader(self);
    tasklinkrpc_create_writer(self);
    return OK;
}
status_t tasklinkrpc_set_timeout(struct task_link_rpc *self,int to)
{
    return socketreaderwriter_set_timeout(self->socket_rw,to);
}

bool_t tasklinkrpc_is_alive(struct task_link_rpc *self)
{
    return (self->reader!=NULL && self->writer!=NULL);
}

status_t tasklinkrpc_send_request(struct task_link_rpc *self,struct file_base *header, struct file_base *data)
{
    ASSERT(self->writer);
    return tasklinkrpcwriter_send_request(self->writer,header,data);
}

status_t tasklinkrpc_send_response(struct task_link_rpc *self,struct file_base *header, struct file_base *data)
{
    ASSERT(self->writer);
    return tasklinkrpcwriter_send_response(self->writer,header,data);
}

status_t tasklinkrpc_send_raw(struct task_link_rpc *self,int linkrpc_cmd,struct file_base *header, struct file_base *data)
{
    ASSERT(self->writer);
    return tasklinkrpcwriter_send_package(self->writer,linkrpc_cmd,header,data);
}

status_t tasklinkrpc_run(struct task_link_rpc *self,uint32_t interval)
{
    if(self->step == STEP_RETRY)
    {
        self->retries++;
        if(self->max_retries > 0 && self->retries > self->max_retries )
        {
            tasklinkrpc_stop(self,C_TASK_LINKRPC_ERROR_EXCEED_MAX_RETRIES);
            return ERROR;
        }
        self->step = STEP_GET_SOCKET;
        tasklinkrpc_on_get_socket(self);
    }

    if(self->step == STEP_GET_SOCKET)
    {
        if(socket_is_connected(self->socket))
        {
            self->step = STEP_GET_SOCKET_OK;
        }
        else
        {
            task_sleep(&self->base_task,10);
        }
    }

    if(self->step == STEP_GET_SOCKET_OK)
    {
        self->step = 0;
        task_suspend(&self->base_task);
        tasklinkrpc_start_reader_and_writer(self);
    }

    return OK;
}

const char * tasklinkrpc_error_to_string(struct task_link_rpc *self,int err)
{
    if(err == C_TASK_LINKRPC_ERROR_NONE)
        return "none";
    if(err == C_TASK_LINKRPC_ERROR_LINKRPC_READER_STOPPED)
        return "linkrpc reader stopped";
    if(err == C_TASK_LINKRPC_ERROR_LINKRPC_WRITER_STOPPED)
        return "linkrpc writer stopped";
    if(err == C_TASK_LINKRPC_ERROR_EXCEED_MAX_RETRIES)
        return "exceed max retries";
    if(err == C_TASK_LINKRPC_ERROR_SOCKET_ERROR)
        return "socket error";
    if(err == C_TASK_LINKRPC_ERROR_CONNECT_ERROR)
        return "connect error";
    if(err == C_TASK_LINKRPC_ERROR_WRONG_HEADER_FORMAT)
        return "wrong header format";
    return "unknown error";
}

status_t tasklinkrpc_report_error(struct task_link_rpc *self,int err)
{
    C_LOCAL_MEM(mem);
    filebase_puts(mem_file,"struct task_link_rpc ");
    filebase_printf(mem_file,"(%d) ",task_get_id(&self->base_task));
    filebase_puts(mem_file,"exit with error:");
    filebase_puts(mem_file,tasklinkrpc_error_to_string(self,err));
    XLOG(LOG_MODULE_MESSAGEPEER,LOG_LEVEL_ERROR,
        "%s",mem_cstr(&mem)
    );
    return OK;
}

status_t tasklinkrpc_start(struct task_link_rpc *self)
{
    struct taskmgr *taskmgr = task_get_taskmgr(&self->base_task);
    
    if(self->socket)
    {
        socket_close_connect(self->socket);
    }

    self->step = STEP_RETRY;    
    taskmgr_quit_task(taskmgr,&self->task_reader);
    taskmgr_quit_task(taskmgr,&self->task_writer);
    self->reader = NULL;
    self->writer = NULL;
    task_resume(&self->base_task);
    return OK;
}

status_t tasklinkrpc_stop(struct task_link_rpc *self,int err)
{
    if(task_is_dead(&self->base_task))
        return ERROR;
    
    if(self->socket)
    {
        socket_close_connect(self->socket);
    }
    
    task_quit(&self->base_task);
    tasklinkrpc_report_error(self,err);
    return OK;
}

status_t tasklinkrpc_set_max_retries(struct task_link_rpc *self,int max)
{
    self->max_retries = max;
    return OK;
}

status_t tasklinkrpc_retry(struct task_link_rpc *self,int err)
{
    XLOG(LOG_MODULE_MESSAGEPEER,LOG_LEVEL_ERROR,
        "struct task_link_rpc: retry with error \"%s\"",tasklinkrpc_error_to_string(self,err)
    );
    tasklinkrpc_start(self);
    task_sleep(&self->base_task,1000);
    return OK;
}

status_t tasklinkrpc_write_immediately(struct task_link_rpc *self)
{
    if(self->writer)
    {
        return tasklinkrpcwriter_write_immediately(self->writer);
    }
    return ERROR;
}

struct socket* tasklinkrpc_get_socket(struct task_link_rpc *self)
{
    return self->socket;
}

int tasklinkrpc_get_retries(struct task_link_rpc *self)
{
    return self->retries;
}

status_t tasklinkrpc_set_retries(struct task_link_rpc *self,int retry)
{
    self->retries = retry;
    return OK;
}

status_t tasklinkrpc_on_socket_error(struct task_link_rpc *self)
{
    closure_set_param_pointer(&self->callback ,1 ,self);
    closure_run_event(&self->callback,C_TASK_LINKRPC_EVENT_SOCKET_ERROR);
    return OK;
}

status_t tasklinkrpc_on_get_socket(struct task_link_rpc *self)
{
    closure_set_param_pointer(&self->callback ,1 ,self);
    closure_run_event(&self->callback,C_TASK_LINKRPC_EVENT_GET_SOCKET);
    return OK;
}

bool_t tasklinkrpc_can_read_next_package(struct task_link_rpc *self)
{
    closure_set_param_pointer(&self->callback ,1 ,self);
    return closure_run_event(&self->callback,C_TASK_LINKRPC_EVENT_CAN_READ_NEXT);
}

status_t tasklinkrpc_on_prepare_package_to_send(struct task_link_rpc *self)
{
    closure_set_param_pointer(&self->callback ,1 ,self);
    closure_run_event(&self->callback,C_TASK_LINKRPC_EVENT_PREPARE_DATA_TO_SEND);
    return FALSE;
}

status_t tasklinkrpc_on_got_package_header(struct task_link_rpc *self,LINKRPC_HEADER *header,struct mem *header_data)
{
    closure_set_param_pointer(&self->callback,1,self);
    closure_set_param_pointer(&self->callback,2,header);
    closure_set_param_pointer(&self->callback,3,header_data);
    closure_run_event(&self->callback,C_TASK_LINKRPC_EVENT_GOT_PACKAGE_HEADER);
    return OK;
}

status_t tasklinkrpc_on_got_package_data(struct task_link_rpc *self,LINKRPC_HEADER *header,struct mem *header_data,struct file_base *data)
{
    closure_set_param_pointer(&self->callback ,1 ,self);
    closure_set_param_pointer(&self->callback,2,header);
    closure_set_param_pointer(&self->callback,3,header_data);
    closure_set_param_pointer(&self->callback,4,data);
    closure_run_event(&self->callback,C_TASK_LINKRPC_EVENT_GOT_PACKAGE_DATA);
    return OK;
}

status_t tasklinkrpc_on_package_send_ok(struct task_link_rpc *self)
{
    closure_set_param_pointer(&self->callback ,1 ,self);
    closure_run_event(&self->callback,C_TASK_LINKRPC_EVENT_PACKAGE_SEND_OK);
    return OK;
}
