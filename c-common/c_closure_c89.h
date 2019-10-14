#ifndef __C_CLOSURE_C89_H
#define __C_CLOSURE_C89_H

#define _C89_CLOSURE_PARAM_PTR_CAN_BE_NULL(closure,type,var,n) \
ASSERT(closure_get_param_type(closure,n) == C_PARAM_TYPE_POINTER \
||closure_get_param_type(closure,n) == C_PARAM_TYPE_MALLOC);\
var = (type)closure_get_param_pointer(closure,n)

#define _C89_CLOSURE_PARAM_PTR(closure,type,var,n) \
_C89_CLOSURE_PARAM_PTR_CAN_BE_NULL(closure,type,var,n);\
ASSERT(var) \
    
#define _C89_CLOSURE_PARAM_INT(closure,var,n) \
ASSERT(closure_get_param_type(closure,n) == C_PARAM_TYPE_INT);\
var = closure_get_param_int(closure,n)\
    
#define _C89_CLOSURE_PARAM_INT64(closure,var,n) \
ASSERT(closure_get_param_type(closure,n) == C_PARAM_TYPE_INT64);\
var = closure_get_param_int64(closure,n) \

#define _C89_CLOSURE_PARAM_FLOAT(closure,var,n) \
ASSERT(closure_get_param_type(closure,n) == C_PARAM_TYPE_FLOAT);\
var = closure_get_param_float(closure,n)

#define _C89_CLOSURE_PARAM_DOUBLE(closure,var,n) \
ASSERT(closure_get_param_type(closure,n) == C_PARAM_TYPE_DOUBLE);\
var = closure_get_param_double(closure,n) \
    
#define _C89_CLOSURE_PARAM_STRING(closure,var,n) \
ASSERT(closure_get_param_type(closure,n) == C_PARAM_TYPE_STRING);\
var = closure_get_param_string(closure,n) \
    
#define C89_CLOSURE_PARAM_PTR_CAN_BE_NULL(type,var,n)  _C89_CLOSURE_PARAM_PTR_CAN_BE_NULL(closure,type,var,n)
#define C89_CLOSURE_PARAM_PTR(type,var,n)              _C89_CLOSURE_PARAM_PTR(closure,type,var,n)
#define C89_CLOSURE_PARAM_OBJ_CAN_BE_NULL(type,var,n)  _C89_CLOSURE_PARAM_PTR_CAN_BE_NULL(closure,type,var,n)
#define C89_CLOSURE_PARAM_OBJ(type,var,n)              _C89_CLOSURE_PARAM_PTR(closure,type,var,n)
#define C89_CLOSURE_PARAM_INT(var,n)                   _C89_CLOSURE_PARAM_INT(closure,var,n)
#define C89_CLOSURE_PARAM_INT64(var,n)                 _C89_CLOSURE_PARAM_INT64(closure,var,n)
#define C89_CLOSURE_PARAM_FLOAT(var,n)                 _C89_CLOSURE_PARAM_FLOAT(closure,var,n)
#define C89_CLOSURE_PARAM_DOUBLE(var,n)                _C89_CLOSURE_PARAM_DOUBLE(closure,var,n)
#define C89_CLOSURE_PARAM_STRING(var,n)                _C89_CLOSURE_PARAM_STRING(closure,var,n)

#endif
