#ifndef __TASK_TEST_H
#define __TASK_TEST_H

#include "c_taskmgr.h"

#define TASK_TEST_ERROR_NONE   0

struct task_test{
	struct task base_task;
	int step;
};

status_t task_test_init_basic(struct task_test *self);
status_t task_test_init(struct task_test *self,struct taskmgr *mgr);
status_t task_test_destroy(struct task_test *self);
status_t task_test_report_error(struct task_test *self,int err);
status_t task_test_start(struct task_test *self);
status_t task_test_stop(struct task_test *self,int err);
status_t task_test_run(struct task_test *self, uint32_t interval);

#endif
