#include "c_simple_link_service.h"
#include "c_task_tcp_connector.h"
#include "syslog.h"
#include "mem_tool.h"

status_t sls_message_init_basic(struct sls_message *self)
{
    self->header_data = NULL;
    self->data = NULL;
    self->linkrpc_msg_type = 0;
    self->is_on_heap = 0;
    return OK;
}

status_t sls_message_init(struct sls_message *self)
{
    sls_message_init_basic(self);
    return OK;
}

status_t sls_message_destroy(struct sls_message *self)
{
    if(self->data)
    {
        X_VIRTUAL_DELETE(self->data,filebase_destroy);
    }

    if(self->header_data)
    {
        X_VIRTUAL_DELETE(self->header_data,filebase_destroy);
    }
    sls_message_init_basic(self);
    return OK;
}

status_t sls_message_check(struct sls_message *self)
{
    if(!self->is_on_heap)
        return ERROR;

    if(self->header_data)
    {
        if(!self->header_data->is_on_heap)
            return ERROR;
    }

    if(self->data)
    {

        if(!self->data->is_on_heap)
            return ERROR;
    }

    return OK;
}
/**************************************************************/
static status_t simplelinkservice_start_tcp_connector(struct simple_link_service *self);
static status_t simplelinkservice_on_got_message(struct simple_link_service *self,LINKRPC_HEADER *header, struct mem *header_data, struct file_base *data);
/**************************************************************/
status_t simplelinkservice_init_basic(struct simple_link_service *self)
{
    self->task_mgr = NULL;
    self->task_link_rpc = 0;
    self->task_tcp_connector = 0;
    closure_init_basic(&self->callback);
    mem_init_basic(&self->header_recv_buf);
    mem_init_basic(&self->data_recv_buf);
    self->sls_sending_queue = NULL;
    self->sls_sending_queue_len = 0;
    self->as_tcp_server = FALSE;
    self->server_name = NULL;
    self->port = 0;
    self->sls_sending_queue_size = 0;
    return OK;
}

status_t simplelinkservice_init(struct simple_link_service *self, struct taskmgr *mgr,bool_t as_tcp_server)
{
    simplelinkservice_init_basic(self);
    self->task_mgr = mgr;
    self->as_tcp_server = as_tcp_server;
    closure_init(&self->callback);
    mem_init(&self->header_recv_buf);
    mem_init(&self->data_recv_buf);
    simplelinkservice_alloc_sls_sending_queue(self,1024);
    return OK;
}

status_t simplelinkservice_destroy(struct simple_link_service *self)
{
    if(self->sls_sending_queue)
    {        
        int i;
        for(i = 0; i < self->sls_sending_queue_len; i++)
        {
            if(self->sls_sending_queue[i])
            {
                sls_message_destroy(self->sls_sending_queue[i]);
                X_FREE(self->sls_sending_queue[i]);
            }
        }       
        X_FREE(self->sls_sending_queue);
    }

    if(self->task_mgr)
    {
        taskmgr_quit_task(self->task_mgr,&self->task_link_rpc);
        taskmgr_quit_task(self->task_mgr,&self->task_tcp_connector);
    } 
    mem_destroy(&self->header_recv_buf);
    mem_destroy(&self->data_recv_buf);
    closure_destroy(&self->callback);
    X_FREE(self->server_name);
    simplelinkservice_init_basic(self);
    return OK;
}

struct taskmgr* simplelinkservice_get_task_mgr(struct simple_link_service *self)
{
    return self->task_mgr;
}

status_t simplelinkservice_set_task_mgr(struct simple_link_service *self,struct taskmgr *_task_mgr)
{
    ASSERT(_task_mgr);
    self->task_mgr = _task_mgr;
    return OK;
}

