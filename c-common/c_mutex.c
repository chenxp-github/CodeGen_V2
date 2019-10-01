#include "c_mutex.h"
#include "syslog.h"
#include "mem_tool.h"

status_t mutex_init_basic(struct mutex *self)
{
    self->initialized = 0;
    return OK;
}

status_t mutex_init(struct mutex *self)
{
    if(!self->initialized)
    {
        crt_init_mutex(&self->lock);
        self->initialized = 1;
    }
    return OK;
}

status_t mutex_destroy(struct mutex *self)
{
    if(self->initialized)
    {
        crt_destroy_mutex(&self->lock);
    }
    mutex_init_basic(self);
    return OK;
}

status_t mutex_lock(struct mutex *self)
{
    ASSERT(self->initialized);
    crt_lock_mutex(&self->lock);
    return OK;
}

status_t mutex_unlock(struct mutex *self)
{
    ASSERT(self->initialized);
    crt_unlock_mutex(&self->lock);
    return OK;
}



