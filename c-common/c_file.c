#include "c_file.h"
#include "syslog.h"
#include "mem_tool.h"

FILE_BASE_VIRTUAL_FUNCTIONS_DEFINE(struct file, file)
/************************************************************************/
status_t file_init_basic(struct file *self)
{
    filebase_init_basic(&self->base_file_base);
    self->fp = ERROR_FILE_HANDLE;
    self->file_buf = NULL;
    self->buf_size = 0;
    self->init_buf_size = 0;
    self->offset = 0;
    self->size = 0;
    self->buf_start = 0;
    return OK;
}

status_t file_init(struct file *self)
{
    file_init_basic(self);
    filebase_init(&self->base_file_base);
    FILE_BASE_INIT_VIRTUAL_FUNCTIONS(file);
    return OK;
}

status_t file_destroy(struct file *self)
{
    file_close_file(self);
    filebase_base_destroy(&self->base_file_base);
    file_init_basic(self);
    return OK;
}

status_t file_copy(struct file *self,struct file *_p)
{
    ASSERT(_p);
    if(self == _p)return OK;
    ASSERT(0);
    return OK;
}

status_t file_comp(struct file *self,struct file *_p)
{
    ASSERT(_p);
    if(self == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t file_print(struct file *self,struct log_buffer *_buf)
{
    ASSERT(_buf);
    filebase_print(&self->base_file_base,_buf);
    return OK;
}

status_t file_open_file(struct file *self,const char *fn , const char *mode)
{
    ASSERT(!crt_is_file_handle(self->fp));
    self->fp = crt_fopen(fn,mode);
    if(!crt_is_file_handle(self->fp)) 
        return ERROR;    
    file_get_size_real(self);
    file_buffer_block(self,0);
    return OK;
}

status_t file_close_file(struct file *self)
{   
    if(crt_is_file_handle(self->fp))
    {
        crt_fclose(self->fp);
        self->fp = ERROR_FILE_HANDLE;
    }
    X_FREE(self->file_buf);   
    file_init_basic(self);  
    return OK;
}

fsize_t file_get_size(struct file *self)
{
    return self->size;
}

fsize_t file_get_size_real(struct file *self)
{
    fsize_t old_off,size;    
    old_off = crt_ftell(self->fp);
    crt_fseek(self->fp,0,SEEK_END);
    size = crt_ftell(self->fp);
    crt_fseek(self->fp,old_off,SEEK_SET);
    self->size = size;
    return size;    
}

fsize_t file_seek(struct file *self,fsize_t n_offset)
{
    if(n_offset > file_get_size(self))
        n_offset = file_get_size(self);
    
    if(n_offset < 0 )
        n_offset = 0;
    
    self->offset = n_offset;
    return self->offset;
}


int_ptr_t file_read(struct file *self,void *buf,int_ptr_t n_byte_to_read)
{
    fsize_t old_off,rs;
    
    if(n_byte_to_read <= 0)return 0;
    old_off = self->offset; 
    
    if(file_has_buffer(self))
    {
        if(old_off >= self->buf_start && (old_off + n_byte_to_read <= self->buf_start + self->buf_size))
        {
            crt_memcpy(buf,self->file_buf+(old_off-self->buf_start),n_byte_to_read);
            file_seek(self,old_off + n_byte_to_read);          
        }
        else
        {           
            crt_fseek(self->fp,self->offset,SEEK_SET);
            rs = crt_fread(self->fp,(void*)buf,n_byte_to_read);
            self->offset += rs;
            
            if(old_off < self->buf_start)
                file_buffer_block(self,old_off - self->init_buf_size + n_byte_to_read);
            else
                file_buffer_block(self,old_off);
        }    
    }
    else
    {
        crt_fseek(self->fp,self->offset,SEEK_SET);
        rs = crt_fread(self->fp,(void*)buf,n_byte_to_read);
        self->offset += rs;
    }
    
    return (int_ptr_t)(self->offset - old_off);
}

int_ptr_t file_write(struct file *self,const void *buf,int_ptr_t n_byte_to_write)
{
    fsize_t old_off,ws;
    
    if(n_byte_to_write <= 0)return 0;
    
    old_off = self->offset;
    crt_fseek(self->fp,self->offset,SEEK_SET);
    ws = crt_fwrite( self->fp,(void*)buf, n_byte_to_write);
    self->offset += ws;
    
    if(self->offset >= self->size)
        self->size = self->offset;
    return (int_ptr_t)(self->offset - old_off);
}

status_t file_set_size(struct file *self,fsize_t n_size)
{
    self->size = n_size; 
    self->offset = 0;
    return OK;
}

fsize_t file_get_offset(struct file *self)
{
    return self->offset;
}
status_t file_add_block(struct file *self,fsize_t bsize)
{
    return OK;
}

fsize_t file_get_max_size(struct file *self)
{
    return file_get_size(self);
}

status_t file_has_buffer(struct file *self)
{
    return self->file_buf != NULL;
}

status_t file_set_buf_size(struct file *self,int_ptr_t size)
{
    ASSERT(!file_is_opened(self));
    X_FREE(self->file_buf);
    X_MALLOC(self->file_buf,char,size);
    self->buf_size = size;
    self->buf_start = -1;
    self->init_buf_size = size;
    
    return OK;
}

status_t file_buffer_block(struct file *self,fsize_t start_pos)
{   
    int_ptr_t rs;
    if(!file_has_buffer(self))
        return ERROR;
    if(start_pos < 0)start_pos = 0;
    if(self->buf_start == start_pos)
        return OK;
    crt_fseek(self->fp,start_pos,SEEK_SET);
    rs  = crt_fread(self->fp,self->file_buf,self->init_buf_size);
    self->buf_start = start_pos;
    self->buf_size = (int_ptr_t)rs;
    return OK;
}

bool_t file_is_opened(struct file *self)
{
    return crt_is_file_handle(self->fp) != 0;
}