C_BEGIN_CLOSURE_FUNC(on_task_link_rpc_event)
{
    struct simple_link_service *self;
    int event;

    C89_CLOSURE_PARAM_PTR(struct simple_link_service*,self,10);
    C89_CLOSURE_PARAM_INT(event,0);
    
    if(event == C_TASK_LINKRPC_EVENT_CAN_READ_NEXT)
    {
        return TRUE;
    }

    else if(event == C_TASK_LINKRPC_EVENT_GET_SOCKET)
    {
        if(!self->as_tcp_server)
        {
            simplelinkservice_start_tcp_connector(self);
        }
    }

    else if(event == C_TASK_LINKRPC_EVENT_PREPARE_DATA_TO_SEND)
    {
        struct sls_message *first;
        struct task_link_rpc *task;
        if(simplelinkservice_get_sls_sending_queue_len(self) <= 0)
            return FALSE;

        first = simplelinkservice_get_sls_sending_queue_elem(self,0);
        ASSERT(first);
        
        task = simplelinkservice_get_task_link_rpc(self);
        ASSERT(task);

        tasklinkrpc_send_raw(task,first->linkrpc_msg_type,first->header_data,first->data);        
        return OK;
    }
    
    else if(event == C_TASK_LINKRPC_EVENT_GOT_PACKAGE_HEADER)
    {
        struct task_link_rpc *pt;
        LINKRPC_HEADER *header;
        struct mem *header_data;

        C89_CLOSURE_PARAM_PTR(struct task_link_rpc*,pt,1);
        C89_CLOSURE_PARAM_PTR(LINKRPC_HEADER*, header, 2);
        C89_CLOSURE_PARAM_PTR(struct mem*, header_data,3);
        
        if(mem_get_malloc_size(&self->data_recv_buf) != header->data_size)
        {
            mem_free(&self->data_recv_buf);
            mem_malloc(&self->data_recv_buf,header->data_size);
        }
    }

    else if(event == C_TASK_LINKRPC_EVENT_GOT_PACKAGE_DATA)
    {
        struct task_link_rpc *pt;
        LINKRPC_HEADER *header;
        struct mem *header_data;
        struct file_base *data;

        C89_CLOSURE_PARAM_PTR(struct task_link_rpc*,pt,1);
        C89_CLOSURE_PARAM_PTR(LINKRPC_HEADER*, header, 2);
        C89_CLOSURE_PARAM_PTR(struct mem*, header_data,3);
        C89_CLOSURE_PARAM_PTR(struct file_base*, data,4);

        simplelinkservice_on_got_message(self,header,header_data,data);
    }

    else if(event == C_TASK_LINKRPC_EVENT_PACKAGE_SEND_OK)
    {
        simplelinkservice_delete_message(self,0); //delete first
    }
    return OK;
}
C_END_CLOSURE_FUNC(on_task_link_rpc_event);

status_t simplelinkservice_start(struct simple_link_service *self)
{
    struct task_link_rpc *pt;
    ASSERT(self->task_mgr);
    ASSERT(!taskmgr_is_task(self->task_mgr,self->task_link_rpc));
    if(!self->as_tcp_server)
    {
        ASSERT(self->server_name);
        ASSERT(self->port != 0);
    }

    mem_free(&self->header_recv_buf);
    mem_malloc(&self->header_recv_buf,4096);
    mem_free(&self->data_recv_buf);

    X_MALLOC(pt,struct task_link_rpc,1);
    tasklinkrpc_init(pt,self->task_mgr);    
    tasklinkrpc_set_header_buf(pt,&self->header_recv_buf);
    tasklinkrpc_set_data_buf(pt,&self->data_recv_buf.base_file_base);
    tasklinkrpc_set_max_retries(pt,self->as_tcp_server?1:-1);
    tasklinkrpc_start(pt);
    closure_set_func(&pt->callback,on_task_link_rpc_event);
    closure_set_param_pointer(&pt->callback,10,self);    

    self->task_link_rpc = task_get_id(&pt->base_task);
    return OK;
}

struct sls_message** simplelinkservice_get_sls_sending_queue(struct simple_link_service *self)
{
    return self->sls_sending_queue;
}

int simplelinkservice_get_sls_sending_queue_len(struct simple_link_service *self)
{
    return self->sls_sending_queue_len;
}

struct sls_message* simplelinkservice_get_sls_sending_queue_elem(struct simple_link_service *self,int _index)
{
    ASSERT(self->sls_sending_queue);
    ASSERT(_index >= 0 && _index < self->sls_sending_queue_len);
    return self->sls_sending_queue[_index];
}

status_t simplelinkservice_alloc_sls_sending_queue(struct simple_link_service *self,int _len)
{
    if(self->sls_sending_queue_size == _len)
        return OK;

    X_FREE(self->sls_sending_queue);
    
    if(_len > 0)
    {
        X_MALLOC(self->sls_sending_queue,struct sls_message*,_len);
        memset(self->sls_sending_queue,0,sizeof(struct sls_message*)*_len);
    }
    self->sls_sending_queue_size = _len;
    return OK;
}

status_t simplelinkservice_set_sls_sending_queue(struct simple_link_service *self,struct sls_message *_sls_sending_queue[], int _len)
{
    ASSERT(_sls_sending_queue);
    simplelinkservice_alloc_sls_sending_queue(self,_len);
    self->sls_sending_queue = _sls_sending_queue;
    return OK;
}

status_t simplelinkservice_set_sls_sending_queue_elem(struct simple_link_service *self,int _index,struct sls_message *_sls_sending_queue)
{
    ASSERT(self->sls_sending_queue);
    ASSERT(_index >= 0 && _index < self->sls_sending_queue_size);
    self->sls_sending_queue[_index] = _sls_sending_queue;
    return OK;
}

