#ifndef _C_MEM_H
#define _C_MEM_H

#include "cruntime.h"
#include "c_log_buffer.h"
#include "c_file_base.h"

struct mem{
    struct file_base base_file_base;
	char *buffer;
    int_ptr_t offset;
    int_ptr_t size;
    int_ptr_t max_size;
    bool_t is_const;
    bool_t is_self_alloc;
};

status_t mem_init_basic(struct mem *self);
status_t mem_init(struct mem *self);
status_t mem_destroy(struct mem *self);
status_t mem_copy(struct mem *self,struct mem *_p);
status_t mem_comp(struct mem *self,struct mem *_p);
status_t mem_print(struct mem *self,struct log_buffer *_buf);


status_t mem_free(struct mem *self);
status_t mem_malloc(struct mem *self,int_ptr_t asize);
status_t mem_free(struct mem *self);
status_t mem_realloc(struct mem *self,int_ptr_t new_size);
int_ptr_t mem_read(struct mem *self,void *buf,int_ptr_t n);
int_ptr_t mem_write(struct mem *self,const void *buf,int_ptr_t n);
status_t mem_zero(struct mem *self);
status_t mem_set_raw_buffer(struct mem *self,void *p,int_ptr_t s,bool_t is_const);
fsize_t mem_seek(struct mem*self,fsize_t off);
int_ptr_t mem_get_offset(struct mem *self);
int_ptr_t mem_get_size(struct mem *self);
status_t mem_set_size(struct mem *self,int_ptr_t _size);
status_t mem_add_block(struct mem *self,fsize_t bsize);
fsize_t mem_get_max_size(struct mem *self);
status_t mem_set_str(struct mem *self,const char *p);
const char *mem_cstr(struct mem *self);
char * mem_get_raw_buffer(struct mem *self);

#endif
