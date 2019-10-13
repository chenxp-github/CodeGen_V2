#include "c_socket_reader_writer.h"
#include "syslog.h"
#include "mem_tool.h"

#define MAX_LINE_LENGTH (64*1024)

#ifndef SOCKETRW_TIMEOUT
#define SOCKETRW_TIMEOUT (90*1000)
#endif

status_t socketreaderwriter_init_basic(struct socket_reader_writer *self)
{
    self->i_task_mgr = NULL;
    self->i_socket = NULL;
    self->i_read_dest_file = NULL;
    self->i_write_src_file = NULL;
    self->total_read_length = 0;
    self->write_end_offset = 0;
    self->write_offset = 0;
    self->read_length = 0;
    self->read_size_this_time = 0;
    self->write_size_this_time = 0;
    self->write_timer = 0;
    self->read_timer = 0;
    self->timeout = 0;
    closure_init_basic(&self->callback);
    return OK;
}

status_t socketreaderwriter_init(struct socket_reader_writer *self)
{
    socketreaderwriter_init_basic(self);
    closure_init(&self->callback);
    return OK;
}

status_t socketreaderwriter_destroy(struct socket_reader_writer *self)
{
    closure_destroy(&self->callback);
    socketreaderwriter_init_basic(self);
    return OK;
}


struct taskmgr* socketreaderwriter_get_task_mgr(struct socket_reader_writer *self)
{
    return self->i_task_mgr;
}

struct socket* socketreaderwriter_get_socket(struct socket_reader_writer *self)
{
    return self->i_socket;
}

struct file_base* socketreaderwriter_get_read_dest_file(struct socket_reader_writer *self)
{
    return self->i_read_dest_file;
}

struct file_base* socketreaderwriter_get_write_src_file(struct socket_reader_writer *self)
{
    return self->i_write_src_file;
}

fsize_t socketreaderwriter_get_total_read_length(struct socket_reader_writer *self)
{
    return self->total_read_length;
}

fsize_t socketreaderwriter_get_write_end_offset(struct socket_reader_writer *self)
{
    return self->write_end_offset;
}

fsize_t socketreaderwriter_get_write_offset(struct socket_reader_writer *self)
{
    return self->write_offset;
}

fsize_t socketreaderwriter_get_read_length(struct socket_reader_writer *self)
{
    return self->read_length;
}

fsize_t socketreaderwriter_get_read_size_this_time(struct socket_reader_writer *self)
{
    return self->read_size_this_time;
}

fsize_t socketreaderwriter_get_write_size_this_time(struct socket_reader_writer *self)
{
    return self->write_size_this_time;
}

uint32_t socketreaderwriter_get_write_timer(struct socket_reader_writer *self)
{
    return self->write_timer;
}

uint32_t socketreaderwriter_get_read_timer(struct socket_reader_writer *self)
{
    return self->read_timer;
}

uint32_t socketreaderwriter_get_timeout(struct socket_reader_writer *self)
{
    return self->timeout;
}

struct closure* socketreaderwriter_get_callback(struct socket_reader_writer *self)
{
    return &self->callback;
}


status_t socketreaderwriter_set_task_mgr(struct socket_reader_writer *self,struct taskmgr *_i_task_mgr)
{
    self->i_task_mgr = _i_task_mgr;
    return OK;
}

status_t socketreaderwriter_set_socket(struct socket_reader_writer *self,struct socket *_i_socket)
{
    self->i_socket = _i_socket;
    return OK;
}

status_t socketreaderwriter_set_read_dest_file(struct socket_reader_writer *self,struct file_base *_i_read_dest_file)
{
    self->i_read_dest_file = _i_read_dest_file;
    return OK;
}

status_t socketreaderwriter_set_write_src_file(struct socket_reader_writer *self,struct file_base *_i_write_src_file)
{
    self->i_write_src_file = _i_write_src_file;
    return OK;
}

status_t socketreaderwriter_set_total_read_length(struct socket_reader_writer *self,fsize_t _total_read_length)
{
    self->total_read_length = _total_read_length;
    return OK;
}

