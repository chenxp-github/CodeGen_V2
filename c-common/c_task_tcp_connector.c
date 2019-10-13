#include "c_task_tcp_connector.h"
#include "mem_tool.h"
#include "syslog.h"

#ifndef TCP_CONNECTOR_TIMEOUT 
#define TCP_CONNECTOR_TIMEOUT (30*1000)
#endif

#define STEP_RESOLVE_HOST                   2
#define STEP_WAIT_RESOLVE_HOST              3
#define STEP_CONNECT                        4
#define STEP_CHECK_CONNECT_COMPLETE         5

TASK_VIRTUAL_FUNCTIONS_DEFINE(struct task_tcp_connector,tasktcpconnector)
/*********************************************/
status_t tasktcpconnector_init_basic(struct task_tcp_connector *self)
{    
	task_init_basic(&self->base_task);
    self->step = 0;
    tcpclient_init_basic(&self->tcp_client);
    self->server_name = NULL;
    self->port = 0;
    self->timeout = 0;
    self->connect_time = 0;
    self->hostoip_context = NULL;
    closure_init_basic(&self->callback);
    return OK;
}
status_t tasktcpconnector_init(struct task_tcp_connector *self,struct taskmgr *mgr)
{
    tasktcpconnector_init_basic(self);
    task_init(&self->base_task,mgr);   
    tcpclient_init(&self->tcp_client);
    closure_init(&self->callback);
    TASK_INIT_VIRTUAL_FUNCTIONS(tasktcpconnector);
    return OK;
}

status_t tasktcpconnector_destroy(struct task_tcp_connector *self)
{
    task_base_destroy(&self->base_task);
    tcpclient_destroy(&self->tcp_client);
    X_FREE(self->server_name);
    closure_destroy(&self->callback);
	crt_free_host_to_ip_context(&self->hostoip_context);
    tasktcpconnector_init_basic(self);
    return OK;
}

status_t tasktcpconnector_report_error(struct task_tcp_connector *self,int err)
{
    const char *err_str="unknown error";
    switch(err)
    {
        case C_TASK_TCP_CONNECTOR_ERROR_NONE: 
            err_str = "none"; 
        break;
    }
    XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,
        "task_test(%d): exit with error \"%s\"",
        task_get_id(&self->base_task),err_str);
    return OK;
}

status_t tasktcpconnector_start(struct task_tcp_connector *self)
{
    task_resume(&self->base_task);
    self->step = 1;
    return OK;
}

status_t tasktcpconnector_stop(struct task_tcp_connector *self,int err)
{
    if(task_is_dead(&self->base_task))
        return ERROR;
    task_quit(&self->base_task);
    tasktcpconnector_report_error(self,err);	
    closure_set_param_int(&self->callback,1,err);	
    closure_set_param_pointer(&self->callback,2,self);	
    closure_run_event(&self->callback,C_TASK_TCP_CONNECTOR_EVENT_STOP);
    return OK;
}

status_t tasktcpconnector_set_server_name(struct task_tcp_connector *self,const char *_server_name)
{
    int len = 0;
    if(_server_name == NULL)
    {
        X_FREE(self->server_name);
        return OK;
    }
    len = strlen(_server_name);
    X_FREE(self->server_name);
    X_MALLOC(self->server_name,char,len+1);
    memcpy(self->server_name,_server_name,len+1);
    return OK;
}

status_t tasktcpconnector_set_port(struct task_tcp_connector *self,int _port)
{
    self->port = _port;
    return OK;
}

status_t tasktcpconnector_set_timeout(struct task_tcp_connector *self,int _timeout)
{
    self->timeout = _timeout;
    return OK;
}

status_t tasktcpconnector_check_connect_timeout(struct task_tcp_connector *self,uint32_t interval)
{
    if(interval < (uint32_t)self->timeout)
        self->connect_time+=interval;
	
    if(self->connect_time >= self->timeout)		
    {		
        self->connect_time = 0;		
        tasktcpconnector_stop(self,C_TASK_TCP_CONNECTOR_ERROR_CONNECT_TIMEOUT);		
        return ERROR;		
    }	
    task_sleep(&self->base_task,100);	
    return OK;	
}

status_t tasktcpconnector_init_tcp_client(struct task_tcp_connector *self,const char *ip_address,int port)
{
    ASSERT(ip_address);
    socket_close_connect(&self->tcp_client.base_socket);	
    tcpclient_set_server_ip(&self->tcp_client,ip_address);	
    tcpclient_set_port(&self->tcp_client,port);
    socket_set_blocking(&self->tcp_client.base_socket,0);
    return OK;
}

status_t tasktcpconnector_run(struct task_tcp_connector *self,uint32_t interval)
{
    if(self->step == STEP_RESOLVE_HOST)
    {

        if(socket_is_ip_address(self->server_name))
        {
            tasktcpconnector_init_tcp_client(self,self->server_name,self->port);     
            self->step = STEP_CONNECT;
        }
        else
        {
            crt_host_to_ip_async(self->server_name,&self->hostoip_context);
            self->step = STEP_WAIT_RESOLVE_HOST;
        }
    }
   

    if(self->step == STEP_WAIT_RESOLVE_HOST)
    {
		_C_MEM_HEADER(mem);
		_C_LOCAL_MEM_HEADER(ip);

		_C_MEM_BODY(mem);
		_C_LOCAL_MEM_BODY(ip);

        if(self->hostoip_context[0] == 0)
        {
            task_sleep(&self->base_task,10);
            return ERROR;
        }

        if(self->hostoip_context[1] == 0)
        {
            tasktcpconnector_stop(self,C_TASK_TCP_CONNECTOR_ERROR_RESOLVE_HOST_ERROR);           
            return ERROR;

        }

        XLOG(LOG_MODULE_MESSAGEPEER,LOG_LEVEL_INFO,
            "resolve host %s ok",self->server_name
        );
        		
		mem_set_str(&mem,(const char*)&self->hostoip_context[5]);
		mem_seek(&mem,0);		       
        filebase_read_line(mem_file,ip_file);
        tasktcpconnector_init_tcp_client(self,mem_cstr(&ip),self->port);       
        self->step = STEP_CONNECT;
    }    

    if(self->step == STEP_CONNECT)
    {
        if(tcpclient_connect(&self->tcp_client))
        {
            self->step = STEP_CHECK_CONNECT_COMPLETE; 
        }
        else
        {
            tasktcpconnector_check_connect_timeout(self,interval);
        }
    }
    
    if(self->step == STEP_CHECK_CONNECT_COMPLETE)
    {
        if(tcpclient_is_connect_complete(&self->tcp_client))
        {

            XLOG(LOG_MODULE_MESSAGEPEER,LOG_LEVEL_INFO,
                "connect to %s:%d ok",
                self->server_name,self->port
            );
            self->connect_time = 0;
	         
			taskmgr_on_socket_connected(
				task_get_taskmgr(&self->base_task),
				socket_get_socket_num(&self->tcp_client.base_socket)
			);

            closure_set_param_pointer(&self->callback,1,self);
            closure_set_param_pointer(&self->callback,2,&self->tcp_client);
            closure_run_event(&self->callback,C_TASK_TCP_CONNECTOR_EVENT_CONNECTED);
            tasktcpconnector_stop(self,C_TASK_TCP_CONNECTOR_ERROR_NONE);
        }
        else
        {
            tasktcpconnector_check_connect_timeout(self,interval);
        }
    }
    return OK;
}
