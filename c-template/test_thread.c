#include "test_thread.h"
#include "syslog.h"
#include "mem_tool.h"

THREAD_VIRTUAL_FUNCTIONS_DEFINE(struct test_thread,testthread)
/******************************************************************/
void* testthread_get_this_pointer(struct test_thread *self)
{
    return (void*)self;
}

status_t testthread_init_basic(struct test_thread *self)
{
    thread_init_basic(&self->base_thread);
    return OK;
}

status_t testthread_init(struct test_thread *self)
{
    testthread_init_basic(self);
    thread_init(&self->base_thread);
    THREAD_INIT_VIRTUAL_FUNCTIONS(testthread);
    return OK;
}

status_t testthread_destroy(struct test_thread *self)
{
    thread_base_destroy(&self->base_thread);
    testthread_init_basic(self);
    return OK;
}

status_t testthread_run(struct test_thread *self)
{
    int i;
    for(i = 0; i<10; i++)
    {
        LOG("here %d",i);
        thread_sleep(&self->base_thread,1000);
    }
    return OK;
}