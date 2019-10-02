#include "c_file_base.h"
#include "syslog.h"
#include "mem_tool.h"

status_t filebase_init_basic(struct file_base *self)
{
    self->user_data = NULL;
    self->split_chars = NULL;    
    self->destroy = NULL;
    self->read = NULL;
    self->write = NULL;
    self->seek = NULL;
    self->get_offset = NULL;
    self->get_size = NULL;
    self->set_size = NULL;
    self->add_block = NULL;
    self->get_max_size = NULL;
    return OK;
}

status_t filebase_init(struct file_base *self)
{
    filebase_init_basic(self);
    return OK;
}

status_t filebase_base_destroy(struct file_base *self)
{
    filebase_init_basic(self);
    return OK;
}

status_t filebase_destroy(struct file_base *self)
{
    if(self->destroy)
    {
        self->destroy(self);
    }
    filebase_base_destroy(self);
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

int_ptr_t filebase_read(struct file_base *self,void *buf,int_ptr_t n)
{
    ASSERT(self->read);
    return self->read(self,buf,n);
}

int_ptr_t filebase_write(struct file_base *self,const void *buf,int_ptr_t n)
{
    ASSERT(self->write);
    return self->write(self,buf,n);
}

fsize_t filebase_seek(struct file_base *self,fsize_t off)
{
    ASSERT(self->seek);
    return self->seek(self,off);
}

fsize_t filebase_get_offset(struct file_base *self)
{
    ASSERT(self->get_offset);
    return self->get_offset(self);
}

fsize_t filebase_get_size(struct file_base *self)
{
    ASSERT(self->get_size);
    return self->get_size(self);
}
status_t filebase_set_size(struct file_base *self,fsize_t size)
{
    ASSERT(self->set_size);
    return self->set_size(self,size);
}

status_t filebase_add_block(struct file_base *self,fsize_t bsize)
{
    ASSERT(self->add_block);
    return self->add_block(self,bsize);
}

fsize_t filebase_get_max_size(struct file_base *self)
{
    ASSERT(self->get_max_size);
    return self->get_max_size(self);
}

