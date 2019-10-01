#ifndef _C_MUTEX_H
#define _C_MUTEX_H

#include "cruntime.h"
#include "c_log_buffer.h"

struct mutex{
    MUTEX_TYPE lock;
    int initialized;
};

status_t mutex_init_basic(struct mutex *self);
status_t mutex_init(struct mutex *self);
status_t mutex_destroy(struct mutex *self);
status_t mutex_lock(struct mutex *self);
status_t mutex_unlock(struct mutex *self);

#endif