status_t socketreaderwriter_set_write_end_offset(struct socket_reader_writer *self,fsize_t _write_end_offset)
{
    self->write_end_offset = _write_end_offset;
    return OK;
}

status_t socketreaderwriter_set_write_offset(struct socket_reader_writer *self,fsize_t _write_offset)
{
    self->write_offset = _write_offset;
    return OK;
}

status_t socketreaderwriter_set_read_length(struct socket_reader_writer *self,fsize_t _read_length)
{
    self->read_length = _read_length;
    return OK;
}

status_t socketreaderwriter_set_read_size_this_time(struct socket_reader_writer *self,fsize_t _read_size_this_time)
{
    self->read_size_this_time = _read_size_this_time;
    return OK;
}

status_t socketreaderwriter_set_write_size_this_time(struct socket_reader_writer *self,fsize_t _write_size_this_time)
{
    self->write_size_this_time = _write_size_this_time;
    return OK;
}

status_t socketreaderwriter_set_write_timer(struct socket_reader_writer *self,uint32_t _write_timer)
{
    self->write_timer = _write_timer;
    return OK;
}

status_t socketreaderwriter_set_read_timer(struct socket_reader_writer *self,uint32_t _read_timer)
{
    self->read_timer = _read_timer;
    return OK;
}

status_t socketreaderwriter_set_timeout(struct socket_reader_writer *self,uint32_t _timeout)
{
    self->timeout = _timeout;
    return OK;
}

status_t socketreaderwriter_set_callback(struct socket_reader_writer *self,struct closure *_callback)
{
    ASSERT(_callback);
    return closure_copy(&self->callback,_callback);
}

/***************************************/
status_t socketreaderwriter_prepare_file_for_write(struct socket_reader_writer *self,struct file_base *ifile, fsize_t start, fsize_t size)
{
    self->write_offset = start;
    self->i_write_src_file = ifile;    	
    self->write_end_offset = size-start;
    self->write_timer = 0;
    return OK;	
}

status_t socketreaderwriter_error(struct socket_reader_writer *self,int err)
{
    const char *err_str="unknown";
	
    if(err == C_SOCKET_READER_WRITER_ERROR_CONNECT_ERROR)		
        err_str="connect error";
	
    if(err == C_SOCKET_READER_WRITER_ERROR_READ_TIMEOUT)	
        err_str="read timeout";
	
    if(err == C_SOCKET_READER_WRITER_ERROR_CONNECTION_CLOSED)	
        err_str="connection closed";
	
    if(err == C_SOCKET_READER_WRITER_ERROR_WRITE_TIMEOUT)	
        err_str="write timeout";
	
    if(err == C_SOCKET_READER_WRITER_ERROR_WRITE_ERROR)	
        err_str="write error";
	
    if(err == C_SOCKET_READER_WRITER_ERROR_READ_ERROR)	
        err_str="read error";
	
    if(err == C_SOCKET_READER_WRITER_ERROR_REACH_END_OF_FILE)
        err_str="reach end of file";
	
    if(err == C_SOCKET_READER_WRITER_ERROR_LINE_IS_TOO_LONG)	
        err_str="line is too long";
	
    closure_set_param_int(&self->callback,1,err);
    closure_set_param_pointer(&self->callback,2,(void*)err_str);
    closure_run_event(&self->callback,C_SOCKET_READER_WRITER_EVENT_ERROR);	
    return OK;
}

