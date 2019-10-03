#ifndef _C_WEAK_PTR_H
#define _C_WEAK_PTR_H

#include "cruntime.h"
#include "c_log_buffer.h"

#define WEAK_PTR_REF(wptr,ptr) do{\
if(ptr)weakptr_ref(&wptr,ptr,(ptr)->__weak_ref_id);\
}while(0)\
    
#define WEAK_PTR_GET(wptr,type,ptr)\
type* ptr = NULL;\
do{\
    type *_p = (type*)(wptr).raw_ptr;\
    if(_p != NULL)\
    {\
        ptr = (type*)weakptr_get(&(wptr),_p->__weak_ref_id);\
    }\
}while(0)\

struct weak_ptr{
    void *raw_ptr;
    int weak_ref_id;
};

status_t weakptr_init_basic(struct weak_ptr *self);
status_t weakptr_init(struct weak_ptr *self);
status_t weakptr_destroy(struct weak_ptr *self);
status_t weakptr_copy(struct weak_ptr *self,struct weak_ptr *_p);
status_t weakptr_comp(struct weak_ptr *self,struct weak_ptr *_p);
status_t weakptr_print(struct weak_ptr *self,struct log_buffer *_buf);
status_t weakptr_ref(struct weak_ptr *self, void *obj, int weak_ref_id);
void* weakptr_get(struct weak_ptr *self, int weak_ref_id);

#endif
