#include "c_mem.h"
#include "syslog.h"
#include "mem_tool.h"

#define EXTRACT_SELF_POINTER(base,self)\
    CONTAINER_OF(struct mem, self, base, base_file_base)

static int_ptr_t mem_virtual_read(struct file_base *base,void *buf,int_ptr_t n)
{
    EXTRACT_SELF_POINTER(base,self);
    return mem_read(self,buf,n);
}

static status_t mem_virtual_destroy(struct file_base *base)
{
    EXTRACT_SELF_POINTER(base,self);
    return mem_destroy(self);
}

static int_ptr_t mem_virtual_write(struct file_base *base,const void *buf,int_ptr_t n)
{
    EXTRACT_SELF_POINTER(base,self);
    return mem_write(self,buf,n);
}

static fsize_t mem_virtual_seek(struct file_base *base,fsize_t off)
{
    EXTRACT_SELF_POINTER(base,self);
    return mem_seek(self,off);
}

static fsize_t mem_virtual_get_offset(struct file_base *base)
{
    EXTRACT_SELF_POINTER(base,self);
    return mem_get_offset(self);
}

static fsize_t mem_virtual_get_size(struct file_base *base)
{
    EXTRACT_SELF_POINTER(base,self);
    return mem_get_size(self);
}

static status_t mem_virtual_set_size(struct file_base *base,fsize_t size)
{
    EXTRACT_SELF_POINTER(base,self);
    return mem_set_size(self,size);
}

static status_t mem_virtual_add_block(struct file_base *base,fsize_t bsize)
{
    EXTRACT_SELF_POINTER(base,self);
    return mem_add_block(self,bsize);
}

static fsize_t mem_virtual_get_max_size(struct file_base *base)
{
    EXTRACT_SELF_POINTER(base,self);
    return mem_get_max_size(self);
}

/************************************************************************/
status_t mem_init_basic(struct mem *self)
{
    filebase_init_basic(&self->base_file_base);
    self->offset = 0;
    self->size = 0;
    self->max_size = 0;
    self->is_const = TRUE;
    self->is_self_alloc = 0;
	self->buffer = NULL;
    return OK;
}

status_t mem_init(struct mem *self)
{
    mem_init_basic(self);
    filebase_init(&self->base_file_base);

    self->base_file_base.read = mem_virtual_read;
    self->base_file_base.destroy = mem_virtual_destroy;
    self->base_file_base.write = mem_virtual_write;
    self->base_file_base.seek = mem_virtual_seek;
    self->base_file_base.get_offset = mem_virtual_get_offset;
    self->base_file_base.get_size = mem_virtual_get_size;
    self->base_file_base.set_size = mem_virtual_set_size;
    self->base_file_base.add_block = mem_virtual_add_block;
    self->base_file_base.get_max_size = mem_virtual_get_max_size;
    return OK;
}

status_t mem_destroy(struct mem *self)
{
    mem_free(self);
    filebase_base_destroy(&self->base_file_base);
    mem_init_basic(self);
    return OK;
}

status_t mem_copy(struct mem *self,struct mem *_p)
{
    ASSERT(_p);
    if(self == _p)return OK;

    filebase_copy(&self->base_file_base,&_p->base_file_base);
    self->offset = _p->offset;
    self->size = _p->size;
    self->max_size = _p->max_size;
    self->is_const = _p->is_const;
    self->is_self_alloc = _p->is_self_alloc;
    return OK;
}