status_t simplelinkservice_send_message(struct simple_link_service *self,struct sls_message *msg)
{
    ASSERT(msg);
    ASSERT(self->sls_sending_queue_len < self->sls_sending_queue_size);
    ASSERT(sls_message_check(msg));
    simplelinkservice_set_sls_sending_queue_elem(self,
        self->sls_sending_queue_len, msg
    );
    self->sls_sending_queue_len ++;
    return OK;
}

status_t simplelinkservice_delete_message(struct simple_link_service *self, int index)
{    
    struct sls_message *p;
    int k;

    ASSERT(index >= 0 && index < self->sls_sending_queue_len);
    p = simplelinkservice_get_sls_sending_queue_elem(self,index);   
    for(k = index; k < self->sls_sending_queue_len-1; k++)
    {
        self->sls_sending_queue[k] = self->sls_sending_queue[k + 1];
    }
    self->sls_sending_queue_len --;
    self->sls_sending_queue[self->sls_sending_queue_len] = NULL;

    sls_message_destroy(p);
    X_FREE(p);
    return OK;
}

struct task_link_rpc *simplelinkservice_get_task_link_rpc(struct simple_link_service *self)
{
    struct task *pt;
    ASSERT(self->task_mgr);
    pt = taskmgr_get_task(self->task_mgr,self->task_link_rpc);
    do{
        CONTAINER_OF(struct task_link_rpc,task,pt,base_task);
        return task;
    }while(0);
    return NULL;
}

C_BEGIN_CLOSURE_FUNC(on_tcp_connector_event)
{
    int event;
    struct simple_link_service *self;

    C89_CLOSURE_PARAM_INT(event,0);
    C89_CLOSURE_PARAM_PTR(struct simple_link_service*,self,10);
    
    if(event == C_TASK_TCP_CONNECTOR_EVENT_STOP)
    {
        int err;
        C89_CLOSURE_PARAM_INT(err,1);
        if(err != C_TASK_TCP_CONNECTOR_ERROR_NONE)
        {
            struct task_link_rpc *task = 
                simplelinkservice_get_task_link_rpc(self);
            ASSERT(task);
            tasklinkrpc_retry(task,C_TASK_LINKRPC_ERROR_CONNECT_ERROR);
        }
    }
    else if(event == C_TASK_TCP_CONNECTOR_EVENT_CONNECTED)
    {
        struct tcp_client *client;
        struct task_link_rpc *task_link_rpc;

        C89_CLOSURE_PARAM_PTR(struct tcp_client*,client,2);
        
        task_link_rpc = simplelinkservice_get_task_link_rpc(self);
        ASSERT(task_link_rpc);
        ASSERT(task_link_rpc->socket);
        socket_transfer_socket_fd(task_link_rpc->socket,&client->base_socket);
    }
    return OK;
}
C_END_CLOSURE_FUNC(on_tcp_connector_event)

static status_t simplelinkservice_start_tcp_connector(struct simple_link_service *self)
{
    struct taskmgr *mgr = self->task_mgr;
    struct task_tcp_connector *connector;
    ASSERT(!taskmgr_is_task(mgr,self->task_tcp_connector));
    X_MALLOC(connector,struct task_tcp_connector,1);
    tasktcpconnector_init(connector,mgr);
    tasktcpconnector_set_server_name(connector,self->server_name);
    tasktcpconnector_set_port(connector,self->port);
    tasktcpconnector_start(connector);
    closure_set_func(&connector->callback,on_tcp_connector_event);
    closure_set_param_pointer(&connector->callback,10,self);
    self->task_tcp_connector = task_get_id(&connector->base_task);
    return OK;
}

status_t simplelinkservice_set_server_name(struct simple_link_service *self,const char *_server_name)
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

status_t simplelinkservice_set_port(struct simple_link_service *self,int _port)
{
    self->port = _port;
    return OK;
}

status_t simplelinkservice_transfer_socket_fd(struct simple_link_service *self,int fd)
{
    struct socket tmp;
    struct task_link_rpc *pt;
    status_t ret;
    pt = simplelinkservice_get_task_link_rpc(self);
    ASSERT(pt);
    socket_init(&tmp);
    socket_set_socket_num(&tmp,fd);    
    ret = tasklinkrpc_transfer_socket(pt,&tmp);
    socket_destroy(&tmp);
    return ret;
}

status_t simplelinkservice_is_connected(struct simple_link_service *self)
{
    ASSERT(self->task_mgr);
    return taskmgr_is_task(self->task_mgr,self->task_link_rpc);
}

static status_t simplelinkservice_on_got_message(struct simple_link_service *self,LINKRPC_HEADER *header, struct mem *header_data, struct file_base *data)
{
    ASSERT(header && header_data && data);
    closure_set_param_pointer(&self->callback,1,header);
    closure_set_param_pointer(&self->callback,2,header_data);
    closure_set_param_pointer(&self->callback,3,&self->data_recv_buf);
    closure_run_event(&self->callback,C_SIMPLE_LINK_SERVICE_EVENT_GOT_MESSAGE);
    return OK;
}
