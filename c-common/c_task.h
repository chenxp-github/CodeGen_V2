#ifndef __C_TASK_H
#define __C_TASK_H

#include "cruntime.h"

#define C_BEGIN_FSM() switch(self->step){
#define C_END_FSM() case 0:default:task_suspend(&self->base_task);break;}

#define C_BEGIN_STATE(state) case state:{
#define C_END_STATE(state) }break

#define C_GOTO_STATE(delay,state) do{task_sleep(&self->base_task,delay);self->step = state;}while(0)
#define C_CONTINUE(ms) do{task_sleep(&self->base_task,ms);return ERROR;}while(0)

struct taskmgr;

struct task{
    status_t (*run)(struct task *self,uint32_t interval);
    status_t (*destroy)(struct task *self);
    
    struct taskmgr *taskmgr;
    int id;
    int type;
    uint32_t last_sleep_time;
    uint32_t sleep_time;
    uint32_t total_sleep_time;
    uint32_t last_run_time;
    uint32_t flags;
};

status_t task_init_basic(struct task *self);
status_t task_init(struct task *self,struct taskmgr *mgr);
status_t task_destroy(struct task *self);
status_t task_quit(struct task *self);
status_t task_suspend(struct task *self);
status_t task_resume(struct task *self);
bool_t task_is_running(struct task *self);
int task_get_type(struct task *self);
status_t task_set_type(struct task *self,int type);
int task_get_id(struct task *self);
bool_t task_is_dead(struct task *self);
bool_t task_is_sleeping(struct task *self,uint32_t interval);
status_t task_sleep(struct task *self,uint32_t ms);
struct taskmgr* task_get_taskmgr(struct task *self);
status_t task_set_taskmgr(struct task *self,struct taskmgr* taskmgr);
status_t task_run_callback(struct task *self,int event);
status_t task_run(struct task *self,int interval);

#endif
