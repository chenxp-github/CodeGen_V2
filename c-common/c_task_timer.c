#include "c_task_timer.h"
#include "syslog.h"
#include "mem_tool.h"

TASK_VIRTUAL_FUNCTIONS_DEFINE(struct task_timer,tasktimer)
/************************************************************/
status_t tasktimer_init_basic(struct task_timer *self)
{
    task_init_basic(&self->base_task);
    self->timeout = 0;
    self->one_shot = 0;
    self->step = 0;
    closure_init_basic(&self->callback);
    return OK;
}

status_t tasktimer_init(struct task_timer *self,struct taskmgr *mgr)
{
    ASSERT(mgr);
    tasktimer_init_basic(self);
    task_init(&self->base_task,mgr);
    closure_init(&self->callback);
    TASK_INIT_VIRTUAL_FUNCTIONS(tasktimer);
    return OK;
}

status_t tasktimer_destroy(struct task_timer *self)
{
    task_base_destroy(&self->base_task);
    closure_destroy(&self->callback);
    tasktimer_init_basic(self);
    return OK;
}

uint32_t tasktimer_get_timeout(struct task_timer *self)
{
    return self->timeout;
}

bool_t tasktimer_get_one_shot(struct task_timer *self)
{
    return self->one_shot;
}

struct closure* tasktimer_get_callback(struct task_timer *self)
{
    return &self->callback;
}


status_t tasktimer_set_timeout(struct task_timer *self,uint32_t _timeout)
{
    self->timeout = _timeout;
    return OK;
}

status_t tasktimer_set_one_shot(struct task_timer *self,bool_t _one_shot)
{
    self->one_shot = _one_shot;
    return OK;
}

status_t tasktimer_start(struct task_timer *self)
{
    task_resume(&self->base_task);
    self->step = 1;
    return OK;
}

status_t tasktimer_stop(struct task_timer *self,int err)
{
    if(task_is_dead(&self->base_task))
        return ERROR;
    task_quit(&self->base_task);
    return OK;
}

status_t tasktimer_run(struct task_timer *self, uint32_t interval)
{
    self->base_task.sleep_time += interval;
    if(self->base_task.sleep_time >= self->timeout)
    {
        closure_set_param_pointer(&self->callback,1,self);
        closure_run_event(&self->callback,self->base_task.sleep_time);        
        self->base_task.sleep_time = 0;
        if(self->one_shot)
        {
            tasktimer_stop(self,0);
        }
    }
    return OK;
}

struct task_timer* tasktimer_new(struct taskmgr *mgr,uint32_t timeout,bool_t one_shot)
{
    struct task_timer *pt;
    ASSERT(mgr);
    X_MALLOC(pt,struct task_timer,1);
    tasktimer_init(pt,mgr);
    tasktimer_set_timeout(pt,timeout);
    tasktimer_set_one_shot(pt,one_shot);    
    tasktimer_start(pt);  
    return pt;
}

