#include "c_weak_ptr.h"
#include "syslog.h"
#include "mem_tool.h"

status_t weakptr_init_basic(struct weak_ptr *self)
{
    self->raw_ptr = NULL;
    self->weak_ref_id = 0;
    return OK;
}

status_t weakptr_init(struct weak_ptr *self)
{
    weakptr_init_basic(self);
    return OK;
}

status_t weakptr_destroy(struct weak_ptr *self)
{
    weakptr_init_basic(self);
    return OK;
}

status_t weakptr_copy(struct weak_ptr *self,struct weak_ptr *_p)
{
    ASSERT(_p);
    if(self == _p)return OK;

    self->raw_ptr = _p->raw_ptr;
    self->weak_ref_id = _p->weak_ref_id;
    return OK;
}

status_t weakptr_comp(struct weak_ptr *self,struct weak_ptr *_p)
{
    ASSERT(_p);
    if(self == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t weakptr_print(struct weak_ptr *self,struct log_buffer *_buf)
{
    ASSERT(_buf);
    log_buffer_log(_buf,"raw_ptr = 0x%p",self->raw_ptr);
    log_buffer_log(_buf,"weak_ref_id = %d",self->weak_ref_id);
    return OK;
}


status_t weakptr_ref(struct weak_ptr *self, void *obj, int weak_ref_id)
{
    self->raw_ptr = obj;
    self->weak_ref_id = weak_ref_id;
    return OK;
}

void* weakptr_get(struct weak_ptr *self, int weak_ref_id)
{
    if(self->weak_ref_id == 0)
        return NULL;
    if(self->weak_ref_id != weak_ref_id)
        return NULL;
    return self->raw_ptr;
}
