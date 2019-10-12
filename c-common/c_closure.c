#include "c_closure.h"
#include "syslog.h"
#include "mem_tool.h"

#define CHECK_INDEX_RANGE(index) ASSERT(index >= 0 && index < CCLOSURE_MAX_PARAMS)

#define SET_PARAM(type,var) \
CHECK_INDEX_RANGE(index); \
closure_free_param(self,index);\
self->types[index] = type;\
self->params[index] = (int64_t)(int_ptr_t)(var)

#define CHECK_TYPE(index,type) \
CHECK_INDEX_RANGE(index); \
ASSERT(self->types[index] == type)

#define TO_POINTER(index) ((void*)(int_ptr_t)self->params[index])
#define C_CLOSURE_FLAG_FUNC_C(func,bit) C_FLAG_FUNC_C(closure,flags,func,bit)

C_CLOSURE_FLAG_FUNC_C(is_called,0x00000001);
C_CLOSURE_FLAG_FUNC_C(is_timeout,0x00000002);
C_CLOSURE_FLAG_FUNC_C(in_on_heap,0x00000004);

status_t closure_init_basic(struct closure *self)
{
    self->func = NULL;
    self->flags = 0;
    memset(self->params,0,sizeof(self->params));
    memset(self->types,0,sizeof(self->types));
    self->_user_data = NULL;
    self->_is_on_heap = 0;
    return OK;
}
status_t closure_init(struct closure *self)
{
    closure_init_basic(self);
    return OK;
}
status_t closure_destroy(struct closure *self)
{
    int i;
    for(i = 0; i < CCLOSURE_MAX_PARAMS; i++)
    {
        closure_free_param(self,i);
    }
    closure_init_basic(self);
    return OK;
}


status_t closure_copy_params(struct closure *self,struct closure *closure)
{
    int i,type;

    for(i = 0; i < CCLOSURE_MAX_PARAMS; i++)
    {
        type = closure_get_param_type(closure,i);
        if(type == C_PARAM_TYPE_MALLOC)
        {
            int size = closure_get_malloc_size(closure,i);
            closure_malloc(self,i,size);
            memcpy(
                closure_get_param_pointer(self,i),
                closure_get_param_pointer(closure,i),
                size
            );
        }
        else if(type == C_PARAM_TYPE_STRING)
        {
            const char *str = closure_get_param_string(closure,i);
            closure_set_param_string(self,i,str,-1);
        }
        else if(type == C_PARAM_TYPE_WEAKPTR)
        {
            int weak_ref_id;
            void *ptr = closure_get_param_weak_pointer(closure,i,&weak_ref_id);
            closure_set_param_weak_pointer(self,i,ptr,weak_ref_id);
        }
        else if(type != C_PARAM_TYPE_UNKNOWN)
        {
            self->types[i] = closure->types[i];
            self->params[i] = closure->params[i];
        }
    }
    
    return OK;
}

int closure_comp(struct closure *self,struct closure *_p)
{
    ASSERT(0);
    return 0;
}

status_t closure_copy(struct closure *self,struct closure *_p)
{
    self->func = _p->func;
    closure_copy_params(self,_p);
    return OK;
}


status_t closure_print(struct closure *self,struct log_buffer *buf)
{
    int i,type;

    for(i = 0; i < CCLOSURE_MAX_PARAMS; i++)
    {
        type = closure_get_param_type(self,i);
        
        if(type != C_PARAM_TYPE_UNKNOWN)
        {
            log_buffer_log(buf,"[%d]=",i);
        }
        
        if(type == C_PARAM_TYPE_MALLOC)
        {
            int size = closure_get_malloc_size(self,i);
            log_buffer_log(buf,"<malloc %d bytes>",size);
        }
        else if(type == C_PARAM_TYPE_STRING)
        {
            const char *str = closure_get_param_string(self,i);
            log_buffer_log(buf,"\"%s\"",str);
        }
        else if(type == C_PARAM_TYPE_WEAKPTR)
        {
            int weak_id;
            void *p = closure_get_param_weak_pointer(self,i,&weak_id);
            log_buffer_log(buf,"<%p>",p);
        }
        else if(type == C_PARAM_TYPE_POINTER)
        {
            void *p = closure_get_param_pointer(self,i);
            log_buffer_log(buf,"<%p>",p);
        }
        else if(type == C_PARAM_TYPE_INT)
        {
            int v = closure_get_param_int(self,i);
            log_buffer_log(buf,"%d\r\n",v);
        }
        else if(type == C_PARAM_TYPE_INT64)
        {
            int64_t v = closure_get_param_int64(self,i);
            log_buffer_log(buf,"%ld\r\n",v);
        }
        else if(type == C_PARAM_TYPE_FLOAT)
        {
            float f = closure_get_param_float(self,i);
            log_buffer_log(buf,"%f\r\n",f);
        }
        else if(type == C_PARAM_TYPE_DOUBLE)
        {
            double v = closure_get_param_double(self,i);
            log_buffer_log(buf,"%lf\r\n",v);
        }
    }   
    return OK;
}

status_t closure_set_func(struct closure *self,C_CLOSURE_FUNC func)
{
    self->func = func;
    return OK;
}

status_t closure_free_param(struct closure *self,int index)
{
    CHECK_INDEX_RANGE(index); 
    if(    self->types[index] == C_PARAM_TYPE_STRING 
        || self->types[index] == C_PARAM_TYPE_MALLOC 
        || self->types[index] == C_PARAM_TYPE_WEAKPTR)
    {
        void *p = TO_POINTER(index);
        X_FREE(p);
        self->params[index] = 0;
    }
    self->types[index] = C_PARAM_TYPE_UNKNOWN;
    self->params[index] = 0;
    return OK;
}

