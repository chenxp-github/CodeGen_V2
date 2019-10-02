#ifndef _C_FILE_BASE_H
#define _C_FILE_BASE_H

#include "cruntime.h"
#include "c_log_buffer.h"

struct file_base{
    void *user_data;
    char *split_chars;

    status_t (*destroy)(struct filebase *self);
    int_ptr_t (*read)(struct filebase *self,void *buf,int_ptr_t n);
};

status_t filebase_init_basic(struct file_base *self);
status_t filebase_init(struct file_base *self);
status_t filebase_destroy(struct file_base *self);
status_t filebase_base_destroy(struct file_base *self);
status_t filebase_copy(struct file_base *self,struct file_base *_p);
status_t filebase_comp(struct file_base *self,struct file_base *_p);
status_t filebase_print(struct file_base *self,struct log_buffer *_buf);

char* filebase_get_split_chars(struct file_base *self);

status_t filebase_set_split_chars(struct file_base *self,char *_split_chars);

#endif
