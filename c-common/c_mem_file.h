#ifndef _C_MEM_FILE_H
#define _C_MEM_FILE_H

#include "cruntime.h"
#include "c_log_buffer.h"
#include "c_file_base.h"

#define C_MEM_FILE(name) \
struct mem_file name;\
struct file_base *name##_file = NULL;\
memfile_init(&name);\
name##_file=&name.base_file_base\

struct mem_file{
    struct file_base base_file_base;

    int_ptr_t size;
    int_ptr_t pages;
    int_ptr_t offset;
    int_ptr_t max_pages;
    char **base;
    int_ptr_t page_size;
    uint32_t mod_n;
    uint32_t shift_n;
};

status_t memfile_init_basic(struct mem_file *self);
static status_t memfile_init_shift(struct mem_file *self);
status_t memfile_init_ex(struct mem_file *self,int_ptr_t page_size,int_ptr_t max_pages);
status_t memfile_init(struct mem_file *self);
status_t memfile_destroy(struct mem_file *self);
status_t memfile_copy(struct mem_file *self,struct mem_file *_p);
status_t memfile_comp(struct mem_file *self,struct mem_file *_p);
status_t memfile_print(struct mem_file *self,struct log_buffer *_buf);
status_t memfile_add_page(struct mem_file *self);
status_t memfile_add_block(struct mem_file *self,fsize_t block_size);
int_ptr_t memfile_read(struct mem_file *self,void *buf,int_ptr_t rsize);
int_ptr_t memfile_write(struct mem_file *self,const void *buf,int_ptr_t wsize);
fsize_t memfile_get_max_size(struct mem_file *self);
fsize_t memfile_seek(struct mem_file *self,fsize_t off);
status_t memfile_set_size(struct mem_file *self,fsize_t s);
fsize_t memfile_get_offset(struct mem_file *self);
fsize_t memfile_get_size(struct mem_file *self);
status_t memfile_transfer(struct mem_file *self,struct mem_file *from);

#endif
