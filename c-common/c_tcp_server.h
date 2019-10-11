#ifndef _C_TCP_SERVER_H
#define _C_TCP_SERVER_H

#include "cruntime.h"
#include "c_log_buffer.h"
#include "c_socket.h"
#include "c_file_base.h"

struct tcp_server{
    struct socket base_socket;
    struct  sockaddr_in sad;      /* structure to hold server's address     */   
    struct  sockaddr_in cad;      /* structure to hold client's address     */    
    int32_t max_connect;          /* max connection */
};

status_t tcpserver_init_basic(struct tcp_server *self);
status_t tcpserver_init(struct tcp_server *self);
status_t tcpserver_destroy(struct tcp_server *self);
status_t tcpserver_init_server(struct tcp_server *self);
status_t tcpserver_set_port(struct tcp_server *self,int32_t port);
int32_t tcpserver_accept(struct tcp_server *self);
status_t tcpserver_get_client_ip_and_port(struct tcp_server *self,struct file_base *ip, int32_t *port);

#endif
