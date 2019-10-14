#ifndef _C_THREAD_H
#define _C_THREAD_H

#include "cruntime.h"
#include "c_log_buffer.h"

#define THREAD_INIT_VIRTUAL_FUNCTIONS(prefix) do{\
    self->base_thread.run = prefix##_virtual_run;\
    self->base_thread.destroy = prefix##_virtual_destroy;\
    self->base_thread.get_this_pointer = prefix##_virtual_get_this_pointer;\
}while(0)\

#define THREAD_VIRTUAL_FUNCTIONS_DEFINE(child_type, prefix)\
static status_t prefix##_virtual_run(struct thread *base)\
{\
    CONTAINER_OF(child_type,self,base,base_thread);\
    return prefix##_run(self);\
}\
static status_t prefix##_virtual_destroy(struct thread *base)\
{\
    CONTAINER_OF(child_type,self,base,base_thread);\
    return prefix##_destroy(self);\
}\
static void* prefix##_virtual_get_this_pointer(struct thread *base)\
{\
    CONTAINER_OF(child_type,self,base,base_thread);\
    return prefix##_get_this_pointer(self);\
}\

struct thread{
    THREAD_HANDLE handle;
    bool_t running;
    status_t (*run)(struct thread *self);
    status_t (*destroy)(struct thread *self);
    void* (*get_this_pointer)(struct thread *self);
};

status_t thread_init_basic(struct thread *self);
status_t thread_init(struct thread *self);
status_t thread_base_destroy(struct thread *self);
status_t thread_destroy(struct thread *self);
status_t thread_start(struct thread *self);
bool_t thread_is_complete(struct thread *self);
status_t thread_wait_complete(struct thread *self,int timeout);
bool_t thread_is_running(struct thread *self);
status_t thread_stop(struct thread *self);
status_t thread_cancel(struct thread *self);
status_t thread_sleep(struct thread *self,int ms);


#endif
