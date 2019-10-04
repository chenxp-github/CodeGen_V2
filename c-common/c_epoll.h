#ifndef _C_EPOLL_H
#define _C_EPOLL_H

#include "cruntime.h"
#include "c_log_buffer.h"

#if HAVE_WINDOWS_H
#else
#include <sys/epoll.h>
#endif

#define C_WAIT_OPT_AUTO_REMOVE  0x01
#define C_WAIT_OPT_OPEN_LOG  0x02

struct epoll{
#if HAVE_WINDOWS_H
    fd_set m_fd_set;
#else
    int m_MaxEventsLen;
    int m_CurEventsLen;
    struct epoll_event *m_Events;
    int m_EpollHandle;
#endif
};

status_t epoll_init_basic(struct epoll *self);
status_t epoll_init(struct epoll *self, int max);
status_t epoll_destroy(struct epoll *self);
status_t epoll_copy(struct epoll *self,struct epoll *_p);
status_t epoll_comp(struct epoll *self,struct epoll *_p);
status_t epoll_print(struct epoll *self,struct log_buffer *_buf);
status_t epoll_add_fd(struct epoll *self,int fd);
status_t epoll_del_fd(struct epoll *self,int fd);
status_t epoll_auto_remove_hungup_fds(struct epoll *self,uint32_t opt);
int epoll_sleep_with_opt(struct epoll *self,int ms, uint32_t opt);
int epoll_sleep(struct epoll *self,int ms);


#endif
