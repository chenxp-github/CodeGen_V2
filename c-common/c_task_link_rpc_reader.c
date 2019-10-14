#include "c_task_link_rpc_reader.h"
#include "syslog.h"
#include "mem_tool.h"

enum {	
    STEP_PREPARE_READ_HEADER = 1,		
	STEP_READ_HEADER,
	STEP_PREPARE_READ_HEADER_DATA,
	STEP_READ_HEADER_DATA,
	STEP_PREPARE_READ_DATA,
	STEP_READ_DATA,
};

TASK_VIRTUAL_FUNCTIONS_DEFINE(struct task_link_rpc_reader,tasklinkrpcreader)

/******************************************************************/
void* tasklinkrpcreader_get_this_pointer(struct task_link_rpc_reader *self)
{
    return (void*)self;
}
status_t tasklinkrpcreader_init_basic(struct task_link_rpc_reader *self)
{
    task_init_basic(&self->base_task);
	self->socket_rwer = NULL;
    self->header_buf = NULL;
    self->data_buf = NULL;
    self->header = NULL;
    self->step = 0;	
    self->callback = NULL;
    return OK;
}

status_t tasklinkrpcreader_init(struct task_link_rpc_reader *self,struct taskmgr *mgr)
{
    tasklinkrpcreader_init_basic(self);
    task_init(&self->base_task,mgr);

	X_MALLOC(self->header,struct mem,1);
	mem_init(self->header);
	mem_malloc(self->header,sizeof(LINKRPC_HEADER));

	X_MALLOC(self->callback,struct closure,1);
	closure_init(self->callback);
	
    TASK_INIT_VIRTUAL_FUNCTIONS(tasklinkrpcreader);
    return OK;
}

status_t tasklinkrpcreader_destroy(struct task_link_rpc_reader *self)
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

    task_base_destroy(&self->base_task);
    tasklinkrpcreader_init_basic(self);
    return OK;
}

status_t tasklinkrpcreader_run(struct task_link_rpc_reader *self,uint32_t interval)
{
    ASSERT(self->socket_rwer);

    if(self->step == STEP_PREPARE_READ_HEADER)
    {
        if(closure_run_event(self->callback,C_TASK_LINKRPC_READER_EVENT_CAN_READ_NEXT_PACKAGE))
        {
            socketreaderwriter_prepare_file_for_read(self->socket_rwer,
				&self->header->base_file_base,sizeof(LINKRPC_HEADER),TRUE);
            self->step = STEP_READ_HEADER;
        }
        else
        {
            task_sleep(&self->base_task,10);
        }
    }

    if(self->step == STEP_READ_HEADER)
    {
        if(socketreaderwriter_do_read(self->socket_rwer,interval))
        {
            LINKRPC_HEADER *header = tasklinkrpcreader_header(self);
            if(header->tag != LINKRPC_TAG)
            {
                tasklinkrpcreader_stop(self,C_TASK_LINKRPC_READER_ERROR_WRONG_PACKAGE_TAG);
                return ERROR;
            }
            if((header->cmd != LINKRPC_CMD_REQUEST)
                &&(header->cmd != LINKRPC_CMD_RESPONSE)
                &&(header->cmd != LINKRPC_CMD_HEARTBEAT))
            {
                tasklinkrpcreader_stop(self,C_TASK_LINKRPC_READER_ERROR_WRONG_CMD);
                return ERROR;
            }
            self->step = STEP_PREPARE_READ_HEADER_DATA;
        }       
    }

    if(self->step == STEP_PREPARE_READ_HEADER_DATA)
    {
        socketreaderwriter_prepare_file_for_read(
			self->socket_rwer,
			&self->header_buf->base_file_base,
			tasklinkrpcreader_header(self)->header_size,TRUE
		);

        self->step = STEP_READ_HEADER_DATA;
    }

    if(self->step == STEP_READ_HEADER_DATA)
    {
        if(socketreaderwriter_do_read(self->socket_rwer,interval))
        {
            self->step = STEP_PREPARE_READ_DATA;
            closure_set_param_pointer(self->callback,1,tasklinkrpcreader_header(self));
            closure_set_param_pointer(self->callback,2,self->header_buf);
            closure_set_param_pointer(self->callback,3,self);
            closure_run_event(self->callback,C_TASK_LINKRPC_READER_EVENT_GOT_HEADER);
        }
    }

    if(self->step == STEP_PREPARE_READ_DATA)
    {
        socketreaderwriter_prepare_file_for_read(
			self->socket_rwer,
			self->data_buf,
			tasklinkrpcreader_header(self)->data_size,
			TRUE
		);
        self->step = STEP_READ_DATA;
    }

    if(self->step == STEP_READ_DATA)
    {
        if(socketreaderwriter_do_read(self->socket_rwer,interval))
        {
            self->step = STEP_PREPARE_READ_HEADER;
            closure_set_param_pointer(self->callback,1,tasklinkrpcreader_header(self));
            closure_set_param_pointer(self->callback,2,self->header_buf);
            closure_set_param_pointer(self->callback,3,self->data_buf);
            closure_run_event(self->callback,C_TASK_LINKRPC_READER_EVENT_GOT_DATA);
        }
    }
    return OK;
}

