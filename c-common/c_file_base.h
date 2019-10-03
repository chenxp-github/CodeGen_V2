#ifndef _C_FILE_BASE_H
#define _C_FILE_BASE_H

#include "cruntime.h"
#include "c_log_buffer.h"

#define FILE_BASE_INIT_VIRTUAL_FUNCTIONS(prefix) do{\
    self->base_file_base.read = prefix##_virtual_read;\
    self->base_file_base.destroy = prefix##_virtual_destroy;\
    self->base_file_base.write = prefix##_virtual_write;\
    self->base_file_base.seek = prefix##_virtual_seek;\
    self->base_file_base.get_offset = prefix##_virtual_get_offset;\
    self->base_file_base.get_size = prefix##_virtual_get_size;\
    self->base_file_base.set_size = prefix##_virtual_set_size;\
    self->base_file_base.add_block = prefix##_virtual_add_block;\
    self->base_file_base.get_max_size = prefix##_virtual_get_max_size;\
}while(0)\


#define FILE_BASE_VIRTUAL_FUNCTIONS_DEFINE(child_type, prefix)\
static int_ptr_t prefix##_virtual_read(struct file_base *base,void *buf,int_ptr_t n)\
{\
    CONTAINER_OF(child_type,self,base,base_file_base);\
    return prefix##_read(self,buf,n);\
}\
static status_t prefix##_virtual_destroy(struct file_base *base)\
{\
    CONTAINER_OF(child_type,self,base,base_file_base);\
    return prefix##_destroy(self);\
}\
static int_ptr_t prefix##_virtual_write(struct file_base *base,const void *buf,int_ptr_t n)\
{\
    CONTAINER_OF(child_type,self,base,base_file_base);\
    return prefix##_write(self,buf,n);\
}\
static fsize_t prefix##_virtual_seek(struct file_base *base,fsize_t off)\
{\
    CONTAINER_OF(child_type,self,base,base_file_base);\
    return prefix##_seek(self,off);\
}\
static fsize_t prefix##_virtual_get_offset(struct file_base *base)\
{\
    CONTAINER_OF(child_type,self,base,base_file_base);\
    return prefix##_get_offset(self);\
}\
static fsize_t prefix##_virtual_get_size(struct file_base *base)\
{\
    CONTAINER_OF(child_type,self,base,base_file_base);\
    return prefix##_get_size(self);\
}\
static status_t prefix##_virtual_set_size(struct file_base *base,fsize_t size)\
{\
    CONTAINER_OF(child_type,self,base,base_file_base);\
    return prefix##_set_size(self,size);\
}\
static status_t prefix##_virtual_add_block(struct file_base *base,fsize_t bsize)\
{\
    CONTAINER_OF(child_type,self,base,base_file_base);\
    return prefix##_add_block(self,bsize);\
}\
static fsize_t prefix##_virtual_get_max_size(struct file_base *base)\
{\
    CONTAINER_OF(child_type,self,base,base_file_base);\
    return prefix##_get_max_size(self);\
}\
/************************************/
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
status_t filebase_base_destroy(struct file_base *self);
status_t filebase_destroy(struct file_base *self);
status_t filebase_copy(struct file_base *self,struct file_base *_p);
status_t filebase_comp(struct file_base *self,struct file_base *_p);
status_t filebase_print(struct file_base *self,struct log_buffer *_buf);
char* filebase_get_split_chars(struct file_base *self);
status_t filebase_set_split_chars(struct file_base *self,char *_split_chars);
status_t filebase_set_default_sp_chars(struct file_base *self);
status_t filebase_is_split_char(struct file_base *self,char ch);
int_ptr_t filebase_read(struct file_base *self,void *buf,int_ptr_t n);
int_ptr_t filebase_write(struct file_base *self,const void *buf,int_ptr_t n);
fsize_t filebase_seek(struct file_base *self,fsize_t off);
fsize_t filebase_get_offset(struct file_base *self);
fsize_t filebase_get_size(struct file_base *self);
status_t filebase_set_size(struct file_base *self,fsize_t size);
status_t filebase_add_block(struct file_base *self,fsize_t bsize);
fsize_t filebase_get_max_size(struct file_base *self);
char filebase_getc(struct file_base *self);
fsize_t filebase_putc(struct file_base *self,char ch);
char filebase_un_getc(struct file_base *self);
bool_t filebase_is_end(struct file_base *self);
fsize_t filebase_search_bin(struct file_base *self,struct file_base *file_bin);
fsize_t filebase_write_file_with_buffer(struct file_base *self,struct file_base *file, fsize_t start, fsize_t ws, void *buf, int_ptr_t buf_size);
fsize_t filebase_write_file_from(struct file_base *self,struct file_base *file, fsize_t start, fsize_t ws);
fsize_t filebase_write_file(struct file_base *self,struct file_base *file);
fsize_t filebase_write_to_file_with_buffer(struct file_base *self,struct file_base *file, fsize_t start, fsize_t wsize, void *buf, int_ptr_t buf_size);
fsize_t filebase_write_to_file_from(struct file_base *self,struct file_base *file,fsize_t start,fsize_t wsize);
fsize_t filebase_write_to_file(struct file_base *self,struct file_base *file);
fsize_t filebase_save_file_fp(struct file_base *self,FILE_HANDLE fp,fsize_t start,fsize_t wsize);
fsize_t filebase_save_file_from(struct file_base *self,const char *fn,fsize_t start,fsize_t wsize);
fsize_t filebase_save_file(struct file_base *self,const char *fn);
status_t filebase_load_file_from_fp(struct file_base *self,FILE_HANDLE fp);
status_t filebase_load_file(struct file_base *self,const char *fn);
int filebase_get_utf8(struct file_base *self,char *utf8);
int filebase_get_gb(struct file_base *self,char *letter);
char filebase_char_at(struct file_base *self,fsize_t index);
status_t filebase_set_char(struct file_base *self,fsize_t index,char ch);
fsize_t filebase_read_word_with_empty_char(struct file_base *self,struct file_base *file);
fsize_t filebase_puts(struct file_base *self,const char *str);
status_t filebase_read_line(struct file_base *self,struct file_base *file);
status_t filebase_read_line_with_eol(struct file_base *self,struct file_base *file,struct file_base *eol);
status_t filebase_is_empty_char(char ch);
char filebase_get_last_char(struct file_base *self,bool_t empty_included);
fsize_t filebase_search_str(struct file_base *self,const char *str, status_t case_sensive,status_t word_only);
status_t filebase_replace_str(struct file_base *self,const char *src_str, const char *des_str, status_t case_sensive, status_t word_only,struct file_base *new_file);
status_t filebase_read_quote_str(struct file_base *self,char escape_char,char end_char,struct file_base *file);
status_t filebase_read_cstr(struct file_base *self,struct file_base *file);
status_t filebase_end_with_file(struct file_base *self,struct file_base *file_str, status_t case_sensive, status_t skip_empty);
status_t filebase_end_with(struct file_base *self,const char *str, status_t case_sensive, status_t skip_empty);
status_t filebase_skip_empty_chars(struct file_base *self);
status_t filebase_start_with_file(struct file_base *self,struct file_base *file_str, status_t case_sensive, status_t skip_empty);
status_t filebase_start_with(struct file_base *self,const char *str, status_t case_sensive, status_t skip_empty);
status_t filebase_trim(struct file_base *self);
fsize_t filebase_read_word(struct file_base *self,struct file_base *file);
status_t filebase_dump(struct file_base *self);
fsize_t filebase_printf(struct file_base *self,const char *sz_format, ...);
fsize_t filebase_log(struct file_base *self,const char *sz_format, ...);

#endif

