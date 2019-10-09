#ifndef _C_TASK_TIMER_H
#define _C_TASK_TIMER_H

#include "cruntime.h"
#include "c_log_buffer.h"
#include "c_task.h"
#include "c_closure.h"
#include "c_taskmgr.h"

struct task_timer{
    struct task base_task;

    uint32_t timeout;
    bool_t one_shot;
    int step;
    struct closure callback;
};

status_t tasktimer_init_basic(struct task_timer *self);
status_t tasktimer_init(struct task_timer *self,struct taskmgr *mgr);
status_t tasktimer_destroy(struct task_timer *self);

uint32_t tasktimer_get_timeout(struct task_timer *self);
bool_t tasktimer_get_one_shot(struct task_timer *self);
struct closure* tasktimer_get_callback(struct task_timer *self);

status_t tasktimer_set_timeout(struct task_timer *self,uint32_t _timeout);
status_t tasktimer_set_one_shot(struct task_timer *self,bool_t _one_shot);

status_t tasktimer_report_error(struct task_timer *self,int err);
status_t tasktimer_start(struct task_timer *self);
status_t tasktimer_stop(struct task_timer *self,int err);
status_t tasktimer_run(struct task_timer *self, uint32_t interval);
struct task_timer * tasktimer_new(struct taskmgr *mgr,uint32_t timeout,bool_t one_shot);

#endif
