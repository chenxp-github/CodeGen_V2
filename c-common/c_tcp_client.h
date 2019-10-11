#ifndef _C_TCP_CLIENT_H
#define _C_TCP_CLIENT_H

#include "cruntime.h"
#include "c_log_buffer.h"
#include "c_socket.h"

struct tcp_client{
    struct socket base_socket;
    struct  hostent  *ptrh;   /* pointer to a host table entry       */ 
    struct  protoent *ptrp;   /* pointer to a protocol table entry   */ 
    struct  sockaddr_in sad;  /* structure to hold an IP address     */
    int32_t port;             /* protocol port number       */         
};

status_t tcpclient_init_basic(struct tcp_client *self);
status_t tcpclient_init(struct tcp_client *self);
status_t tcpclient_destroy(struct tcp_client *self);
status_t tcpclient_set_server_ip(struct tcp_client *self,const char *name);
status_t tcpclient_set_port(struct tcp_client *self,int32_t port);
status_t tcpclient_connect(struct tcp_client *self);
status_t tcpclient_is_connect_complete(struct tcp_client *self);

#endif
