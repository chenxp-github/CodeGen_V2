#include "c_thread.h"
#include "syslog.h"
#include "mem_tool.h"

status_t thread_init_basic(struct thread *self)
{
    self->handle = 0;
    self->running = 0;
    self->run = NULL;
    self->destroy = NULL;
    self->get_this_pointer = NULL;
    return OK;
}

status_t thread_init(struct thread *self)
{
    thread_init_basic(self);
    return OK;
}

status_t thread_base_destroy(struct thread *self)
{
    thread_stop(self);
    thread_wait_complete(self,10000);
    thread_init_basic(self);
    return OK;
}

status_t thread_destroy(struct thread *self)
{
    if(self->destroy)
    {
        self->destroy(self);
    }
    return thread_base_destroy(self);
}

static void _thread_proc(void *lpvoid)
{
    struct thread *self;
    VASSERT(lpvoid);
    self = (struct thread*)lpvoid;
    if(self->run)
    {
        self->run(self);
    }
    self->running = FALSE;
    self->handle = 0;
}

status_t thread_start(struct thread *self)
{
    ASSERT(!self->running);
    ASSERT(self->handle == 0);
    self->running = TRUE;
    self->handle = crt_create_thread(_thread_proc, 1024*1024,self);
    return self->handle != 0;
}

bool_t thread_is_complete(struct thread *self)
{
    return self->handle == 0;
}

status_t thread_wait_complete(struct thread *self,int timeout)
{
    int t = 0,delay = 10;
    while(!thread_is_complete(self))
    {
        crt_msleep(delay);
        t += delay;
        if(t > timeout)
        {
            XLOG(LOG_MODULE_COMMON,LOG_LEVEL_WARNING,
                "thread wait_complete timeout: %d",t 
            );
            delay = 1000;
        }
    }
    return OK;
}

bool_t thread_is_running(struct thread *self)
{
    return self->running != 0;
}

status_t thread_stop(struct thread *self)
{
    self->running = 0;
    return OK;
}
status_t thread_cancel(struct thread *self)
{
    THREAD_HANDLE old_handle = self->handle;
    self->handle = 0;
    self->running = 0;
    return crt_cancel_thread(old_handle);
}

status_t thread_sleep(struct thread *self,int ms)
{
    int t = 0;
    if(ms <= 50)
    {
        crt_msleep(ms);
        return OK;
    } 
        
    while(self->running && t < ms)
    {
        crt_msleep(50);
        t += 50;
    }
    
    return OK;
}
