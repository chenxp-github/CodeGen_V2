#ifndef _C_PRINT_BUFFER_H
#define _C_PRINT_BUFFER_H

#include "cruntime.h"

struct log_buffer{
    char *m_buf;
    int m_buf_size;
    int m_buf_len;
    int m_log_level;
};

status_t log_buffer_init_basic(struct log_buffer *self);
status_t log_buffer_init(struct log_buffer *self);
status_t log_buffer_destroy(struct log_buffer *self);
status_t log_buffer_copy(struct log_buffer *self,struct log_buffer *_p);
status_t log_buffer_comp(struct log_buffer *self,struct log_buffer *_p);
char* log_buffer_get_buf(struct log_buffer *self);
int log_buffer_get_buf_size(struct log_buffer *self);
char log_buffer_get_buf_elem(struct log_buffer *self,int _index);
int log_buffer_get_buf_len(struct log_buffer *self);
int log_buffer_get_log_level(struct log_buffer *self);
status_t log_buffer_alloc_buf(struct log_buffer *self,int _len);
status_t log_buffer_set_buf(struct log_buffer *self,char _buf[], int _len);
status_t log_buffer_set_buf_elem(struct log_buffer *self,int _index,char _buf);
status_t log_buffer_set_buf_len(struct log_buffer *self,int _buf_len);
status_t log_buffer_set_log_level(struct log_buffer *self,int _log_level);

status_t log_buffer_write(struct log_buffer *self, const char *str, int size);
status_t log_buffer_puts(struct log_buffer *self,const char *str);
status_t log_buffer_log(struct log_buffer *self, const char* format, ...);
status_t log_buffer_tab(struct log_buffer *self);
status_t log_buffer_printf(struct log_buffer *self, const char* format, ...);
status_t log_buffer_nl(struct log_buffer *self);
status_t log_buffer_inc_level(struct log_buffer *self, int level);
const char * log_buffer_get_text(struct log_buffer *self);

#endif
