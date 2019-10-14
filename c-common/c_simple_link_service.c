#include "c_simple_link_service.h"
#include "syslog.h"
#include "mem_tool.h"

status_t sls_message_init_basic(struct sls_message *self)
{
    self->header_data = NULL;
    self->data = NULL;
    self->linkrpc_msg_type = 0;    
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
        X_VIRTUAL_DELETE(self->data,filebase_destroy);
    }
    sls_message_init_basic(self);
    return OK;
}
/**************************************************************/
status_t simplelinkservice_init_basic(struct simple_link_service *self)
{
    self->task_mgr = NULL;
    self->task_link_rpc = 0;
    closure_init_basic(&self->callback);
    mem_init_basic(&self->header_recv_buf);
    mem_init_basic(&self->data_recv_buf);
    self->sls_sending_queue = NULL;
    self->sls_sending_queue_len = 0;
    self->sls_sending_queue_len = 0;
    return OK;
}

status_t simplelinkservice_init(struct simple_link_service *self, struct taskmgr *mgr)
{
    simplelinkservice_init_basic(self);
    self->task_mgr = mgr;
    closure_init(&self->callback);
    mem_init(&self->header_recv_buf);
    mem_init(&self->data_recv_buf);
    return OK;
}

status_t simplelinkservice_destroy(struct simple_link_service *self)
{
    if(self->sls_sending_queue)
    {
        int i;
        for(i = 0; i < self->sls_sending_queue_len; i++)
        {
            sls_message_destroy(self->sls_sending_queue[i]);
            X_FREE(self->sls_sending_queue[i]);
        }       
        X_FREE(self->sls_sending_queue);
    }

    if(self->task_mgr)
    {
        taskmgr_quit_task(self->task_mgr,&self->task_link_rpc);
    } 
    mem_destroy(&self->header_recv_buf);
    mem_destroy(&self->data_recv_buf);
    closure_destroy(&self->callback);
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

    else if(event == C_TASK_LINKRPC_EVENT_PREPARE_DATA_TO_SEND)
    {
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
    }

    else if(event == C_TASK_LINKRPC_EVENT_PACKAGE_SEND_OK)
    {
    }
    return OK;
}
C_END_CLOSURE_FUNC(on_task_link_rpc_event);

status_t simplelinkservice_start(struct simple_link_service *self)
{
    struct task_link_rpc *pt;
    ASSERT(self->task_mgr);
    ASSERT(!taskmgr_is_task(self->task_mgr,self->task_link_rpc));
    
    mem_free(&self->header_recv_buf);
    mem_malloc(&self->header_recv_buf,4096);
    mem_free(&self->data_recv_buf);

    X_MALLOC(pt,struct task_link_rpc,1);
    tasklinkrpc_init(pt,self->task_mgr);    
    tasklinkrpc_set_header_buf(pt,&self->header_recv_buf);
    tasklinkrpc_set_data_buf(pt,&self->data_recv_buf.base_file_base);
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
    ASSERT(_index >= 0 && _index < self->sls_sending_queue_len);
    self->sls_sending_queue[_index] = _sls_sending_queue;
    return OK;
}

status_t simplelinkservice_send_message(struct simple_link_service *self,struct sls_message *msg)
{
    ASSERT(msg);
    ASSERT(self->sls_sending_queue_len < self->sls_sending_queue_size);

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
