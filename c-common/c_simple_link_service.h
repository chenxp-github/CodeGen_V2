#ifndef _C_SIMPLE_LINK_SERVICE_H
#define _C_SIMPLE_LINK_SERVICE_H

#include "cruntime.h"
#include "c_log_buffer.h"
#include "c_taskmgr.h"
#include "c_closure.h"
#include "c_task_link_rpc.h"
#include "c_mem.h"

struct sls_message{
    struct file_base *header_data;
    struct file_base *data;
    int linkrpc_msg_type;
};

struct simple_link_service{
    struct taskmgr *task_mgr;
    int task_link_rpc;
    struct mem header_recv_buf,data_recv_buf;
    struct closure callback;
    struct sls_message **sls_sending_queue;
    int sls_sending_queue_size,sls_sending_queue_len;

};

status_t simplelinkservice_init_basic(struct simple_link_service *self);
status_t simplelinkservice_init(struct simple_link_service *self, struct taskmgr *mgr);
status_t simplelinkservice_destroy(struct simple_link_service *self);

#endif