status_t closure_set_param_pointer(struct closure *self,int index, const void *p)
{
    SET_PARAM(C_PARAM_TYPE_POINTER,p);
    return OK;
}

status_t closure_set_param_int(struct closure *self,int index, int i)
{
    SET_PARAM(C_PARAM_TYPE_INT,i);
    return OK;
}
status_t closure_set_param_int64(struct closure *self,int index, int64_t i)
{
    SET_PARAM(C_PARAM_TYPE_INT64,i);
    return OK;
}

status_t closure_set_param_float(struct closure *self,int index, float f)
{
    int_ptr_t *t = (int_ptr_t*)&f;
    ASSERT(sizeof(f) == sizeof(int_ptr_t));
    SET_PARAM(C_PARAM_TYPE_FLOAT,*t);
    return OK;
}
status_t closure_set_param_double(struct closure *self,int index, double f)
{
    int64_t *t = (int64_t*)&f;
    ASSERT(sizeof(f) == sizeof(int64_t));
    SET_PARAM(C_PARAM_TYPE_DOUBLE,*t);
    return OK;
}

status_t closure_set_param_string(struct closure *self,int index, const char *str,int len)
{
    char *tmp;
    ASSERT(str);
    CHECK_INDEX_RANGE(index);
    if(len < 0)len = crt_strlen(str);
    X_MALLOC(tmp,char,len+1);
    crt_strcpy(tmp,str);    
    SET_PARAM(C_PARAM_TYPE_STRING,tmp);
    return OK;
}

status_t closure_set_param_weak_pointer(struct closure *self,int index,void *ptr, int weak_ref_id)
{
    int_ptr_t *tmp;
    CHECK_INDEX_RANGE(index); 
    X_MALLOC(tmp,int_ptr_t,2);
    tmp[0] = (int_ptr_t)ptr;
    tmp[1] = (int_ptr_t)weak_ref_id;
    SET_PARAM(C_PARAM_TYPE_WEAKPTR,tmp);
    return OK;
}

status_t closure_run(struct closure *self)
{
    status_t ret = ERROR;
    if(self->func==NULL)
        return ERROR;
    ret = self->func(self);
    closure_set_is_called(self,TRUE);
    return ret;
}

int closure_get_param_int(struct closure *self,int index)
{
    CHECK_TYPE(index,C_PARAM_TYPE_INT);
    return (int)((int_ptr_t)self->params[index]);
}

int64_t closure_get_param_int64(struct closure *self,int index)
{
    CHECK_TYPE(index,C_PARAM_TYPE_INT64);
    return self->params[index];
}

float closure_get_param_float(struct closure *self,int index)
{
    float *t;
    CHECK_TYPE(index,C_PARAM_TYPE_FLOAT);
    t = (float*)&self->params[index];
    return *t;
}

double closure_get_param_double(struct closure *self,int index)
{
    double *t;
    CHECK_TYPE(index,C_PARAM_TYPE_DOUBLE);
    t = (double*)&self->params[index];
    return *t;
}

const char* closure_get_param_string(struct closure *self,int index)
{
    CHECK_INDEX_RANGE(index); 
    ASSERT(self->types[index] == C_PARAM_TYPE_STRING);
    return (const char*)(int_ptr_t)self->params[index];
}

void* closure_get_param_pointer(struct closure *self,int index)
{
    CHECK_INDEX_RANGE(index);
    
    if(self->types[index] == C_PARAM_TYPE_POINTER)
        return TO_POINTER(index);
    
    if(self->types[index] == C_PARAM_TYPE_MALLOC)
    {
        int *p = (int*)TO_POINTER(index);
        return (void*)(p+1); //skip size
    }
    
    ASSERT(0); //type error
    return NULL;
}

void* closure_get_param_weak_pointer(struct closure *self,int index,int *weak_ref_id)
{
    int_ptr_t *p;
    CHECK_TYPE(index,C_PARAM_TYPE_WEAKPTR);
    p = (int_ptr_t*)TO_POINTER(index);
    *weak_ref_id = (int)p[1];
    return (void*)p[0];
}

void* closure_malloc(struct closure *self,int index, int size)
{
    void *p;
    int *ip;

    if(size <= 0)
        return NULL;
    
    X_MALLOC(p,char,size+sizeof(int));
    SET_PARAM(C_PARAM_TYPE_MALLOC,p);
    
    ip = (int*)p;
    *ip = size;
    
    return (void*)(ip+1);
}

void* closure_malloc_with_data(struct closure *self,int index, const void *buf,int size)
{
    void *p;
    if(size <= 0)return NULL;
    p = closure_malloc(self,index,size);
    ASSERT(p);
    memcpy(p,buf,size);
    return p;
}

status_t closure_run_event(struct closure *self,int event)
{
    closure_set_param_int(self,0,event);
    return closure_run(self);
}

int closure_get_malloc_size(struct closure *self,int index)
{
    int *p;
    ASSERT(self->types[index] == C_PARAM_TYPE_MALLOC);
    p = (int*)TO_POINTER(index);
    ASSERT(p);
    return p[0];
}

int closure_get_param_type(struct closure *self,int index)
{
    CHECK_INDEX_RANGE(index);
    return self->types[index];
}
