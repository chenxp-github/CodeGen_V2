#ifndef _C_TEST_THREAD_H
#define _C_TEST_THREAD_H

#include "cruntime.h"
#include "c_log_buffer.h"
#include "c_thread.h"

struct test_thread{
    struct thread base_thread;
};

status_t testthread_init_basic(struct test_thread *self);
status_t testthread_init(struct test_thread *self);
status_t testthread_destroy(struct test_thread *self);
status_t testthread_run(struct test_thread *self);

#endif
