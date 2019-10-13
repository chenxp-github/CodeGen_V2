#ifndef _C_SOCKET_H
#define _C_SOCKET_H

#include "cruntime.h"
#include "c_log_buffer.h"
#include "c_mem.h"

struct socket{
    int socket_num;
};

status_t socket_init_basic(struct socket *self);
status_t socket_init(struct socket *self);
status_t socket_destroy(struct socket *self);
int socket_get_socket_num(struct socket *self);
status_t socket_set_socket_num(struct socket *self,int _socket_num);
status_t socket_close_connect(struct socket *self);
int_ptr_t socket_write(struct socket *self,const void *buf, int_ptr_t n);
int_ptr_t socket_read(struct socket *self,void *buf, int_ptr_t n);
status_t socket_set_blocking(struct socket *self,bool_t enable);
bool_t socket_is_connected(struct socket *self);
status_t socket_transfer_socket_fd(struct socket *self, struct socket *from);
bool_t socket_is_ip_address(const char *str);

#endif