status_t mem_comp(struct mem *self,struct mem *_p)
{
    ASSERT(_p);
    if(self == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t mem_print(struct mem *self,struct log_buffer *_buf)
{
    ASSERT(_buf);
    filebase_print(&self->base_file_base,_buf);
    log_buffer_log(_buf,"offset = %d",self->offset);
    log_buffer_log(_buf,"size = %d",self->size);
    log_buffer_log(_buf,"max_size = %d",self->max_size);
    log_buffer_log(_buf,"is_const = %d",self->is_const);
    log_buffer_log(_buf,"is_self_alloc = %d",self->is_self_alloc);
    return OK;
}

status_t mem_malloc(struct mem *self,int_ptr_t asize)
{
    asize += sizeof(int);
    ASSERT(asize > 0);
    ASSERT(self->buffer == NULL);
    X_MALLOC(self->buffer,char,asize);
    self->is_self_alloc = TRUE;
    self->size = 0;
    self->offset = 0;
    self->max_size = asize;
    self->is_const = FALSE;
    return OK;
}

status_t mem_free(struct mem *self)
{
    if(self->buffer == NULL)
        return ERROR;
    if(self->is_self_alloc)
    {        
        X_FREE(self->buffer);
        self->is_self_alloc = FALSE;
    }
    return OK;
}

status_t mem_realloc(struct mem *self,int_ptr_t new_size)
{
    struct mem new_mem;
    fsize_t old_off = self->offset;
    int_ptr_t copy_size = (int_ptr_t)self->size;

    if(new_size == self->size)
        return OK;

    mem_init(&new_mem);
    mem_malloc(&new_mem,new_size);

    if(new_size < copy_size)
        copy_size = new_size;

    crt_memcpy(new_mem.buffer,self->buffer,copy_size);

    mem_free(self);

    self->buffer = new_mem.buffer;
    self->max_size = new_mem.max_size;

    if(self->size > new_size)
        self->size = new_size;

    self->is_const = FALSE;
    self->is_self_alloc = TRUE;
    new_mem.buffer = NULL;

    mem_seek(self,old_off);
    mem_destroy(&new_mem);
    return OK;
}

int_ptr_t mem_read(struct mem *self,void *buf,int_ptr_t n)
{
    int_ptr_t  copy_length;
    ASSERT(self->buffer && buf);
    if(self->size - self->offset > n)
        copy_length=n;
    else
        copy_length=(int_ptr_t)(self->size - self->offset);
    if(copy_length > 0)
    {
        crt_memcpy(buf,(self->buffer+self->offset),copy_length);
        self->offset += copy_length;
        return copy_length;
    }
    return 0;
}

int_ptr_t mem_write(struct mem *self,const void *buf,int_ptr_t n)
{
    int_ptr_t  copy_length;
    
    ASSERT(!self->is_const);
    ASSERT(self->buffer && buf);

    if(n + self->offset > self->size)
    {
        mem_add_block(self,n + self->offset - self->size);
    }

    if(self->size - self->offset > n)
        copy_length=n;
    else
        copy_length=(int_ptr_t)(self->size - self->offset);
    if(copy_length > 0)
    {
        crt_memcpy((self->buffer+self->offset),buf,copy_length);
        self->offset += copy_length;
        return copy_length;
    }

    return 0;
}

status_t mem_zero(struct mem *self)
{
    ASSERT(self->buffer);
    ASSERT(!self->is_const);
    crt_memset(self->buffer,0,self->max_size);
    return OK;
}

status_t mem_set_raw_buffer(struct mem *self,void *p,int_ptr_t s,bool_t is_const)
{
    ASSERT(self->buffer == NULL);
    self->buffer = (char*)p;
    self->size = s;
    self->max_size = s;
    self->is_const = is_const;
    self->is_self_alloc = FALSE;
    self->offset = 0;
    return OK;
}

fsize_t mem_seek(struct mem*self,fsize_t off)
{
    if(off > self->size)off = self->size;
    if(off < 0 )off = 0;
    self->offset = (int_ptr_t)off;
    return self->offset;
}


int_ptr_t mem_get_offset(struct mem *self)
{
    return self->offset;
}

int_ptr_t mem_get_size(struct mem *self)
{
    return self->size;
}

status_t mem_set_size(struct mem *self,fsize_t _size)
{
    if(_size >= 0 && _size <= self->max_size)
    {
        self->size = (int_ptr_t)_size;
        self->offset = 0;
        return OK;
    }
    return ERROR;
}

status_t mem_add_block(struct mem *self,fsize_t bsize)
{
    if(self->buffer == NULL)
    {
        mem_malloc(self,(int_ptr_t)bsize);
        return OK;
    }

    self->size += (int_ptr_t)bsize;
    if(self->size < 0)
        self->size = 0;
    if(self->size > self->max_size)
        self->size = self->max_size;
    return OK;
}

fsize_t mem_get_max_size(struct mem *self)
{
    return self->max_size;
}

status_t mem_set_str(struct mem *self,const char *p)
{
    ASSERT(p);
    return mem_set_raw_buffer(self,(void*)p,crt_strlen(p),TRUE);
}

const char *mem_cstr(struct mem *self)
{
    ASSERT(self->buffer);
    if(!self->is_const)
    {
        if(self->size >= 0 && self->size < self->max_size)
            self->buffer[self->size] = 0;
        else if(self->size >= self->max_size && self->max_size > 0)
            self->buffer[self->max_size-1] = 0;
    }
    return self->buffer;
}

char * mem_get_raw_buffer(struct mem *self)
{
    return self->buffer;
}