status_t socketreaderwriter_do_write(struct socket_reader_writer *self,uint32_t interval)
{   
    char buf[64*1024];
	int_ptr_t need_write,rs,ws;

    self->write_size_this_time = 0;
    if(self->write_end_offset <= 0 || self->i_write_src_file == NULL)
    {
        return OK;
    }
    ASSERT(self->i_socket);

    if(!socket_is_connected(self->i_socket))
    {
        socketreaderwriter_error(self,C_SOCKET_READER_WRITER_ERROR_CONNECTION_CLOSED);
        return ERROR;
    }
   
    if(self->write_offset >= self->write_end_offset)
    {
        return OK;
    }

    need_write = (int_ptr_t)(self->write_end_offset - self->write_offset);  
    if(need_write > (int_ptr_t)sizeof(buf))need_write = sizeof(buf);
    filebase_seek(self->i_write_src_file,self->write_offset);


    if(filebase_is_end(self->i_write_src_file))
    {
        socketreaderwriter_error(self,C_SOCKET_READER_WRITER_ERROR_REACH_END_OF_FILE);
        return ERROR;
    }

    rs = filebase_read(self->i_write_src_file,buf,need_write);
    ws = socket_write(self->i_socket,buf,rs);
    self->write_size_this_time = ws;

    if(ws > 0)
    {
        self->write_offset += ws;
        self->write_timer = 0;
        if(self->write_offset >= self->write_end_offset)
        {
            return OK;
        }
		ASSERT(self->i_task_mgr);
		taskmgr_turbo_on(self->i_task_mgr);
    }

    else if(ws == 0)
    {
        self->write_offset += ws;
        if(interval < self->timeout)
            self->write_timer += interval;
        if(self->write_timer >= self->timeout)
        {
            socket_close_connect(self->i_socket);
            socketreaderwriter_error(self,C_SOCKET_READER_WRITER_ERROR_WRITE_TIMEOUT);
            return ERROR;
        }
    }
    else
    {
        socket_close_connect(self->i_socket);
        socketreaderwriter_error(self,C_SOCKET_READER_WRITER_ERROR_WRITE_ERROR);
        return ERROR;
    }
    return ERROR;
}

status_t socketreaderwriter_prepare_file_for_read(struct socket_reader_writer *self,struct file_base *ifile, fsize_t size, bool_t reset)
{
    if(ifile&&reset)
		filebase_set_size(ifile,0);
    self->i_read_dest_file = ifile;
	self->read_timer = 0;
	self->total_read_length = size;
	self->read_length = 0;
	return OK;
}

status_t socketreaderwriter_do_read(struct socket_reader_writer *self,uint32_t interval)
{
    char buf[64*1024];
    int_ptr_t need_read,rs;

    self->read_size_this_time = 0;
    if(self->total_read_length == 0 || self->i_read_dest_file == NULL)
    {
        return OK;
    }

    ASSERT(self->i_socket);
    if(!socket_is_connected(self->i_socket))
    {
        socketreaderwriter_error(self,C_SOCKET_READER_WRITER_ERROR_CONNECTION_CLOSED);
        return ERROR;
    }

    if(self->total_read_length>0 && self->read_length >= self->total_read_length)
    {
        return OK;
    }

    if(self->total_read_length > 0)
    {
        need_read = (int_ptr_t)(self->total_read_length - self->read_length);
        if(need_read > (int_ptr_t)sizeof(buf))need_read = sizeof(buf);  
    }
    else
    {
        need_read = sizeof(buf);
    }

    rs = socket_read(self->i_socket,buf,need_read);
    self->read_size_this_time = rs;

    if(rs > 0)
    {
        self->read_timer = 0;
        self->read_length += rs;
        filebase_write(self->i_read_dest_file,buf,rs);
        if(self->total_read_length>0 && self->read_length>=self->total_read_length)
        {
            return OK;
        }
    }
    else if(rs == 0)
    {
        if(interval < self->timeout)
            self->read_timer += interval;
        if(self->read_timer >= self->timeout)
        {
            socket_close_connect(self->i_socket);
            socketreaderwriter_error(self,C_SOCKET_READER_WRITER_ERROR_READ_TIMEOUT);
            return ERROR;
        }
    }
    else
    {
        socket_close_connect(self->i_socket);
        socketreaderwriter_error(self,C_SOCKET_READER_WRITER_ERROR_READ_ERROR);     
        return ERROR;
    }
    return ERROR;
}


