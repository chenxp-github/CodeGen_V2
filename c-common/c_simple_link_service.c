#include "c_simple_link_service.h"
#include "syslog.h"
#include "mem_tool.h"

status_t simplelinkservice_init_basic(struct simple_link_service *self)
{
    self->task_mgr = NULL;
    self->task_link_rpc = 0;
    closure_init_basic(&self->callback);
    mem_init_basic(&self->header_recv_buf);
    mem_init_basic(&self->data_recv_buf);
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

        mem_free(&self->data_recv_buf);
        mem_malloc(&self->data_recv_buf,header->data_size);
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