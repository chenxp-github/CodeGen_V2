#ifndef _C_PART_FILE_H
#define _C_PART_FILE_H

#include "cruntime.h"
#include "c_log_buffer.h"
#include "c_file_base.h"

struct part_file{
    struct file_base base_file_base;

    struct file_base *i_host_file;
    fsize_t offset;
    fsize_t size;
    fsize_t host_off;
    fsize_t max_size;
};

void* partfile_get_this_pointer(struct part_file *self);
status_t partfile_init_basic(struct part_file *self);
status_t partfile_init(struct part_file *self);
status_t partfile_destroy(struct part_file *self);
status_t partfile_copy(struct part_file *self,struct part_file *_p);
status_t partfile_comp(struct part_file *self,struct part_file *_p);
status_t partfile_print(struct part_file *self,struct log_buffer *_buf);
fsize_t partfile_get_offset(struct part_file *self);
fsize_t partfile_get_size(struct part_file *self);
int_ptr_t partfile_read(struct part_file *self,void *buf, int_ptr_t n);
int_ptr_t partfile_write(struct part_file *self,const void *buf, int_ptr_t n);
int partfile_set_size(struct part_file *self,fsize_t s);
fsize_t partfile_get_max_size(struct part_file *self);
int partfile_add_block(struct part_file *self,fsize_t bsize);
fsize_t partfile_seek(struct part_file *self,fsize_t off);
int partfile_set_host_file(struct part_file *self,struct file_base *host, fsize_t off, fsize_t size);
fsize_t partfile_get_host_offset(struct part_file *self);

#endif
