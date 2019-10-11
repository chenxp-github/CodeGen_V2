#include "c_tcp_client.h"
#include "syslog.h"
#include "mem_tool.h"

status_t tcpclient_init_basic(struct tcp_client *self)
{
    socket_init_basic(&self->base_socket);
    crt_memset((char *)&self->sad,0,sizeof(self->sad)); /* clear sockaddr structure */
    self->sad.sin_family = AF_INET;           /* set family to Internet*/
    self->base_socket.socket_num = -1;
    return OK;
}

status_t tcpclient_init(struct tcp_client *self)
{
    tcpclient_init_basic(self);
    socket_init(&self->base_socket);
    return OK;
}

status_t tcpclient_destroy(struct tcp_client *self)
{
    socket_destroy(&self->base_socket);
    tcpclient_init_basic(self);
    return OK;
}

status_t tcpclient_set_server_ip(struct tcp_client *self,const char *name)
{
    struct in_addr addr;
    addr.s_addr = inet_addr(name);
    if(addr.s_addr == INADDR_NONE)
    {
        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
            "invalidate ip %s\n",name
        );
        return ERROR;
    }
    else
    {
        self->sad.sin_addr.s_addr = addr.s_addr;
    }
    return OK;
}

status_t tcpclient_set_port(struct tcp_client *self,int32_t port)
{
    if (port > 0)
    {
        self->sad.sin_port = crt_htons((u_short)port);
    }
    else
    {
        return ERROR;
    }
    
    if(self->base_socket.socket_num >= 0)
    {
        return ERROR;
    }
    
    self->base_socket.socket_num = crt_socket(AF_INET, SOCK_STREAM, 0);
    
    if (self->base_socket.socket_num < 0)
    {
        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
            "crt_socket creation failed!\r\n"
        );
        return ERROR;
    }
    return OK;
}


status_t tcpclient_connect(struct tcp_client *self)
{
    int32_t ret;

    ASSERT(self->base_socket.socket_num >0);
    
    ret = crt_connect(self->base_socket.socket_num, 
        (struct sockaddr *)&self->sad, sizeof(self->sad));

    if(ret < 0)
    {
        return ERROR;
    }

    return OK;
}
status_t tcpclient_is_connect_complete(struct tcp_client *self)
{
    ASSERT(self->base_socket.socket_num > 0);
    return crt_is_connect_complete(self->base_socket.socket_num);
}

