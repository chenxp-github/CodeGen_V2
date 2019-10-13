#ifndef _C_TASK_LINK_RPC_WRITER_H
#define _C_TASK_LINK_RPC_WRITER_H

#include "cruntime.h"
#include "linkrpccommon.h"
#include "c_log_buffer.h"
#include "c_task.h"
#include "c_taskmgr.h"
#include "c_socket_reader_writer.h"
#include "c_comb_file.h"
#include "c_mem.h"

enum{
	C_TASK_LINKRPC_WRITER_ERROR_NONE = 0,	
    C_TASK_LINKRPC_WRITER_ERROR_WRONG_PACKAGE_TAG,
    C_TASK_LINKRPC_WRITER_ERROR_WRONG_CMD,
    C_TASK_LINKRPC_WRITER_ERROR_CONNECTION_CLOSED,
    C_TASK_LINKRPC_WRITER_ERROR_WRITE_TIME_OUT,
    C_TASK_LINKRPC_WRITER_ERROR_WRITE_ERROR,
};

enum{
	C_TASK_LINKRPC_WRITER_EVENT_GET_NEXT_PACKAGE = 1,
    C_TASK_LINKRPC_WRITER_EVENT_PACKAGE_SEND_OK,
    C_TASK_LINKRPC_WRITER_EVENT_STOP,
};

struct task_link_rpc_writer{
    struct task base_task;
    struct socket_reader_writer *socket_rwer;
    struct file_base *header_data;	
    struct file_base *data;
    struct mem *header;
    struct comb_file *package;
    int step;	
    uint32_t hear_beat_time;	
    struct closure *callback;
};

status_t tasklinkrpcwriter_init_basic(struct task_link_rpc_writer *self);
status_t tasklinkrpcwriter_init(struct task_link_rpc_writer *self,struct taskmgr *mgr);
status_t tasklinkrpcwriter_destroy(struct task_link_rpc_writer *self);

status_t tasklinkrpcwriter_run(struct task_link_rpc_writer *self,uint32_t interval);
const char * tasklinkrpcwriter_error_to_string(struct task_link_rpc_writer *self,int err);
status_t tasklinkrpcwriter_report_error(struct task_link_rpc_writer *self,int err);
status_t tasklinkrpcwriter_start(struct task_link_rpc_writer *self);
status_t tasklinkrpcwriter_stop(struct task_link_rpc_writer *self,status_t err);
LINKRPC_HEADER * tasklinkrpcwriter_header(struct task_link_rpc_writer *self);
status_t tasklinkrpcwriter_send_package(struct task_link_rpc_writer *self,int cmd, struct file_base *header, struct file_base *data);
status_t tasklinkrpcwriter_send_response(struct task_link_rpc_writer *self,struct file_base *header, struct file_base *data);
status_t tasklinkrpcwriter_send_request(struct task_link_rpc_writer *self,struct file_base *header, struct file_base *data);
status_t tasklinkrpcwriter_comb_package(struct task_link_rpc_writer *self);
struct closure* tasklinkrpcwriter_callback(struct task_link_rpc_writer *self);
status_t tasklinkrpcwriter_write_immediately(struct task_link_rpc_writer *self);

#endif
