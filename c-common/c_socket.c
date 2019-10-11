#include "c_socket.h"
#include "misc.h"
#include "syslog.h"
#include "mem_tool.h"
#include "c_mem.h"

status_t socket_init_basic(struct socket *self)
{
    self->socket_num = -1;
    return OK;
}

status_t socket_init(struct socket *self)
{
    socket_init_basic(self);
    return OK;
}

status_t socket_destroy(struct socket *self)
{
    socket_close_connect(self);
    socket_init_basic(self);
    return OK;
}

int socket_get_socket_num(struct socket *self)
{
    return self->socket_num;
}

status_t socket_set_socket_num(struct socket *self,int _socket_num)
{
    self->socket_num = _socket_num;
    return OK;
}

status_t socket_close_connect(struct socket *self)
{
    if(self->socket_num >= 0)
    {
        crt_closesocket(self->socket_num);
        self->socket_num = -1;
    }
    return OK;
}

int_ptr_t socket_write(struct socket *self,const void *buf, int_ptr_t n)
{
    int_ptr_t ret;    

    ASSERT(self->socket_num > 0);    
    if(n <= 0)return 0;

    ret = crt_send(self->socket_num,(const char*)buf,n,0);
    
    if(ret ==SOCKET_ERROR)
    {
        if(crt_is_socket_broken())
            socket_close_connect(self);
        return 0;
    }
    else if(ret == 0)
    {
        socket_close_connect(self);
    }
    return ret;
}

int_ptr_t socket_read(struct socket *self,void *buf, int_ptr_t n)
{
    int_ptr_t ret;
    ASSERT(self->socket_num > 0);
    if(n <= 0) return 0;
    
    ret = crt_recv(self->socket_num,(char*)buf,n,0);
    if(ret==SOCKET_ERROR)
    {
        if(crt_is_socket_broken())
            socket_close_connect(self);
        return 0;
    }
    else if(ret == 0)
    {
        socket_close_connect(self);
    }
    return ret;
}

status_t socket_set_blocking(struct socket *self,bool_t enable)
{
    return crt_set_blocking_mode(self->socket_num,enable);
}

bool_t socket_is_connected(struct socket *self)
{
    return self->socket_num > 0;
}

status_t socket_transfer_socket_fd(struct socket *self, struct socket *from)
{
    ASSERT(from);
    self->socket_num = from->socket_num;
    from->socket_num = -1;
    return OK;
}

bool_t socket_is_ip_address(struct mem *str)
{
    _C_LOCAL_MEM_HEADER(mem);
    struct file_base *str_file = &str->base_file_base;
    int c = 0;

    ASSERT(str);
    _C_LOCAL_MEM_BODY(mem);
           
    filebase_set_split_chars(str_file,".");
    filebase_seek(str_file,0);
    
    while(filebase_read_string(str_file,mem_file))
    {
        if(!is_dec(mem_cstr(&mem)))
            break;
        c++;
    }
    return c == 4;
}
