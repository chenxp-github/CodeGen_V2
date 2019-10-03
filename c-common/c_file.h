#ifndef _C_FILE_H
#define _C_FILE_H

#include "cruntime.h"
#include "c_log_buffer.h"
#include "c_file_base.h"

struct file{
    struct file_base base_file_base;

    FILE_HANDLE fp;
    char *file_buf;
    int_ptr_t buf_size;
    int_ptr_t init_buf_size;
    fsize_t offset;
    fsize_t size;
    fsize_t buf_start;
};

status_t file_init_basic(struct file *self);
status_t file_init(struct file *self);
status_t file_destroy(struct file *self);
status_t file_copy(struct file *self,struct file *_p);
status_t file_comp(struct file *self,struct file *_p);
status_t file_print(struct file *self,struct log_buffer *_buf);
status_t file_open_file(struct file *self,const char *fn , const char *mode);
status_t file_close_file(struct file *self);
fsize_t file_get_size(struct file *self);
fsize_t file_get_size_real(struct file *self);
fsize_t file_seek(struct file *self,fsize_t n_offset);
int_ptr_t file_read(struct file *self,void *buf,int_ptr_t n_byte_to_read);
int_ptr_t file_write(struct file *self,const void *buf,int_ptr_t n_byte_to_write);
status_t file_set_size(struct file *self,fsize_t n_size);
fsize_t file_get_offset(struct file *self);
status_t file_add_block(struct file *self,fsize_t bsize);
fsize_t file_get_max_size(struct file *self);
status_t file_has_buffer(struct file *self);
status_t file_set_buf_size(struct file *self,int_ptr_t size);
status_t file_buffer_block(struct file *self,fsize_t start_pos);
bool_t file_is_opened(struct file *self);

#endif
