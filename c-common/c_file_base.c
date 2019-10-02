#include "c_file_base.h"
#include "syslog.h"
#include "mem_tool.h"

status_t filebase_init_basic(struct file_base *self)
{
    self->user_data = NULL;
    self->split_chars = NULL;
    return OK;
}

status_t filebase_init(struct file_base *self)
{
    filebase_init_basic(self);
    return OK;
}

status_t filebase_destroy(struct file_base *self)
{
    filebase_init_basic(self);
    return OK;
}

status_t filebase_copy(struct file_base *self,struct file_base *_p)
{
    ASSERT(_p);
    if(self == _p)return OK;

    self->user_data = _p->user_data;
    self->split_chars = _p->split_chars;
    return OK;
}

status_t filebase_comp(struct file_base *self,struct file_base *_p)
{
    ASSERT(_p);
    if(self == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t filebase_print(struct file_base *self,struct log_buffer *_buf)
{
    ASSERT(_buf);
    log_buffer_log(_buf,"user_data = 0x%p",self->user_data);
    log_buffer_log(_buf,"split_chars = 0x%p",self->split_chars);
    return OK;
}


char* filebase_get_split_chars(struct file_base *self)
{
    return self->split_chars;
}


status_t filebase_set_split_chars(struct file_base *self,char *_split_chars)
{
    self->split_chars = _split_chars;
    return OK;
}