const char * tasklinkrpcreader_error_to_string(struct task_link_rpc_reader *self,int err)
{
    if(err == C_TASK_LINKRPC_READER_ERROR_NONE)
        return "none";
    if(err == C_TASK_LINKRPC_READER_ERROR_READ_TIME_OUT)
        return "read time out";
    if(err == C_TASK_LINKRPC_READER_ERROR_READ_ERROR)
        return "read error";
    if(err == C_TASK_LINKRPC_READER_ERROR_CONNECTION_CLOSED)
        return "connection closed";
    if(err == C_TASK_LINKRPC_READER_ERROR_WRONG_PACKAGE_TAG)
        return "wrong package tag";
    if(err == C_TASK_LINKRPC_READER_ERROR_WRONG_CMD)
        return "wrong cmd";
    return "unknown error";
}

status_t tasklinkrpcreader_report_error(struct task_link_rpc_reader *self,int err)
{
    C_LOCAL_MEM(mem);
    filebase_puts(mem_file,"ctask_link_rpc_reader ");
    filebase_printf(mem_file,"(%d) ",task_get_id(&self->base_task));
    filebase_puts(mem_file,"exit with error:");
    filebase_puts(mem_file,tasklinkrpcreader_error_to_string(self,err));
    XLOG(LOG_MODULE_MESSAGEPEER,LOG_LEVEL_ERROR,
        "%s\n",mem_cstr(&mem)
    );
    return OK;
}
status_t tasklinkrpcreader_start(struct task_link_rpc_reader *self)
{
    self->step = STEP_PREPARE_READ_HEADER;
    task_resume(&self->base_task);
    return OK;
}

status_t tasklinkrpcreader_stop(struct task_link_rpc_reader *self,status_t err)
{
    if(task_is_dead(&self->base_task))return OK;
    task_quit(&self->base_task);
    tasklinkrpcreader_report_error(self,err);
    closure_set_param_int(self->callback,1,err);
    closure_set_param_pointer(self->callback,2,self);
    closure_run_event(self->callback,C_TASK_LINKRPC_READER_EVENT_STOP);
    return OK;
}

LINKRPC_HEADER * tasklinkrpcreader_header(struct task_link_rpc_reader *self)
{
    return (LINKRPC_HEADER*)mem_get_raw_buffer(self->header);
}

status_t tasklinkrpcreader_set_header_buf(struct task_link_rpc_reader *self,struct mem *i_buf)
{
    self->header_buf = i_buf;
    return OK;
}

status_t tasklinkrpcreader_set_data_buf(struct task_link_rpc_reader *self, struct file_base *i_buf)
{
    self->data_buf = i_buf;
    return OK;
}

struct closure* tasklinkrpcreader_callback(struct task_link_rpc_reader *self)
{
    return self->callback;
}
