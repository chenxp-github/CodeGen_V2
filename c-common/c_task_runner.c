#include "c_task_runner.h"
#include "syslog.h"
#include "mem_tool.h"

struct closure_extra_info{
    uint32_t total_delay;
    uint32_t cur_delay;
};

status_t taskrunner_init_basic(struct task_runner *self)
{
    closurelist_init_basic(&self->closure_list);
    mutex_init_basic(&self->mutex);
    self->last_time_val = 0;
    return OK;
}

status_t taskrunner_init(struct task_runner *self)
{
    taskrunner_init_basic(self);
    closurelist_init(&self->closure_list,1024);
    mutex_init(&self->mutex);
    self->last_time_val = crt_get_sys_timer();
    return OK;
}

status_t taskrunner_destroy(struct task_runner *self)
{
    int i;
    
    for(i = 0; i < closurelist_get_len(&self->closure_list); i++)
    {
        struct closure *c = closurelist_get_elem(&self->closure_list,i);
        X_FREE(c->_user_data);
    }

    closurelist_destroy(&self->closure_list);
    mutex_destroy(&self->mutex);
    taskrunner_init_basic(self);
    return OK;
}

status_t taskrunner_add_closure(struct task_runner *self,struct closure *closure,uint32_t delay)
{
    status_t ret = ERROR;
    struct closure_extra_info *info;
    ASSERT(closure);
    ASSERT(closure->_is_on_heap);
    ASSERT(closure->_user_data == NULL);

    X_MALLOC(info,struct closure_extra_info,1);
    info->cur_delay = 0;
    info->total_delay = delay;
    closure->_user_data = info;

    mutex_lock(&self->mutex);
    ret = closurelist_push_ptr(&self->closure_list,closure);
    mutex_unlock(&self->mutex);
    return ret;
}

int taskrunner_schedule(struct task_runner *self)
{
    uint32_t now = crt_get_sys_timer();
    uint32_t interval = now - self->last_time_val;
    int i,has_zero_delay_tasks = 0;
    self->last_time_val = now;
    
    for(i = 0; i < closurelist_get_len(&self->closure_list); i++)
    {
        struct closure *closure = closurelist_get_elem(&self->closure_list,i);
        struct closure_extra_info *info = (struct closure_extra_info *)closure->_user_data;
        ASSERT(info);
        
        if(info->cur_delay == 0)
            has_zero_delay_tasks++;
        
        info->cur_delay += interval;
        if(info->cur_delay >= info->total_delay)
        {            
            closure_run(closure);
            mutex_lock(&self->mutex);
            X_FREE(closure->_user_data);
            closurelist_del_elem(&self->closure_list,i);
            mutex_unlock(&self->mutex);            
            i--;
        }
    }
    
    return has_zero_delay_tasks;
}


