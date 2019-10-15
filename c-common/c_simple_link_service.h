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
    bool_t as_tcp_server;
    int task_tcp_connector; //when as client    
    char* server_name; //when as client
    int port;
};

status_t simplelinkservice_init_basic(struct simple_link_service *self);
status_t simplelinkservice_init(struct simple_link_service *self, struct taskmgr *mgr,bool_t as_tcp_server);
status_t simplelinkservice_destroy(struct simple_link_service *self);
struct taskmgr* simplelinkservice_get_task_mgr(struct simple_link_service *self);
status_t simplelinkservice_set_task_mgr(struct simple_link_service *self,struct taskmgr *_task_mgr);
struct sls_message** simplelinkservice_get_sls_sending_queue(struct simple_link_service *self);
int simplelinkservice_get_sls_sending_queue_len(struct simple_link_service *self);
struct sls_message* simplelinkservice_get_sls_sending_queue_elem(struct simple_link_service *self,int _index);
status_t simplelinkservice_alloc_sls_sending_queue(struct simple_link_service *self,int _len);
status_t simplelinkservice_set_sls_sending_queue(struct simple_link_service *self,struct sls_message *_sls_sending_queue[], int _len);
status_t simplelinkservice_set_sls_sending_queue_elem(struct simple_link_service *self,int _index,struct sls_message *_sls_sending_queue);
status_t simplelinkservice_send_message(struct simple_link_service *self,struct sls_message *msg);
status_t simplelinkservice_delete_message(struct simple_link_service *self, int index);
struct task_link_rpc *simplelinkservice_get_task_link_rpc(struct simple_link_service *self);
status_t simplelinkservice_start(struct simple_link_service *self);
status_t simplelinkservice_set_server_name(struct simple_link_service *self,const char *_server_name);
status_t simplelinkservice_set_port(struct simple_link_service *self,int _port);

#endif
