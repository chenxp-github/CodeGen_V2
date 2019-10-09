#include "task_test.h"
#include "mem_tool.h"
#include "syslog.h"

TASK_VIRTUAL_FUNCTIONS_DEFINE(struct task_test,tasktest)
/*********************************************/
status_t tasktest_init_basic(struct task_test *self)
{
	task_init_basic(&self->base_task);
	self->step = 0;
	return OK;
}
status_t tasktest_init(struct task_test *self,struct taskmgr *mgr)
{
	tasktest_init_basic(self);
	task_init(&self->base_task,mgr);
    TASK_INIT_VIRTUAL_FUNCTIONS(tasktest);
	return OK;
}

status_t tasktest_destroy(struct task_test *self)
{
    task_base_destroy(&self->base_task);
	tasktest_init_basic(self);
	return OK;
}

status_t tasktest_report_error(struct task_test *self,int err)
{
	const char *err_str="unknown error";
	switch(err)
	{
		case TASK_TEST_ERROR_NONE: 
			err_str = "none"; 
		break;
	}
	XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,
        "task_test(%d): exit with error \"%s\"",
		task_get_id(&self->base_task),err_str);
	return OK;
}

status_t tasktest_start(struct task_test *self)
{
	task_resume(&self->base_task);
    self->step = 1;
	return OK;
}

status_t tasktest_stop(struct task_test *self,int err)
{
	if(task_is_dead(&self->base_task))
		return ERROR;
	task_quit(&self->base_task);
	tasktest_report_error(self,err);
	return OK;
}

status_t tasktest_run(struct task_test *self, uint32_t interval)
{
	C_BEGIN_FSM();
    /**********************************/
    
    C_BEGIN_STATE(1)
    {
        LOG("Hello");
        C_GOTO_STATE(1000,2);
    }
    C_END_STATE(1);
    //////////////

    C_BEGIN_STATE(2)
    {
        LOG("World");
        C_GOTO_STATE(1000,1);
    }
    C_END_STATE(2);
    /**********************************/
	C_END_FSM();
	return OK;
}
