#ifndef __C_TASK_TCP_ACCEPTOR_H
#define __C_TASK_TCP_ACCEPTOR_H

#include "c_taskmgr.h"
#include "c_tcp_server.h"
#include "c_closure.h"

enum{
    C_TASK_TCP_ACCEPTOR_ERROR_NONE = 0,
    C_TASK_TCP_ACCEPTOR_ERROR_BIND_FAIL,
};

enum{
    C_TASK_TCP_ACCEPTOR_EVENT_NEW_CLIENT = 1,
    C_TASK_TCP_ACCEPTOR_EVENT_STOP,
};

struct task_tcp_acceptor{
    struct task base_task;
    int step;    
    int port;
    int fast_mode_time_ms;
    struct tcp_server tcp_server;
    struct closure callback;
};

void* tasktcpacceptor_get_this_pointer(struct task_tcp_acceptor *self);
status_t tasktcpacceptor_init_basic(struct task_tcp_acceptor *self);
status_t tasktcpacceptor_init(struct task_tcp_acceptor *self,struct taskmgr *mgr);
status_t tasktcpacceptor_destroy(struct task_tcp_acceptor *self);
status_t tasktcpacceptor_report_error(struct task_tcp_acceptor *self,int err);
status_t tasktcpacceptor_start(struct task_tcp_acceptor *self);
status_t tasktcpacceptor_stop(struct task_tcp_acceptor *self,int err);
status_t tasktcpacceptor_run(struct task_tcp_acceptor *self, uint32_t interval);
status_t tasktcpacceptor_set_listen_port(struct task_tcp_acceptor *self,int port);

#endif
