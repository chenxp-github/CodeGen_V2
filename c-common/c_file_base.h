#ifndef _C_FILE_BASE_H
#define _C_FILE_BASE_H

#include "cruntime.h"
#include "c_log_buffer.h"

struct file_base{
    void *user_data;
    char *split_chars;

    status_t (*destroy)(struct file_base *self);
    int_ptr_t (*read)(struct file_base *self,void *buf,int_ptr_t n);
    int_ptr_t (*write)(struct file_base *self,const void *buf,int_ptr_t n);
    fsize_t (*seek)(struct file_base *self,fsize_t off);
    fsize_t (*get_offset)(struct file_base *self);
    fsize_t (*get_size)(struct file_base *self);
    status_t (*set_size)(struct file_base *self,fsize_t size);
    status_t (*add_block)(struct file_base *self,fsize_t bsize);
    fsize_t (*get_max_size)(struct file_base *self);
};

status_t filebase_init_basic(struct file_base *self);
status_t filebase_init(struct file_base *self);
status_t filebase_destroy(struct file_base *self);
status_t filebase_base_destroy(struct file_base *self);
status_t filebase_copy(struct file_base *self,struct file_base *_p);
status_t filebase_comp(struct file_base *self,struct file_base *_p);
status_t filebase_print(struct file_base *self,struct log_buffer *_buf);

int_ptr_t filebase_read(struct file_base *self,void *buf,int_ptr_t n);
int_ptr_t filebase_write(struct file_base *self,const void *buf,int_ptr_t n);
fsize_t filebase_seek(struct file_base *self,fsize_t off);
fsize_t filebase_get_offset(struct file_base *self);
fsize_t filebase_get_size(struct file_base *self);
status_t filebase_set_size(struct file_base *self,fsize_t size);
status_t filebase_add_block(struct file_base *self,fsize_t bsize);
fsize_t filebase_get_max_size(struct file_base *self);

char* filebase_get_split_chars(struct file_base *self);
status_t filebase_set_split_chars(struct file_base *self,char *_split_chars);

#endif

