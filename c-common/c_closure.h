#ifndef __S2_C_CLOSURE_H
#define __S2_C_CLOSURE_H

#include "cruntime.h"
#include "c_log_buffer.h"

#define CCLOSURE_MAX_PARAMS 16

#ifdef __cplusplus
extern "C" {
#endif

enum{
    CPARAM_TYPE_UNKNOWN = 0,
    CPARAM_TYPE_POINTER,
    CPARAM_TYPE_INT,
    CPARAM_TYPE_INT64,
    CPARAM_TYPE_DOUBLE,
    CPARAM_TYPE_FLOAT,
    CPARAM_TYPE_STRING,
    CPARAM_TYPE_MALLOC,
    CPARAM_TYPE_WEAKPTR,
};

#define __CCLOSURE_PARAM_PTR_CAN_BE_NULL(closure,type,var,n) \
ASSERT(closure_get_param_type(closure,n) == CPARAM_TYPE_POINTER \
||closure_get_param_type(closure,n) == CPARAM_TYPE_MALLOC);\
type var = (type)closure_get_param_pointer(closure,n)

#define __CCLOSURE_PARAM_PTR(closure,type,var,n) \
__CCLOSURE_CPARAM_PTR_CAN_BE_NULL(closure,type,var,n);\
ASSERT(var) \
    
#define __CCLOSURE_PARAM_INT(closure,var,n) \
ASSERT(closure_get_param_type(closure,n) == CPARAM_TYPE_INT);\
int var = closure_get_param_int(closure,n)\
    
#define __CCLOSURE_PARAM_INT64(closure,var,n) \
    ASSERT(closure_get_param_Type(n) == CPARAM_TYPE_INT64);\
    int64_t var = closure_get_param_Int64(n) \
    
#define __CCLOSURE_PARAM_FLOAT(closure,var,n) \
ASSERT(closure_get_param_type(closure,n) == CPARAM_TYPE_FLOAT);\
float var = closure_get_param_float(closure,n)

#define __CCLOSURE_PARAM_DOUBLE(closure,var,n) \
ASSERT(closure_get_param_Type(n) == CPARAM_TYPE_DOUBLE);\
double var = closure_get_param_Double(n) \
    
#define __CCLOSURE_PARAM_STRING(closure,var,n) \
ASSERT(closure_get_param_type(closure,n) == CPARAM_TYPE_STRING);\
const char* var = closure_get_param_string(closure,n) \
    
#define __CCLOSURE_PARAM_WEAKPTR(closure,type,var,n) \
int __weak_ref_id_##var; \
ASSERT(closure_get_param_type(closure,n) == CPARAM_TYPE_WEAKPTR); \
type var = (type)closure_get_param_weak_pointer(closure,n,&__weak_ref_id_##var);\
ASSERT(var && var->__weak_ref_id != 0 && var->__weak_ref_id == __weak_ref_id_##var)

#define CCLOSURE_PARAM_PTR_CAN_BE_NULL(type,var,n)  __CCLOSURE_PARAM_PTR_CAN_BE_NULL(closure,type,var,n)
#define CCLOSURE_PARAM_PTR(type,var,n)              __CCLOSURE_PARAM_PTR(closure,type,var,n)
#define CCLOSURE_PARAM_OBJ_CAN_BE_NULL(type,var,n)  __CCLOSURE_PARAM_PTR_CAN_BE_NULL(closure,type,var,n)
#define CCLOSURE_PARAM_OBJ(type,var,n)              __CCLOSURE_PARAM_PTR(closure,type,var,n)
#define CCLOSURE_PARAM_INT(var,n)                   __CCLOSURE_PARAM_INT(closure,var,n)
#define CCLOSURE_PARAM_INT64(var,n)                 __CCLOSURE_PARAM_INT64(closure,var,n)
#define CCLOSURE_PARAM_FLOAT(var,n)                 __CCLOSURE_PARAM_FLOAT(closure,var,n)
#define CCLOSURE_PARAM_DOUBLE(var,n)                __CCLOSURE_PARAM_DOUBLE(closure,var,n)
#define CCLOSURE_PARAM_STRING(var,n)                __CCLOSURE_PARAM_STRING(closure,var,n)
#define CCLOSURE_PARAM_WEAKPTR(type,var,n)          __CCLOSURE_PARAM_WEAKPTR(closure,type,var,n)

#define CCLOSURE_FLAG_FUNC_C(func,bit) CFLAG_FUNC_C(closure,flags,func,bit)
#define CCLOSURE_FLAG_FUNC_H(func,bit) CFLAG_FUNC_H(closure,flags,func,bit)

struct closure;
typedef int (*CCLOSURE_FUNC)(struct closure*);

struct closure{
    int64_t params[CCLOSURE_MAX_PARAMS];
    uint8_t types[CCLOSURE_MAX_PARAMS];
    CCLOSURE_FUNC func;
    uint32_t flags;
};

status_t closure_init_basic(struct closure *self);
status_t closure_init(struct closure *self);
status_t closure_destroy(struct closure *self);
int closure_comp(struct closure *self,struct closure *_p);
status_t closure_copy(struct closure *self,struct closure *_p);
status_t closure_print(struct closure *self,struct log_buffer *buf);
status_t closure_set_func(struct closure *self,CCLOSURE_FUNC func);
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
