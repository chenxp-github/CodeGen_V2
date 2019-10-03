#include "c_mem_file.h"
#include "syslog.h"
#include "mem_tool.h"

FILE_BASE_VIRTUAL_FUNCTIONS_DEFINE(struct mem_file, memfile)
/********************************************************/
status_t memfile_init_basic(struct mem_file *self)
{
    filebase_init_basic(&self->base_file_base);
    self->size = 0;
    self->pages = 0;
    self->offset = 0;
    self->max_pages = 0;
    self->base = NULL;
    self->page_size = 0;
    self->mod_n = ~0x0;
    self->shift_n = 0;
    return OK;
}

static status_t memfile_init_shift(struct mem_file *self)
{
    int32_t i;
    
    for(i = 5; i < (int32_t)sizeof(fsize_t)*8; i++)
    {
        if( (fsize_t)(1 << i) == self->page_size)
        {
            self->shift_n = i;
            self->mod_n = self->page_size - 1;
            return OK;
        }
    }
    
    XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
        "error page size, must be pow of 2."
    );
    return ERROR;
}

status_t memfile_init_ex(struct mem_file *self,int_ptr_t page_size,int_ptr_t max_pages)
{
    memfile_init_basic(self);
    filebase_init(&self->base_file_base);
    ASSERT(max_pages > 0);
    self->max_pages = max_pages;
    self->offset = 0;
    self->pages = 0;
    self->page_size = page_size;
    FILE_BASE_INIT_VIRTUAL_FUNCTIONS(memfile);
    return memfile_init_shift(self);
}

status_t memfile_init(struct mem_file *self)
{
    return memfile_init_ex(self,64*1024,4*1024);
}

status_t memfile_destroy(struct mem_file *self)
{
    if(self->base)
    {
        int i;
        for(i=0;i<self->pages;i++)
        {
            X_FREE(self->base[i]);
        }
        X_FREE(self->base);
    }
    filebase_base_destroy(&self->base_file_base);
    memfile_init_basic(self);
    return OK;
}


status_t memfile_copy(struct mem_file *self,struct mem_file *_p)
{
    ASSERT(_p);
    if(self == _p)return OK;

    filebase_copy(&self->base_file_base,&_p->base_file_base);
    self->size = _p->size;
    self->pages = _p->pages;
    self->offset = _p->offset;
    self->max_pages = _p->max_pages;
    self->base = _p->base;
    self->page_size = _p->page_size;
    self->mod_n = _p->mod_n;
    self->shift_n = _p->shift_n;
    return OK;
}

status_t memfile_comp(struct mem_file *self,struct mem_file *_p)
{
    ASSERT(_p);
    if(self == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t memfile_print(struct mem_file *self,struct log_buffer *_buf)
{
    ASSERT(_buf);
    filebase_print(&self->base_file_base,_buf);
    log_buffer_log(_buf,"size = %d",self->size);
    log_buffer_log(_buf,"pages = %d",self->pages);
    log_buffer_log(_buf,"offset = %d",self->offset);
    log_buffer_log(_buf,"max_pages = %d",self->max_pages);
    log_buffer_log(_buf,"base = 0x%p",self->base);
    log_buffer_log(_buf,"page_size = %d",self->page_size);
    log_buffer_log(_buf,"mod_n = %d",self->mod_n);
    log_buffer_log(_buf,"shift_n = %d",self->shift_n);
    return OK;
}

status_t memfile_add_page(struct mem_file *self)
{
    if(self->base == NULL)
    {
        int i;
        X_MALLOC(self->base,char *,self->max_pages);
        for(i=0;i<self->max_pages;i++)
            self->base[i] = NULL;
    }
    
    if(self->pages < self->max_pages - 1)
    {
        X_MALLOC(self->base[self->pages] ,char, self->page_size);
        self->pages ++;
    }
    else
    {
        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
            "memfile: too many pages!"
        );
        return ERROR;
    }
    
    return OK;
}

status_t memfile_add_block(struct mem_file *self,fsize_t block_size)
{
    int_ptr_t i,pn,new_page;
    new_page = ((self->size+(int_ptr_t)block_size) >> self->shift_n) + 1;
    pn=new_page - self->pages;
    if(new_page > self->pages)
    {
        for(i = 0; i < pn; i++)
            memfile_add_page(self);
    }
    self->size += (int_ptr_t)block_size;
    return OK;
}

int_ptr_t memfile_read(struct mem_file *self,void *buf,int_ptr_t rsize)
{
    int_ptr_t left,sum=0,read_size=0;
    int_ptr_t block_end;
    int_ptr_t block=self->offset >> self->shift_n;
    int_ptr_t off=self->offset & self->mod_n;
    
    if(self->offset + rsize > self->size)
        rsize=self->size - self->offset;
    
    left = rsize;
    while(sum<rsize)
    {
        block_end = (block << self->shift_n) + self->page_size;
        if(self->offset + left < block_end)
        {
            read_size=left;
        }
        else
        {
            read_size = block_end - self->offset;
        }
        crt_memcpy((char*)buf+sum , self->base[block] + off, read_size);
        self->offset += read_size;
        sum += read_size;
        left -= read_size;
        block = self->offset >> self->shift_n;
        off = self->offset & self->mod_n;
    }
    return sum;
}


int_ptr_t memfile_write(struct mem_file *self,const void *buf,int_ptr_t wsize)
{
    int_ptr_t sum=0, write_size=0;
    int_ptr_t block_end;
    int_ptr_t left=wsize;
    int_ptr_t block= self->offset >> self->shift_n;
    int_ptr_t off=self->offset & self->mod_n;
    
    if(wsize <= 0) return 0;
    
    if(wsize + self->offset > self->size)
    {
        memfile_add_block(self,wsize + self->offset - self->size);
    }
    
    while(sum<wsize)
    {
        block_end = (block << self->shift_n) + self->page_size;
        if(self->offset + left < block_end)
        {
            write_size=left;
        }
        else
        {
            write_size=block_end - self->offset;
        }
        crt_memcpy(self->base[block]+off,(char*)buf+sum,write_size);
        self->offset += write_size;
        sum += write_size;
        left -= write_size;
        block = self->offset >> self->shift_n;
        off = self->offset & self->mod_n;
    }
    return sum;
}


fsize_t memfile_get_max_size(struct mem_file *self)
{
    return self->max_pages * self->page_size;
}

fsize_t memfile_seek(struct mem_file *self,fsize_t off)
{
    if(off > self->size)off = self->size;
    if(off < 0 )off = 0;
    self->offset = (int_ptr_t)off;
    return self->offset;
}

status_t memfile_set_size(struct mem_file *self,fsize_t s)
{
    if(s < 0) return ERROR;
    if(s > memfile_get_max_size(self))
        return ERROR;
    self->size = (int_ptr_t)s;
    self->offset = 0;
    return OK;
}

fsize_t memfile_get_offset(struct mem_file *self)
{
    return self->offset;
}

fsize_t memfile_get_size(struct mem_file *self)
{
    return self->size;
}


status_t memfile_transfer(struct mem_file *self,struct mem_file *from)
{
    ASSERT(from);

    memfile_destroy(self);

    self->size = from->size;
    self->base = from->base;
    self->max_pages = from->max_pages;
    self->offset = from->offset;
    self->pages = from->pages;
    self->page_size = from->page_size;
    self->mod_n = from->mod_n;
    self->shift_n = from->shift_n;

    memfile_init_basic(from);
    return OK;
}