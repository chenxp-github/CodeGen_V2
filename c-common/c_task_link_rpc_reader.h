#ifndef _C_TASK_LINK_RPC_READER_H
#define _C_TASK_LINK_RPC_READER_H

#include "cruntime.h"
#include "linkrpccommon.h"
#include "c_log_buffer.h"
#include "c_task.h"
#include "c_taskmgr.h"
#include "c_socket_reader_writer.h"

enum{
	
	C_TASK_LINKRPC_READER_ERROR_NONE = 0,
    C_TASK_LINKRPC_READER_ERROR_READ_TIME_OUT,
    C_TASK_LINKRPC_READER_ERROR_READ_ERROR,
    C_TASK_LINKRPC_READER_ERROR_CONNECTION_CLOSED,
    C_TASK_LINKRPC_READER_ERROR_WRONG_PACKAGE_TAG,
    C_TASK_LINKRPC_READER_ERROR_WRONG_CMD,
};

enum{
	C_TASK_LINKRPC_READER_EVENT_GOT_HEADER = 1,
    C_TASK_LINKRPC_READER_EVENT_GOT_DATA,
    C_TASK_LINKRPC_READER_EVENT_STOP,
    C_TASK_LINKRPC_READER_EVENT_CAN_READ_NEXT_PACKAGE,
};

struct task_link_rpc_reader{
    struct task base_task;
    struct socket_reader_writer *socket_rwer;
	struct mem *header_buf;	
    struct file_base *data_buf;	
    struct mem *header;	
    struct closure *callback;
	int step;
};

status_t tasklinkrpcreader_init_basic(struct task_link_rpc_reader *self);
status_t tasklinkrpcreader_init(struct task_link_rpc_reader *self,struct taskmgr *mgr);
status_t tasklinkrpcreader_destroy(struct task_link_rpc_reader *self);

status_t tasklinkrpcreader_run(struct task_link_rpc_reader *self,uint32_t interval);
const char * tasklinkrpcreader_error_to_string(struct task_link_rpc_reader *self,int err);
status_t tasklinkrpcreader_report_error(struct task_link_rpc_reader *self,int err);
status_t tasklinkrpcreader_start(struct task_link_rpc_reader *self);
status_t tasklinkrpcreader_stop(struct task_link_rpc_reader *self,status_t err);
LINKRPC_HEADER * tasklinkrpcreader_header(struct task_link_rpc_reader *self);
status_t tasklinkrpcreader_set_header_buf(struct task_link_rpc_reader *self,struct mem *i_buf);
status_t tasklinkrpcreader_set_data_buf(struct task_link_rpc_reader *self, struct file_base *i_buf);
struct closure* tasklinkrpcreader_callback(struct task_link_rpc_reader *self);


#endif
