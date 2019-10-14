#ifndef __C_TASK_TCP_CONNECTOR_H
#define __C_TASK_TCP_CONNECTOR_H

#include "c_taskmgr.h"
#include "c_mem.h"
#include "c_tcp_client.h"


enum{	
	C_TASK_TCP_CONNECTOR_ERROR_NONE=0,
	C_TASK_TCP_CONNECTOR_ERROR_RESOLVE_HOST_ERROR,
	C_TASK_TCP_CONNECTOR_ERROR_CONNECT_TIMEOUT,
	C_TASK_TCP_CONNECTOR_ERROR_EXCEED_MAX_RETRIES,
	C_TASK_TCP_CONNECTOR_ERROR_CONNECT_ERROR,
    C_TASK_TCP_CONNECTOR_ERROR_CONNECTION_CLOSED,	
};

enum{
	C_TASK_TCP_CONNECTOR_EVENT_CONNECTED = 1,
    C_TASK_TCP_CONNECTOR_EVENT_STOP,
};

struct task_tcp_connector{
    struct task base_task;
    int step;
    struct tcp_client tcp_client;
    char* server_name;
    int port;
    int timeout;
    int connect_time;
    int_ptr_t *hostoip_context;
    struct closure callback;
};


void* tasktcpconnector_get_this_pointer(struct task_tcp_connector *self);
status_t tasktcpconnector_init_basic(struct task_tcp_connector *self);
status_t tasktcpconnector_init(struct task_tcp_connector *self,struct taskmgr *mgr);
status_t tasktcpconnector_destroy(struct task_tcp_connector *self);
status_t tasktcpconnector_report_error(struct task_tcp_connector *self,int err);
status_t tasktcpconnector_start(struct task_tcp_connector *self);
status_t tasktcpconnector_stop(struct task_tcp_connector *self,int err);
status_t tasktcpconnector_run(struct task_tcp_connector *self, uint32_t interval);
status_t tasktcpconnector_set_server_name(struct task_tcp_connector *self,const char *_server_name);
status_t tasktcpconnector_set_port(struct task_tcp_connector *self,int _port);
status_t tasktcpconnector_set_timeout(struct task_tcp_connector *self,int _timeout);

#endif
