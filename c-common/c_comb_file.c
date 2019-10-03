#include "c_comb_file.h"
#include "syslog.h"
#include "mem_tool.h"

#define EXTRACT_SELF_POINTER(base,self)\
CONTAINER_OF(struct comb_file, self, base, base_file_base)

static int_ptr_t combfile_virtual_read(struct file_base *base,void *buf,int_ptr_t n)
{
    EXTRACT_SELF_POINTER(base,self);
    return combfile_read(self,buf,n);
}

static status_t combfile_virtual_destroy(struct file_base *base)
{
    EXTRACT_SELF_POINTER(base,self);
    return combfile_destroy(self);
}

static int_ptr_t combfile_virtual_write(struct file_base *base,const void *buf,int_ptr_t n)
{
    EXTRACT_SELF_POINTER(base,self);
    return combfile_write(self,buf,n);
}

static fsize_t combfile_virtual_seek(struct file_base *base,fsize_t off)
{
    EXTRACT_SELF_POINTER(base,self);
    return combfile_seek(self,off);
}

static fsize_t combfile_virtual_get_offset(struct file_base *base)
{
    EXTRACT_SELF_POINTER(base,self);
    return combfile_get_offset(self);
}

static fsize_t combfile_virtual_get_size(struct file_base *base)
{
    EXTRACT_SELF_POINTER(base,self);
    return combfile_get_size(self);
}

static status_t combfile_virtual_set_size(struct file_base *base,fsize_t size)
{
    EXTRACT_SELF_POINTER(base,self);
    return combfile_set_size(self,size);
}

static status_t combfile_virtual_add_block(struct file_base *base,fsize_t bsize)
{
    EXTRACT_SELF_POINTER(base,self);
    return combfile_add_block(self,bsize);
}

static fsize_t combfile_virtual_get_max_size(struct file_base *base)
{
    EXTRACT_SELF_POINTER(base,self);
    return combfile_get_max_size(self);
}

/**************************************************************/
status_t combfile_init_basic(struct comb_file *self)
{
    filebase_init_basic(&self->base_file_base);
    self->offset = 0;
    self->all_files = NULL;
    self->file_num = 0;
    self->max_file_num = 0;
    return OK;
}

status_t combfile_init(struct comb_file *self,int max)
{
    combfile_init_basic(self);
    filebase_init(&self->base_file_base);
  
    self->base_file_base.read = combfile_virtual_read;
    self->base_file_base.destroy = combfile_virtual_destroy;
    self->base_file_base.write = combfile_virtual_write;
    self->base_file_base.seek = combfile_virtual_seek;
    self->base_file_base.get_offset = combfile_virtual_get_offset;
    self->base_file_base.get_size = combfile_virtual_get_size;
    self->base_file_base.set_size = combfile_virtual_set_size;
    self->base_file_base.add_block = combfile_virtual_add_block;
    self->base_file_base.get_max_size = combfile_virtual_get_max_size;
  
    X_MALLOC(self->all_files,struct file_base*,max);
    self->max_file_num = max;
    return OK;
}

status_t combfile_destroy(struct comb_file *self)
{
    X_FREE(self->all_files);
    filebase_base_destroy(&self->base_file_base);
    combfile_init_basic(self);
    return OK;
}

status_t combfile_copy(struct comb_file *self,struct comb_file *_p)
{
    ASSERT(_p);
    if(self == _p)return OK;
    filebase_copy(&self->base_file_base,&_p->base_file_base);
    ASSERT(0);
    return OK;
}

status_t combfile_comp(struct comb_file *self,struct comb_file *_p)
{
    ASSERT(_p);
    if(self == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t combfile_print(struct comb_file *self,struct log_buffer *_buf)
{
    ASSERT(_buf);
    filebase_print(&self->base_file_base,_buf);
    return OK;
}


fsize_t combfile_get_offset(struct comb_file *self)
{
    return self->offset;
}

fsize_t combfile_get_size(struct comb_file *self)
{
    int i;
    fsize_t size = 0;
    for(i = 0; i < self->file_num; i++)
    {
        size += filebase_get_size(self->all_files[i]);
    }
    return size;
}

status_t combfile_set_size(struct comb_file *self,fsize_t ssize)
{
    self->offset = 0;
    return OK;
}

fsize_t combfile_seek(struct comb_file *self,fsize_t off)
{
    if(off > combfile_get_size(self))off = combfile_get_size(self);
    if(off < 0 )off = 0;
    self->offset = (int_ptr_t)off;
    return self->offset;
}

status_t combfile_add_block(struct comb_file *self,fsize_t bsize)
{
    return OK;
}

fsize_t combfile_get_max_size(struct comb_file *self)
{
    return combfile_get_size(self);
}

int combfile_in_which_file(struct comb_file *self,fsize_t off,fsize_t *sub_off)
{
    int i;
    fsize_t begin,size = 0;

    for(i = 0; i < self->file_num; i++)
    {
        begin = size;
        size += filebase_get_size(self->all_files[i]);
        if(size > off)
        {
            *sub_off = off - begin;
            return i;
        }
    }
    return -1;
}

status_t combfile_add_file(struct comb_file *self,struct file_base *file)
{
    ASSERT(file);
    ASSERT(self->file_num < self->max_file_num);
    self->all_files[self->file_num] = file;
    self->file_num++;
    return OK;
}

status_t combfile_clear(struct comb_file *self)
{
    self->file_num = 0;
    combfile_set_size(self,0);
    return OK;
}

int_ptr_t combfile_read(struct comb_file *self,void *buf,int_ptr_t n)
{
    int_ptr_t read_size = 0;
    
    while(read_size < n)
    {
        fsize_t off = 0,old_off;
        int_ptr_t rs;
        int i = combfile_in_which_file(self,self->offset,&off);
        if(i < 0) break;
        
        old_off = filebase_get_offset(self->all_files[i]);              
        filebase_seek(self->all_files[i],off);

        rs = filebase_read(self->all_files[i],
            ((uint8_t*)buf)+read_size,n-read_size
        );
        
        filebase_seek(self->all_files[i],old_off);
        if(rs <= 0) break;
        self->offset += rs;
        read_size += rs;
    }

    return read_size;
}

int_ptr_t combfile_write(struct comb_file *self,const void *buf,int_ptr_t n)
{
    ASSERT(0);
    return 0;
}