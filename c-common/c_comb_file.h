#ifndef _C_COMB_FILE_H
#define _C_COMB_FILE_H

#include "cruntime.h"
#include "c_log_buffer.h"
#include "c_file_base.h"

#define C_COMB_FILE(name,m) \
struct comb_file name;\
struct file_base *name##_file = NULL;\
combfile_init(&name,m);\
name##_file=&name.base_file_base\

struct comb_file{
    struct file_base base_file_base;

    fsize_t offset;
    struct file_base **all_files;
    int file_num;
    int max_file_num;
};

status_t combfile_init_basic(struct comb_file *self);
status_t combfile_init(struct comb_file *self,int max);
status_t combfile_destroy(struct comb_file *self);
status_t combfile_copy(struct comb_file *self,struct comb_file *_p);
status_t combfile_comp(struct comb_file *self,struct comb_file *_p);
status_t combfile_print(struct comb_file *self,struct log_buffer *_buf);
fsize_t combfile_get_offset(struct comb_file *self);
fsize_t combfile_get_size(struct comb_file *self);
status_t combfile_set_size(struct comb_file *self,fsize_t ssize);
fsize_t combfile_seek(struct comb_file *self,fsize_t off);
status_t combfile_add_block(struct comb_file *self,fsize_t bsize);
fsize_t combfile_get_max_size(struct comb_file *self);
int combfile_in_which_file(struct comb_file *self,fsize_t off,fsize_t *sub_off);
status_t combfile_add_file(struct comb_file *self,struct file_base *file);
status_t combfile_clear(struct comb_file *self);
int_ptr_t combfile_read(struct comb_file *self,void *buf,int_ptr_t n);
int_ptr_t combfile_write(struct comb_file *self,const void *buf,int_ptr_t n);


#endif
