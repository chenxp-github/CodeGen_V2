#ifndef __S2_C_CLOSURE_H
#define __S2_C_CLOSURE_H

#include "cruntime.h"
#include "c_log_buffer.h"
#include "c_closure_c89.h"

#define CCLOSURE_MAX_PARAMS 16

#ifdef __cplusplus
extern "C" {
#endif

enum{
    C_PARAM_TYPE_UNKNOWN = 0,
    C_PARAM_TYPE_POINTER,
    C_PARAM_TYPE_INT,
    C_PARAM_TYPE_INT64,
    C_PARAM_TYPE_DOUBLE,
    C_PARAM_TYPE_FLOAT,
    C_PARAM_TYPE_STRING,
    C_PARAM_TYPE_MALLOC,
    C_PARAM_TYPE_WEAKPTR,
};

#define __CCLOSURE_PARAM_PTR_CAN_BE_NULL(closure,type,var,n) \
ASSERT(closure_get_param_type(closure,n) == C_PARAM_TYPE_POINTER \
||closure_get_param_type(closure,n) == C_PARAM_TYPE_MALLOC);\
type var = (type)closure_get_param_pointer(closure,n)

#define __CCLOSURE_PARAM_PTR(closure,type,var,n) \
__CCLOSURE_PARAM_PTR_CAN_BE_NULL(closure,type,var,n);\
ASSERT(var) \
    
#define __CCLOSURE_PARAM_INT(closure,var,n) \
ASSERT(closure_get_param_type(closure,n) == C_PARAM_TYPE_INT);\
int var = closure_get_param_int(closure,n)\
    
#define __CCLOSURE_PARAM_INT64(closure,var,n) \
ASSERT(closure_get_param_type(closure,n) == C_PARAM_TYPE_INT64);\
int64_t var = closure_get_param_int64(closure,n) \

#define __CCLOSURE_PARAM_FLOAT(closure,var,n) \
ASSERT(closure_get_param_type(closure,n) == C_PARAM_TYPE_FLOAT);\
float var = closure_get_param_float(closure,n)

#define __CCLOSURE_PARAM_DOUBLE(closure,var,n) \
ASSERT(closure_get_param_type(closure,n) == C_PARAM_TYPE_DOUBLE);\
double var = closure_get_param_double(closure,n) \
    
#define __CCLOSURE_PARAM_STRING(closure,var,n) \
ASSERT(closure_get_param_type(closure,n) == C_PARAM_TYPE_STRING);\
const char* var = closure_get_param_string(closure,n) \
    
#define __CCLOSURE_PARAM_WEAKPTR(closure,type,var,n) \
int __weak_ref_id_##var; \
ASSERT(closure_get_param_type(closure,n) == C_PARAM_TYPE_WEAKPTR); \
type var = (type)closure_get_param_weak_pointer(closure,n,&__weak_ref_id_##var);\
ASSERT(var && var->__weak_ref_id != 0 && var->__weak_ref_id == __weak_ref_id_##var)

#define C_CLOSURE_PARAM_PTR_CAN_BE_NULL(type,var,n)  __CCLOSURE_PARAM_PTR_CAN_BE_NULL(closure,type,var,n)
#define C_CLOSURE_PARAM_PTR(type,var,n)              __CCLOSURE_PARAM_PTR(closure,type,var,n)
#define C_CLOSURE_PARAM_OBJ_CAN_BE_NULL(type,var,n)  __CCLOSURE_PARAM_PTR_CAN_BE_NULL(closure,type,var,n)
#define C_CLOSURE_PARAM_OBJ(type,var,n)              __CCLOSURE_PARAM_PTR(closure,type,var,n)
#define C_CLOSURE_PARAM_INT(var,n)                   __CCLOSURE_PARAM_INT(closure,var,n)
#define C_CLOSURE_PARAM_INT64(var,n)                 __CCLOSURE_PARAM_INT64(closure,var,n)
#define C_CLOSURE_PARAM_FLOAT(var,n)                 __CCLOSURE_PARAM_FLOAT(closure,var,n)
#define C_CLOSURE_PARAM_DOUBLE(var,n)                __CCLOSURE_PARAM_DOUBLE(closure,var,n)
#define C_CLOSURE_PARAM_STRING(var,n)                __CCLOSURE_PARAM_STRING(closure,var,n)
#define C_CLOSURE_PARAM_WEAKPTR(type,var,n)          __CCLOSURE_PARAM_WEAKPTR(closure,type,var,n)

#define C_BEGIN_CLOSURE_FUNC(name)\
    static status_t name(struct closure *closure)\
    
#define C_END_CLOSURE_FUNC(name)

#define C_NEW_CLOSURE(name,func)\
struct closure *name;\
X_MALLOC(name,struct closure,1);\
closure_init(name);\
name->_is_on_heap = 1;\
closure_set_func(name,func)\

struct closure;
typedef int (*C_CLOSURE_FUNC)(struct closure*);

struct closure{
    int64_t params[CCLOSURE_MAX_PARAMS];
    uint8_t types[CCLOSURE_MAX_PARAMS];
    C_CLOSURE_FUNC func;
    uint32_t flags;
    void *_user_data;
    bool_t _is_on_heap;
};

status_t closure_init_basic(struct closure *self);
status_t closure_init(struct closure *self);
status_t closure_destroy(struct closure *self);
int closure_comp(struct closure *self,struct closure *_p);
status_t closure_copy(struct closure *self,struct closure *_p);
status_t closure_print(struct closure *self,struct log_buffer *buf);
status_t closure_set_func(struct closure *self,C_CLOSURE_FUNC func);
status_t closure_free_param(struct closure *self,int index);
status_t closure_set_param_pointer(struct closure *self,int index, const void *p);
status_t closure_set_param_int(struct closure *self,int index, int i);
status_t closure_set_param_int64(struct closure *self,int index, int64_t i);
status_t closure_set_param_float(struct closure *self,int index, float f);
status_t closure_set_param_double(struct closure *self,int index, double f);
status_t closure_set_param_string(struct closure *self,int index, const char *str,int len);
status_t closure_set_param_weak_pointer(struct closure *self,int index,void *ptr, int weak_ref_id);
status_t closure_run(struct closure *self);
int closure_get_param_int(struct closure *self,int index);
int64_t closure_get_param_int64(struct closure *self,int index);
float closure_get_param_float(struct closure *self,int index);
double closure_get_param_double(struct closure *self,int index);
const char* closure_get_param_string(struct closure *self,int index);
void* closure_get_param_pointer(struct closure *self,int index);
void* closure_get_param_weak_pointer(struct closure *self,int index,int *weak_ref_id);
void* closure_malloc(struct closure *self,int index, int size);
void* closure_malloc_with_data(struct closure *self,int index, const void *buf,int size);
status_t closure_run_event(struct closure *self,int event);
int closure_get_malloc_size(struct closure *self,int index);
int closure_get_param_type(struct closure *self,int index);


#ifdef __cplusplus
} //extern "C"
#endif

#endif
