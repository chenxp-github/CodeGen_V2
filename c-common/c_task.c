#include "c_task.h"
#include "syslog.h"
#include "mem_tool.h"
#include "c_taskmgr.h"

#define TASK_IS_RUNNING     0x00000001
#define TASK_IS_DELETED     0x00000002
#define TASK_IS_SLEEPING    0x00000004

status_t task_init_basic(struct task *self)
{
    self->run = NULL;  
    self->destroy = NULL;
    self->id = 0;
    self->type = 0;
    self->last_sleep_time = 0;
    self->sleep_time = 0;
    self->total_sleep_time = 0;
    self->last_run_time = 0;
    self->flags = 0;
    self->taskmgr = NULL;

    return OK;
}
status_t task_init(struct task *self,struct taskmgr *mgr)
{
    task_init_basic(self);

    self->last_run_time = crt_get_sys_timer();
    self->last_sleep_time = self->last_run_time;

    if(mgr)
    {
        self->id = taskmgr_alloc_id(mgr);
        taskmgr_add_task(mgr,self);
        self->taskmgr = mgr;
    }

    return OK;
}

status_t task_base_destroy(struct task *self)
{
    task_init_basic(self);
    return OK;
}

status_t task_destroy(struct task *self)
{
    if(self->destroy)
    {
        return self->destroy(self);
    }
    task_base_destroy(self);
    return OK;
}

status_t task_virtual_free(struct task *self)
{
    if(self->virtual_free)
    {
        return self->virtual_free(self);
    }
    return ERROR;
}

status_t task_quit(struct task *self)
{
    self->flags |= TASK_IS_DELETED;
    return OK;
}
status_t task_suspend(struct task *self)
{
    self->flags &= (~TASK_IS_RUNNING);
    return OK;
}
status_t task_resume(struct task *self)
{
    self->flags |= TASK_IS_RUNNING;
    return OK;
}
bool_t task_is_running(struct task *self)
{
    if(task_is_dead(self))return FALSE;
    return (self->flags&TASK_IS_RUNNING) != 0;
}
int task_get_type(struct task *self)
{
    return self->type;
}
status_t task_set_type(struct task *self,int type)
{
    self->type = type;
    return OK;
}
int task_get_id(struct task *self)
{
    return self->id;
}
bool_t task_is_dead(struct task *self)
{
    return (self->flags & TASK_IS_DELETED) != 0;
}

bool_t task_is_sleeping(struct task *self,uint32_t interval)
{
    if(self->flags & TASK_IS_SLEEPING)
    {
        self->sleep_time += interval;
        if(self->sleep_time >= self->total_sleep_time)
        {
            self->flags &= (~TASK_IS_SLEEPING);
            self->sleep_time = 0;
            self->total_sleep_time = 0;
        }
        return TRUE;
    }
    return FALSE;
}

status_t task_sleep(struct task *self,uint32_t ms)
{
    if(ms > 0)
    {
        self->flags |= TASK_IS_SLEEPING;
        self->sleep_time = 0;
        self->total_sleep_time = ms;
    }
    return OK;
}
struct taskmgr* task_get_taskmgr(struct task *self)
{
    return self->taskmgr;
}
status_t task_set_taskmgr(struct task *self,struct taskmgr* taskmgr)
{
    self->taskmgr = taskmgr;
    return OK;
}

status_t task_run(struct task *self,int interval)
{
    ASSERT(self->run);
    return self->run(self,interval);
}

bool_t task_is_invalid_interval(struct task *self,uint32_t interval)
{
    //system time maybe adjusted.
    return self->total_sleep_time > 10 && interval > self->total_sleep_time*100;
}

