#include "c_tcp_server.h"
#include "syslog.h"
#include "mem_tool.h"

status_t tcpserver_init_basic(struct tcp_server *self)
{
    socket_init_basic(&self->base_socket);
    crt_memset((void*)&self->sad,0,sizeof(struct  sockaddr_in));
    crt_memset((void*)&self->cad,0,sizeof(struct  sockaddr_in));
    self->max_connect = 100;
    return OK;
}

status_t tcpserver_init(struct tcp_server *self)
{
    tcpserver_init_basic(self);
    socket_init(&self->base_socket);
    return OK;
}

status_t tcpserver_destroy(struct tcp_server *self)
{
    socket_destroy(&self->base_socket);
    tcpserver_init_basic(self);
    return OK;
}

status_t tcpserver_init_server(struct tcp_server *self)
{
    crt_memset((char *)&self->sad,0,sizeof(self->sad));  /* clear sockaddr structure   */
    self->sad.sin_family = AF_INET;            /* set family to internet     */
    self->sad.sin_addr.s_addr = INADDR_ANY;    /* set the local IP address   */
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

status_t tcpserver_set_port(struct tcp_server *self,int32_t port)
{
    self->sad.sin_port = crt_htons((u_short)port);
    if (crt_bind(self->base_socket.socket_num, (struct sockaddr *)&self->sad, sizeof(self->sad)) < 0)
    {
        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
            "bind failed on port %d!\r\n",port
        );
        return ERROR;
    }

    if (crt_listen(self->base_socket.socket_num, self->max_connect) < 0)
    {
        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
            "listen failed!\r\n"
        );
        return ERROR;
    }
    return OK;
}
int32_t tcpserver_accept(struct tcp_server *self)
{
    int32_t alen = sizeof(self->cad);
    int32_t snum;
    if ((snum = crt_accept(self->base_socket.socket_num, 
        (struct sockaddr *)&self->cad, &alen)) < 0)
    {
        snum = -1;
    }
    return snum;
}

status_t tcpserver_get_client_ip_and_port(struct tcp_server *self,struct file_base *ip, int32_t *port)
{
    char *psz_addr;
    ASSERT(ip && port);
    psz_addr = crt_inet_ntoa(self->cad.sin_addr);
    ASSERT(psz_addr);    
    filebase_set_size(ip,0);
    filebase_puts(ip,psz_addr);
    *port = self->cad.sin_port;
    return OK;
}
