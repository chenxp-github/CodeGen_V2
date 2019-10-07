#include "c_log_buffer.h"
#include "syslog.h"
#include "mem_tool.h"

#define LOCAL_BUFFER_SIZE 8192

status_t log_buffer_init_basic(struct log_buffer *self)
{
    self->m_buf = NULL;
    self->m_buf_size = 0;
    self->m_buf_len = 0;
    self->m_log_level = 0;
    return OK;
}

status_t log_buffer_init(struct log_buffer *self)
{
    log_buffer_init_basic(self);
    return OK;
}

status_t log_buffer_destroy(struct log_buffer *self)
{
    FREE(self->m_buf);
    log_buffer_init_basic(self);
    return OK;
}

status_t log_buffer_copy(struct log_buffer *self,struct log_buffer *_p)
{
    int i = 0;

    ASSERT(_p);
    if(self == _p)return OK;

    log_buffer_alloc_buf(self,_p->m_buf_size);
    for(i = 0; i < self->m_buf_size; i++)
    {
        self->m_buf[i] = _p->m_buf[i];
    }
    self->m_buf_len = _p->m_buf_len;
    self->m_log_level = _p->m_log_level;
    return OK;
}

status_t log_buffer_comp(struct log_buffer *self,struct log_buffer *_p)
{
    ASSERT(_p);
    if(self == _p)return 0;
    ASSERT(0);
    return 0;
}

char* log_buffer_get_buf(struct log_buffer *self)
{
    return self->m_buf;
}

int log_buffer_get_buf_size(struct log_buffer *self)
{
    return self->m_buf_size;
}
char log_buffer_get_buf_elem(struct log_buffer *self,int _index)
{
    ASSERT(self->m_buf);
    ASSERT(_index >= 0 && _index < self->m_buf_size);
    return self->m_buf[_index];
}
int log_buffer_get_buf_len(struct log_buffer *self)
{
    return self->m_buf_len;
}

int log_buffer_get_log_level(struct log_buffer *self)
{
    return self->m_log_level;
}

status_t log_buffer_alloc_buf(struct log_buffer *self,int _len)
{
    if(self->m_buf_size == _len)
        return OK;

    X_FREE(self->m_buf);

    if(_len > 0)
    {
        X_MALLOC(self->m_buf,char,_len);
        memset(self->m_buf,0,sizeof(char)*_len);
    }
    self->m_buf_size = _len;
    return OK;
}

status_t log_buffer_set_buf(struct log_buffer *self,char _buf[], int _len)
{
    ASSERT(_buf);
    log_buffer_alloc_buf(self,_len);
    memcpy(self->m_buf,_buf,sizeof(char)*_len);
    return OK;
}

status_t log_buffer_set_buf_elem(struct log_buffer *self,int _index,char _buf)
{
    ASSERT(self->m_buf);
    ASSERT(_index >= 0 && _index < self->m_buf_size);
    self->m_buf[_index] = _buf;
    return OK;
}

status_t log_buffer_set_buf_len(struct log_buffer *self,int _buf_len)
{
    self->m_buf_len = _buf_len;
    return OK;
}

status_t log_buffer_set_log_level(struct log_buffer *self,int _log_level)
{
    self->m_log_level = _log_level;
    return OK;
}

status_t log_buffer_write(struct log_buffer *self, const char *str, int size)
{
    int new_len = self->m_buf_len + size;
    //auto resize buffer
    if(new_len >= self->m_buf_size)
    {
        char *tmp;
        X_MALLOC(tmp,char,new_len*2);
        if(self->m_buf)
        {
            memcpy(tmp,self->m_buf,self->m_buf_len);
            X_FREE(self->m_buf); //free old buffer
        }
        self->m_buf = tmp;
        self->m_buf_size = new_len*2;
    }
    memcpy(self->m_buf + self->m_buf_len,str,size);
    self->m_buf_len += size;
    return OK;
}

status_t log_buffer_puts(struct log_buffer *self,const char *str)
{
    ASSERT(str);
    return log_buffer_write(self,str,strlen(str));
}

status_t log_buffer_printf(struct log_buffer *self, const char* format, ...)
{
    char buffer[LOCAL_BUFFER_SIZE];
    crt_va_list ap;
    crt_va_start(ap, format);
    crt_vsnprintf (buffer,sizeof(buffer),format, ap);
    log_buffer_puts(self,buffer);
    crt_va_end(ap);
    return OK;
}

status_t log_buffer_tab(struct log_buffer *self)
{
    int i;

    for(i = 0; i < self->m_log_level; i++)
    {
        log_buffer_puts(self,"    ");    
    }
    return OK;
}


status_t log_buffer_log(struct log_buffer *self, const char* format, ...)
{
    char buffer[LOCAL_BUFFER_SIZE];

    crt_va_list ap;
    crt_va_start(ap, format);
    crt_vsnprintf (buffer,sizeof(buffer),format, ap);

    log_buffer_tab(self);
    log_buffer_puts(self,buffer);
    log_buffer_nl(self);

    crt_va_end(ap);
    return OK;
}

status_t log_buffer_inc_level(struct log_buffer *self, int level)
{
    self->m_log_level += level;
    return OK;
}

const char * log_buffer_get_text(struct log_buffer *self)
{
    ASSERT(self->m_buf_len < self->m_buf_size);
    self->m_buf[self->m_buf_len] = 0;
    return self->m_buf;
}

status_t log_buffer_nl(struct log_buffer *self)
{
    return log_buffer_puts(self,"\r\n");
}