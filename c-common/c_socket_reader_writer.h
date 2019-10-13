#ifndef _C_SOCKET_READER_WRITER_H
#define _C_SOCKET_READER_WRITER_H

#include "cruntime.h"
#include "c_log_buffer.h"
#include "c_taskmgr.h"
#include "c_socket.h"
#include "c_file_base.h"
#include "c_closure.h"

enum{	
	C_SOCKET_READER_WRITER_EVENT_ERROR = 1,		
};



enum{
	C_SOCKET_READER_WRITER_ERROR_NONE = 0,
	C_SOCKET_READER_WRITER_ERROR_CONNECT_ERROR = 1000,
	C_SOCKET_READER_WRITER_ERROR_READ_TIMEOUT,
	C_SOCKET_READER_WRITER_ERROR_CONNECTION_CLOSED,
	C_SOCKET_READER_WRITER_ERROR_WRITE_TIMEOUT,
	C_SOCKET_READER_WRITER_ERROR_WRITE_ERROR,
	C_SOCKET_READER_WRITER_ERROR_READ_ERROR,		
	C_SOCKET_READER_WRITER_ERROR_REACH_END_OF_FILE,		
	C_SOCKET_READER_WRITER_ERROR_LINE_IS_TOO_LONG,
};



struct socket_reader_writer{
    struct taskmgr *i_task_mgr;
    struct socket *i_socket;
    struct file_base *i_read_dest_file;
    struct file_base *i_write_src_file;
    fsize_t total_read_length;
    fsize_t write_end_offset;
    fsize_t write_offset;
    fsize_t read_length;
    fsize_t read_size_this_time;
    fsize_t write_size_this_time;
    uint32_t write_timer;
    uint32_t read_timer;
    uint32_t timeout;
    struct closure callback;
};

status_t socketreaderwriter_init_basic(struct socket_reader_writer *self);
status_t socketreaderwriter_init(struct socket_reader_writer *self);
status_t socketreaderwriter_destroy(struct socket_reader_writer *self);

struct taskmgr* socketreaderwriter_get_task_mgr(struct socket_reader_writer *self);
struct socket* socketreaderwriter_get_socket(struct socket_reader_writer *self);
struct file_base* socketreaderwriter_get_read_dest_file(struct socket_reader_writer *self);
struct file_base* socketreaderwriter_get_write_src_file(struct socket_reader_writer *self);
fsize_t socketreaderwriter_get_total_read_length(struct socket_reader_writer *self);
fsize_t socketreaderwriter_get_write_end_offset(struct socket_reader_writer *self);
fsize_t socketreaderwriter_get_write_offset(struct socket_reader_writer *self);
fsize_t socketreaderwriter_get_read_length(struct socket_reader_writer *self);
fsize_t socketreaderwriter_get_read_size_this_time(struct socket_reader_writer *self);
fsize_t socketreaderwriter_get_write_size_this_time(struct socket_reader_writer *self);
uint32_t socketreaderwriter_get_write_timer(struct socket_reader_writer *self);
uint32_t socketreaderwriter_get_read_timer(struct socket_reader_writer *self);
uint32_t socketreaderwriter_get_timeout(struct socket_reader_writer *self);
struct closure* socketreaderwriter_get_callback(struct socket_reader_writer *self);

status_t socketreaderwriter_set_task_mgr(struct socket_reader_writer *self,struct taskmgr *_i_task_mgr);
status_t socketreaderwriter_set_socket(struct socket_reader_writer *self,struct socket *_i_socket);
status_t socketreaderwriter_set_read_dest_file(struct socket_reader_writer *self,struct file_base *_i_read_dest_file);
status_t socketreaderwriter_set_write_src_file(struct socket_reader_writer *self,struct file_base *_i_write_src_file);
status_t socketreaderwriter_set_total_read_length(struct socket_reader_writer *self,fsize_t _total_read_length);
status_t socketreaderwriter_set_write_end_offset(struct socket_reader_writer *self,fsize_t _write_end_offset);
status_t socketreaderwriter_set_write_offset(struct socket_reader_writer *self,fsize_t _write_offset);
status_t socketreaderwriter_set_read_length(struct socket_reader_writer *self,fsize_t _read_length);
status_t socketreaderwriter_set_read_size_this_time(struct socket_reader_writer *self,fsize_t _read_size_this_time);
status_t socketreaderwriter_set_write_size_this_time(struct socket_reader_writer *self,fsize_t _write_size_this_time);
status_t socketreaderwriter_set_write_timer(struct socket_reader_writer *self,uint32_t _write_timer);
status_t socketreaderwriter_set_read_timer(struct socket_reader_writer *self,uint32_t _read_timer);
status_t socketreaderwriter_set_timeout(struct socket_reader_writer *self,uint32_t _timeout);

status_t socketreaderwriter_prepare_file_for_write(struct socket_reader_writer *self,struct file_base *ifile, fsize_t start, fsize_t size);
status_t socketreaderwriter_error(struct socket_reader_writer *self,int err);
status_t socketreaderwriter_do_write(struct socket_reader_writer *self,uint32_t interval);
status_t socketreaderwriter_prepare_file_for_read(struct socket_reader_writer *self,struct file_base *ifile, fsize_t size, bool_t reset);
status_t socketreaderwriter_do_read(struct socket_reader_writer *self,uint32_t interval);
status_t socketreaderwriter_do_read_until_eol(struct socket_reader_writer *self,uint32_t interval,bool_t only_lf);
status_t socketreaderwriter_do_read_until_empty_line(struct socket_reader_writer *self,uint32_t interval);

#endif
