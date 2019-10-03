#include "c_part_file.h"
#include "syslog.h"
#include "mem_tool.h"

FILE_BASE_VIRTUAL_FUNCTIONS_DEFINE(struct part_file, partfile)
/************************************************************************/
status_t partfile_init_basic(struct part_file *self)
{
    filebase_init_basic(&self->base_file_base);
    self->i_host_file = NULL;
    self->offset = 0;
    self->size = 0;
    self->host_off = 0;
    self->max_size = 0;
    return OK;
}

status_t partfile_init(struct part_file *self)
{
    partfile_init_basic(self);
    filebase_init(&self->base_file_base);
    FILE_BASE_INIT_VIRTUAL_FUNCTIONS(partfile);
    return OK;
}

status_t partfile_destroy(struct part_file *self)
{
    filebase_base_destroy(&self->base_file_base);
    partfile_init_basic(self);
    return OK;
}

status_t partfile_copy(struct part_file *self,struct part_file *_p)
{
    ASSERT(_p);
    if(self == _p)return OK;

    filebase_copy(&self->base_file_base,&_p->base_file_base);
    self->i_host_file = _p->i_host_file;
    self->offset = _p->offset;
    self->size = _p->size;
    self->host_off = _p->host_off;
    self->max_size = _p->max_size;
    return OK;
}

status_t partfile_comp(struct part_file *self,struct part_file *_p)
{
    ASSERT(_p);
    if(self == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t partfile_print(struct part_file *self,struct log_buffer *_buf)
{
    ASSERT(_buf);
    filebase_print(&self->base_file_base,_buf);
    return OK;
}


fsize_t partfile_get_offset(struct part_file *self)
{
    return self->offset;
}

fsize_t partfile_get_size(struct part_file *self)
{
    return self->size;
}

int_ptr_t partfile_read(struct part_file *self,void *buf, int_ptr_t n)
{
    fsize_t old_host_offset, max_rs;
    int_ptr_t read_size;

    ASSERT(self->i_host_file);
    
    if(n <= 0)return 0;
    old_host_offset = filebase_get_offset(self->i_host_file);
    
    filebase_seek(self->i_host_file,self->host_off + self->offset);
    max_rs = partfile_get_size(self) - self->offset;
    
    if(n > max_rs)n = (int_ptr_t)max_rs;
    self->offset += n;
   
    read_size = filebase_read(self->i_host_file,buf,n);
    filebase_seek(self->i_host_file,old_host_offset);

    return read_size;
}

int_ptr_t partfile_write(struct part_file *self,const void *buf, int_ptr_t n)
{
    fsize_t old_host_offset,max_ws;
    int_ptr_t write_size;

    ASSERT(self->i_host_file);
    
    if(n <= 0)return 0;
    
    if(n + self->offset > self->size)
    {
        partfile_add_block(self,n + self->offset - self->size);
    }
    
    old_host_offset = filebase_get_offset(self->i_host_file);
    filebase_seek(self->i_host_file,self->host_off + self->offset);
    max_ws = partfile_get_size(self) - self->offset;
    
    if(n > max_ws) n = (int_ptr_t)max_ws;
    self->offset += n;
    
    write_size = filebase_write(self->i_host_file,buf,n);
    filebase_seek(self->i_host_file,old_host_offset);
    return write_size;
}

int partfile_set_size(struct part_file *self,fsize_t s)
{
    if(s > partfile_get_max_size(self))
        s = partfile_get_max_size(self);
    if(s < 0) s = 0;
    self->size = s;
    self->offset = 0;
    return OK;
}

fsize_t partfile_get_max_size(struct part_file *self)
{
    return self->max_size;
}

int partfile_add_block(struct part_file *self,fsize_t bsize)
{
    fsize_t max = partfile_get_max_size(self);
    self->size += bsize;
    if(self->size > max)
        self->size = max;
    return OK;
}

fsize_t partfile_seek(struct part_file *self,fsize_t off)
{
    if(self->offset > self->size)
        self->offset = self->size;
    if(off < 0 )off = 0;
    self->offset = off;
    return self->offset;
}

int partfile_set_host_file(struct part_file *self,struct file_base *host, fsize_t off, fsize_t size)
{
    ASSERT(host);
    self->i_host_file = host;
    self->host_off = off;
    self->max_size = size;
    if(size > filebase_get_max_size(host) - self->host_off)
        self->max_size = filebase_get_size(host) - self->host_off;
    partfile_set_size(self,self->max_size);
    return OK;
}

fsize_t partfile_get_host_offset(struct part_file *self)
{
    return self->host_off;
}
