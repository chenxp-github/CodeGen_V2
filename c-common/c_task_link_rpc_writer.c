#include "c_task_link_rpc_writer.h"
#include "syslog.h"
#include "mem_tool.h"

enum{
    STEP_PREPARE_PACKAGE = 1,
	STEP_WRITE_PACKAGE,
	STEP_WRITE_HEARTBEAT,
};

TASK_VIRTUAL_FUNCTIONS_DEFINE(struct task_link_rpc_writer,tasklinkrpcwriter)

/*****************************************************/
status_t tasklinkrpcwriter_init_basic(struct task_link_rpc_writer *self)
{
    task_init_basic(&self->base_task);
    self->socket_rwer = NULL;
    self->header_data = NULL;
    self->data = NULL;
    self->header = NULL;
    self->package = NULL;
    self->step = 0;
    self->hear_beat_time = 0;	
    self->callback = NULL;
    return OK;
}

status_t tasklinkrpcwriter_init(struct task_link_rpc_writer *self,struct taskmgr *mgr)
{
    tasklinkrpcwriter_init_basic(self);
    task_init(&self->base_task,mgr);

	X_MALLOC(self->header,struct mem,1);
	mem_init(self->header);
	mem_malloc(self->header,sizeof(LINKRPC_HEADER));

	X_MALLOC(self->callback,struct closure,1);
	closure_init(self->callback);

	X_MALLOC(self->package,struct comb_file,1);
	combfile_init(self->package,4);

	TASK_INIT_VIRTUAL_FUNCTIONS(tasklinkrpcwriter);		
    return OK;
}

status_t tasklinkrpcwriter_destroy(struct task_link_rpc_writer *self)
{
	if(self->header)
	{
		mem_destroy(self->header);
		X_FREE(self->header);
	}
	
	if(self->callback)
	{
		closure_destroy(self->callback);
		X_FREE(self->callback);
	}

	if(self->package)
	{
		combfile_destroy(self->package);
		X_FREE(self->package);
	}

    task_base_destroy(&self->base_task);
    tasklinkrpcwriter_init_basic(self);
    return OK;
}


status_t tasklinkrpcwriter_run(struct task_link_rpc_writer *self,uint32_t interval)
{
    ASSERT(self->socket_rwer);
    if(self->step == STEP_PREPARE_PACKAGE)
    {
        self->data = NULL;
        self->header_data = NULL;
        combfile_clear(self->package);

        closure_set_param_pointer(self->callback,1,self);
        closure_run_event(self->callback,C_TASK_LINKRPC_WRITER_EVENT_GET_NEXT_PACKAGE);

        if(self->header_data || self->data)
        {
            tasklinkrpcwriter_comb_package(self);
            self->step = STEP_WRITE_PACKAGE;
        }
        else
        {
            self->hear_beat_time += interval;
            if(self->hear_beat_time >= (socketreaderwriter_get_timeout(self->socket_rwer)/2))
            {
                self->hear_beat_time = 0;
                tasklinkrpcwriter_send_package(self,LINKRPC_CMD_HEARTBEAT,NULL,NULL);
                tasklinkrpcwriter_comb_package(self);
                self->step = STEP_WRITE_HEARTBEAT;
            }
            else
            {
                task_sleep(&self->base_task,10);
            }
        }
    }

    if(self->step == STEP_WRITE_PACKAGE)
    {
        if(socketreaderwriter_do_write(self->socket_rwer,interval))
        {
            combfile_clear(self->package);
            self->step = STEP_PREPARE_PACKAGE;
            closure_set_param_pointer(self->callback,1,self);
            closure_run_event(self->callback,C_TASK_LINKRPC_WRITER_EVENT_PACKAGE_SEND_OK);
            return OK;
        }
    }

    if(self->step == STEP_WRITE_HEARTBEAT)
    {
        if(socketreaderwriter_do_write(self->socket_rwer,interval))
        {
            combfile_clear(self->package);
            self->step = STEP_PREPARE_PACKAGE;
            return OK;
        }
    }

    return OK;
}

