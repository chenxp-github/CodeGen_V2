#ifndef _C_TASK_RUNNER_H
#define _C_TASK_RUNNER_H

#include "cruntime.h"
#include "c_log_buffer.h"
#include "c_closure_list.h"
#include "c_mutex.h"

struct task_runner{
    struct closure_list closure_list;
    struct mutex mutex;
    uint32_t last_time_val;
};

status_t taskrunner_init_basic(struct task_runner *self);
status_t taskrunner_init(struct task_runner *self);
status_t taskrunner_destroy(struct task_runner *self);
status_t taskrunner_add_closure(struct task_runner *self,struct closure *closure,uint32_t delay);
int taskrunner_schedule(struct task_runner *self);

#endif
