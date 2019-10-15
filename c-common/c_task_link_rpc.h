#ifndef __C_TASK_LINK_RPC_H
#define __C_TASK_LINK_RPC_H

#include "c_taskmgr.h"
#include "c_socket.h"
#include "c_socket_reader_writer.h"
#include "c_task_link_rpc_reader.h"
#include "c_task_link_rpc_writer.h"
#include "c_mem.h"
#include "c_closure.h"

enum{
    C_TASK_LINKRPC_EVENT_SOCKET_ERROR = 1,
    C_TASK_LINKRPC_EVENT_GET_SOCKET,
    C_TASK_LINKRPC_EVENT_CAN_READ_NEXT,
    C_TASK_LINKRPC_EVENT_PREPARE_DATA_TO_SEND,
    C_TASK_LINKRPC_EVENT_GOT_PACKAGE_HEADER,
    C_TASK_LINKRPC_EVENT_GOT_PACKAGE_DATA,
    C_TASK_LINKRPC_EVENT_PACKAGE_SEND_OK,
};

enum{
    C_TASK_LINKRPC_ERROR_NONE = 0,
    C_TASK_LINKRPC_ERROR_LINKRPC_READER_STOPPED,
    C_TASK_LINKRPC_ERROR_LINKRPC_WRITER_STOPPED,
    C_TASK_LINKRPC_ERROR_EXCEED_MAX_RETRIES,
    C_TASK_LINKRPC_ERROR_SOCKET_ERROR,
    C_TASK_LINKRPC_ERROR_CONNECT_ERROR,
    C_TASK_LINKRPC_ERROR_WRONG_HEADER_FORMAT,
};

struct task_link_rpc{
    struct task base_task;
    struct socket *socket;
    struct socket_reader_writer *socket_rw;
    int task_reader;
    int task_writer;
    struct task_link_rpc_reader *reader;
    struct task_link_rpc_writer *writer;
    struct mem *header_buf;
    struct file_base *data_buf;
    int step;
    int retries;
    int max_retries;
    struct closure callback;
};

void* tasklinkrpc_get_this_pointer(struct task_link_rpc *self);
status_t tasklinkrpc_init_basic(struct task_link_rpc *self);
status_t tasklinkrpc_init(struct task_link_rpc *self,struct taskmgr *mgr);
status_t tasklinkrpc_destroy(struct task_link_rpc *self);
status_t tasklinkrpc_report_error(struct task_link_rpc *self,int err);
status_t tasklinkrpc_start(struct task_link_rpc *self);
status_t tasklinkrpc_stop(struct task_link_rpc *self,int err);
status_t tasklinkrpc_run(struct task_link_rpc *self, uint32_t interval);

status_t tasklinkrpc_create_reader(struct task_link_rpc *self);
status_t tasklinkrpc_create_writer(struct task_link_rpc *self);
status_t tasklinkrpc_start_sending(struct task_link_rpc *self);
status_t tasklinkrpc_set_header_buf(struct task_link_rpc *self,struct mem *i_buf);
status_t tasklinkrpc_set_data_buf(struct task_link_rpc *self,struct file_base *i_buf);
status_t tasklinkrpc_set_socket(struct task_link_rpc *self,int32_t fd);
status_t tasklinkrpc_transfer_socket(struct task_link_rpc *self,struct socket *from);
status_t tasklinkrpc_start_reader_and_writer(struct task_link_rpc *self);
status_t tasklinkrpc_set_timeout(struct task_link_rpc *self,int to);
status_t tasklinkrpc_stop(struct task_link_rpc *self,int err);
bool_t tasklinkrpc_is_alive(struct task_link_rpc *self);
status_t tasklinkrpc_send_request(struct task_link_rpc *self,struct file_base *header, struct file_base *data);
status_t tasklinkrpc_send_response(struct task_link_rpc *self,struct file_base *header, struct file_base *data);
status_t tasklinkrpc_run(struct task_link_rpc *self,uint32_t interval);
const char * tasklinkrpc_error_to_string(struct task_link_rpc *self,int err);
status_t tasklinkrpc_report_error(struct task_link_rpc *self,int err);
status_t tasklinkrpc_start(struct task_link_rpc *self);
status_t tasklinkrpc_set_max_retries(struct task_link_rpc *self,int max);
status_t tasklinkrpc_retry(struct task_link_rpc *self,int err);
status_t tasklinkrpc_write_immediately(struct task_link_rpc *self);
struct socket* tasklinkrpc_get_socket(struct task_link_rpc *self);
int tasklinkrpc_get_retries(struct task_link_rpc *self);
status_t tasklinkrpc_set_retries(struct task_link_rpc *self,int retry);
status_t tasklinkrpc_on_socket_error(struct task_link_rpc *self);
status_t tasklinkrpc_send_raw(struct task_link_rpc *self,int linkrpc_cmd,struct file_base *header, struct file_base *data);

#endif