const char * tasklinkrpcwriter_error_to_string(struct task_link_rpc_writer *self,int err)
{
    if(err == C_TASK_LINKRPC_WRITER_ERROR_NONE)
        return "none";
    if(err == C_TASK_LINKRPC_WRITER_ERROR_CONNECTION_CLOSED)
        return "connection closed";
    if(err == C_TASK_LINKRPC_WRITER_ERROR_WRITE_TIME_OUT)
        return "write time out";
    if(err == C_TASK_LINKRPC_WRITER_ERROR_WRITE_ERROR)
        return "write error";
    if(err == C_TASK_LINKRPC_WRITER_ERROR_WRONG_PACKAGE_TAG)
        return "wrong package tag";
    if(err == C_TASK_LINKRPC_WRITER_ERROR_WRONG_CMD)
        return "wrong cmd";
    return "unknown error";
}
status_t tasklinkrpcwriter_report_error(struct task_link_rpc_writer *self,int err)
{
    C_LOCAL_MEM(mem);
    filebase_puts(mem_file,"ctask_link_rpc_writer ");
    filebase_printf(mem_file,"(%d) ",task_get_id(&self->base_task));
    filebase_puts(mem_file,"exit with error:");
    filebase_puts(mem_file,tasklinkrpcwriter_error_to_string(self,err));
    XLOG(LOG_MODULE_MESSAGEPEER,LOG_LEVEL_ERROR,
        "%s",mem_cstr(&mem)
    );
    return OK;
}
status_t tasklinkrpcwriter_start(struct task_link_rpc_writer *self)
{
    self->step = STEP_PREPARE_PACKAGE;
    task_resume(&self->base_task);
    return OK;
}

status_t tasklinkrpcwriter_stop(struct task_link_rpc_writer *self,status_t err)
{
    if(task_is_dead(&self->base_task))return OK;
    tasklinkrpcwriter_report_error(self,err);
    task_quit(&self->base_task);
    closure_set_param_int(self->callback,1,err);
    closure_set_param_pointer(self->callback,2,self);
    closure_run_event(self->callback,C_TASK_LINKRPC_WRITER_EVENT_STOP);
    return OK;
}

LINKRPC_HEADER * tasklinkrpcwriter_header(struct task_link_rpc_writer *self)
{
    return (LINKRPC_HEADER*)mem_get_raw_buffer(self->header);
}

status_t tasklinkrpcwriter_send_package(struct task_link_rpc_writer *self,int cmd, struct file_base *header, struct file_base *data)
{
	LINKRPC_HEADER *h = tasklinkrpcwriter_header(self);

    ASSERT(self->step == STEP_PREPARE_PACKAGE);

    h->data_size = 0;
    h->header_size = 0;
    h->tag = LINKRPC_TAG;

    if(data)
        h->data_size = (int32_t)filebase_get_size(data);
    if(header)
        h->header_size = (int32_t)filebase_get_size(header);
    h->cmd = cmd;

    self->header_data = header;
    self->data = data;
    return OK;
}

status_t tasklinkrpcwriter_send_response(struct task_link_rpc_writer *self,struct file_base *header, struct file_base *data)
{
    return tasklinkrpcwriter_send_package(self,LINKRPC_CMD_RESPONSE,header,data);
}

status_t tasklinkrpcwriter_send_request(struct task_link_rpc_writer *self,struct file_base *header, struct file_base *data)
{
    return tasklinkrpcwriter_send_package(self,LINKRPC_CMD_REQUEST,header,data);
}

status_t tasklinkrpcwriter_comb_package(struct task_link_rpc_writer *self)
{
    mem_set_size(self->header,sizeof(LINKRPC_HEADER));
    combfile_add_file(self->package,&self->header->base_file_base);
    if(self->header_data)
        combfile_add_file(self->package,self->header_data);
    if(self->data)
        combfile_add_file(self->package,self->data);
    socketreaderwriter_prepare_file_for_write(self->socket_rwer,
		&self->package->base_file_base,0,
		combfile_get_size(self->package));
    return OK;
}
struct closure* tasklinkrpcwriter_callback(struct task_link_rpc_writer *self)
{
    return self->callback;
}

//write immediately when you have data
status_t tasklinkrpcwriter_write_immediately(struct task_link_rpc_writer *self)
{
	int i;

    ASSERT(self->socket_rwer);

    if(!socketreaderwriter_is_connected(self->socket_rwer))
        return ERROR;

    for(i = 0; i < 4; i++)
    {
        if(!task_is_dead(&self->base_task))
        {
            tasklinkrpcwriter_run(self,0);
        }
    }
    return OK;
}
