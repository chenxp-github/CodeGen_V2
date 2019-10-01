#include "task_test.h"
#include "mem_tool.h"
#include "syslog.h"

static status_t task_test_how_to_destroy(struct task *base)
{
	CONTAINER_OF(struct task_test, self, base, base_task);
    return task_test_destroy(self);
}

static status_t task_test_how_to_run(struct task *base, uint32_t interval)
{	
    CONTAINER_OF(struct task_test, self, base, base_task);
    return task_test_run(self,interval);
}
/*********************************************/
status_t task_test_init_basic(struct task_test *self)
{
	task_init_basic(&self->base_task);
	self->step = 0;
	return OK;
}
status_t task_test_init(struct task_test *self,struct taskmgr *mgr)
{
	task_test_init_basic(self);
	task_init(&self->base_task,mgr);
    self->base_task.run = task_test_how_to_run;
    self->base_task.destroy = task_test_how_to_destroy;
	return OK;
}
status_t task_test_destroy(struct task_test *self)
{
	task_test_init_basic(self);
	return OK;
}

status_t task_test_report_error(struct task_test *self,int err)
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

status_t task_test_start(struct task_test *self)
{
	task_resume(&self->base_task);
    self->step = 1;
	return OK;
}

status_t task_test_stop(struct task_test *self,int err)
{
	if(task_is_dead(&self->base_task))
		return ERROR;
	task_quit(&self->base_task);
	task_test_report_error(self,err);
	return OK;
}

status_t task_test_run(struct task_test *self, uint32_t interval)
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
