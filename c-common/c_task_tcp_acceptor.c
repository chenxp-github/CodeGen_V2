#include "c_task_tcp_acceptor.h"
#include "mem_tool.h"
#include "syslog.h"

enum{
    STEP_INIT_TCP_SERVER = 1,
    STEP_ACCEPT,
};

TASK_VIRTUAL_FUNCTIONS_DEFINE(struct task_tcp_acceptor,tasktcpacceptor)
/*********************************************/
status_t tasktcpacceptor_init_basic(struct task_tcp_acceptor *self)
{
    task_init_basic(&self->base_task);
    self->step = 0;    
    self->port = 0;
    self->fast_mode_time_ms = 0;
    tcpserver_init_basic(&self->tcp_server);
    closure_init_basic(&self->callback);
    return OK;
}
status_t tasktcpacceptor_init(struct task_tcp_acceptor *self,struct taskmgr *mgr)
{
    tasktcpacceptor_init_basic(self);
    task_init(&self->base_task,mgr);
    tcpserver_init(&self->tcp_server);
    tcpserver_init_server(&self->tcp_server);
    socket_set_blocking(&self->tcp_server.base_socket,0);
    closure_init(&self->callback);
    TASK_INIT_VIRTUAL_FUNCTIONS(tasktcpacceptor);
    return OK;
}

status_t tasktcpacceptor_destroy(struct task_tcp_acceptor *self)
{
    task_base_destroy(&self->base_task);
    tasktcpacceptor_init_basic(self);
    return OK;
}

status_t tasktcpacceptor_report_error(struct task_tcp_acceptor *self,int err)
{
    const char *err_str="unknown error";
    switch(err)
    {
        case C_TASK_TCP_ACCEPTOR_ERROR_NONE: 
            err_str = "none"; 
        break;

        case C_TASK_TCP_ACCEPTOR_ERROR_BIND_FAIL:
            err_str = "bind fail.";
        break;
    }
    XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,
        "task_test(%d): exit with error \"%s\"",
        task_get_id(&self->base_task),err_str);
    return OK;
}

status_t tasktcpacceptor_start(struct task_tcp_acceptor *self)
{
    task_resume(&self->base_task);
    self->step = STEP_INIT_TCP_SERVER;
    return OK;
}

status_t tasktcpacceptor_stop(struct task_tcp_acceptor *self,int err)
{
    if(task_is_dead(&self->base_task))
        return ERROR;
    task_quit(&self->base_task);
    tasktcpacceptor_report_error(self,err);
	closure_run_event(&self->callback,C_TASK_TCP_ACCEPTOR_EVENT_STOP);
    return OK;
}

status_t tasktcpacceptor_run(struct task_tcp_acceptor *self, uint32_t interval)
{
    if(self->step == STEP_INIT_TCP_SERVER)
    {
        if(!tcpserver_set_port(&self->tcp_server,self->port))
        {
            tasktcpacceptor_stop(self,C_TASK_TCP_ACCEPTOR_ERROR_BIND_FAIL);
            return ERROR;
        }
        self->step = STEP_ACCEPT;
    }
    
    if(self->step == STEP_ACCEPT)
    {
        int32_t fd = tcpserver_accept(&self->tcp_server);
        if(fd > 0)
        {
            taskmgr_on_socket_connected(
                task_get_taskmgr(&self->base_task),fd);
            closure_set_param_int(&self->callback,1,fd);
            closure_set_param_pointer(&self->callback,2,&self->tcp_server);
            closure_run_event(&self->callback,C_TASK_TCP_ACCEPTOR_EVENT_NEW_CLIENT);
        }
        
        if(self->fast_mode_time_ms > 0)
        {
            task_sleep(&self->base_task,1);
            self->fast_mode_time_ms -= interval;
        }
        else
        {
            task_sleep(&self->base_task,20);
        }
    }    
    return OK;
}

status_t tasktcpacceptor_set_listen_port(struct task_tcp_acceptor *self,int port)
{	
    self->port = port;
	return OK;	
}