status_t socketreaderwriter_do_read_until_eol(struct socket_reader_writer *self,uint32_t interval,bool_t only_lf)
{
	char buf[8],last[2];
	int_ptr_t rs;
	fsize_t size,offset;

    self->read_size_this_time = 0;
    ASSERT(self->i_read_dest_file);
    if(!socket_is_connected(self->i_socket))
    {
        socketreaderwriter_error(self,C_SOCKET_READER_WRITER_ERROR_CONNECTION_CLOSED);
        return ERROR;
    }

    while(TRUE)
    {
        rs = socket_read(self->i_socket,buf,1);
        if(rs == 1)
        {
            self->read_size_this_time += rs;
            self->read_timer = 0;
            filebase_putc(self->i_read_dest_file,buf[0]);
            size = filebase_get_size(self->i_read_dest_file);           
            if(size > MAX_LINE_LENGTH)
            {
                socketreaderwriter_error(self,C_SOCKET_READER_WRITER_ERROR_LINE_IS_TOO_LONG);
                return ERROR;
            }

            if(only_lf)
            {               
                if(buf[0] == '\n')
                {
                    return OK;
                }
            }

            if(size >= 2 && (!only_lf))
            {
                offset = filebase_get_offset(self->i_read_dest_file);
                filebase_seek(self->i_read_dest_file,size-2);
                filebase_read(self->i_read_dest_file,last,2);
                filebase_seek(self->i_read_dest_file,offset);
                if(last[0]=='\r'&&last[1]=='\n')
                {
                    return OK;
                }
            }
        }

        else if(rs == 0)
        {
            if(interval < self->timeout)
                self->read_timer += interval;
            if(self->read_timer > self->timeout)
            {
                socketreaderwriter_error(self,C_SOCKET_READER_WRITER_ERROR_READ_TIMEOUT);
            }
            return ERROR;
        }
        else
        {
            socket_close_connect(self->i_socket);
            socketreaderwriter_error(self,C_SOCKET_READER_WRITER_ERROR_READ_ERROR);
            return ERROR;
        }
    }
    return ERROR;
}

status_t socketreaderwriter_do_read_until_empty_line(struct socket_reader_writer *self,uint32_t interval)
{
	char buf[8],last[4];
	int_ptr_t rs;
	fsize_t size,offset;

    self->read_size_this_time = 0;
    ASSERT(self->i_read_dest_file);
    if(!socket_is_connected(self->i_socket))
    {
        socketreaderwriter_error(self,C_SOCKET_READER_WRITER_ERROR_CONNECTION_CLOSED);
        return ERROR;
    }
 
    while(TRUE)
    {
        rs = socket_read(self->i_socket,buf,1);
        if(rs == 1)
        {
            self->read_size_this_time += rs;
            self->read_timer = 0;
            filebase_putc(self->i_read_dest_file,buf[0]);
            size = filebase_get_size(self->i_read_dest_file);
            if(size > MAX_LINE_LENGTH)
            {
                socketreaderwriter_error(self,C_SOCKET_READER_WRITER_ERROR_LINE_IS_TOO_LONG);
                return ERROR;
            }

            if(size >= 4)
            {
                offset = filebase_get_offset(self->i_read_dest_file);
                filebase_seek(self->i_read_dest_file,size-4);
                filebase_read(self->i_read_dest_file,last,4);
                filebase_seek(self->i_read_dest_file,offset);
                if(last[0]=='\r'&&last[1]=='\n'&&last[2]=='\r'&&last[3]=='\n')
                {
                    return OK;
                }
            }
        }
        else if(rs == 0)
        {
            if(interval < self->timeout)
                self->read_timer += interval;
            if(self->read_timer > self->timeout)
            {
                socketreaderwriter_error(self,C_SOCKET_READER_WRITER_ERROR_READ_TIMEOUT);
            }
            return ERROR;
        }
        else
        {
            socket_close_connect(self->i_socket);
            socketreaderwriter_error(self,C_SOCKET_READER_WRITER_ERROR_READ_ERROR);
            return ERROR;
        }
    }

    return ERROR;

}

bool_t socketreaderwriter_is_connected(struct socket_reader_writer *self)
{
	if(self->i_socket == NULL)
		return FALSE;
	return socket_is_connected(self->i_socket);
}


