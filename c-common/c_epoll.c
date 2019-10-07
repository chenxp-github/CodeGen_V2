#include "c_epoll.h"
#include "syslog.h"
#include "mem_tool.h"

status_t epoll_init_basic(struct epoll *self)
{
#if HAVE_WINDOWS_H
    memset(&self->m_fd_set,0,sizeof(self->m_fd_set));
#else
    self->m_Events = NULL;
    self->m_CurEventsLen = 0;
    self->m_MaxEventsLen = 0;
    self->m_EpollHandle = 0;
#endif
    return OK;
}

status_t epoll_init(struct epoll *self, int max)
{
    epoll_init_basic(self);
#if HAVE_WINDOWS_H
#else
    ASSERT(self->m_EpollHandle == 0);
    self->m_EpollHandle = epoll_create(max);
    ASSERT(self->m_EpollHandle > 0);
    X_ZALLOC(self->m_Events,struct epoll_event, max);
    self->m_MaxEventsLen = max;
#endif
    return OK;
}

status_t epoll_destroy(struct epoll *self)
{
#if HAVE_WINDOWS_H
#else
    if(self->m_EpollHandle)
    {
        close(self->m_EpollHandle);
        self->m_EpollHandle = 0;
    }
    X_FREE(self->m_Events);
#endif
    epoll_init_basic(self);
    return OK;
}

status_t epoll_copy(struct epoll *self,struct epoll *_p)
{
    ASSERT(_p);
    if(self == _p)return OK;

    return OK;
}

status_t epoll_comp(struct epoll *self,struct epoll *_p)
{
    ASSERT(_p);
    if(self == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t epoll_print(struct epoll *self,struct log_buffer *_buf)
{
    ASSERT(_buf);
    return OK;
}


status_t epoll_add_fd(struct epoll *self,int fd)
{
#if HAVE_WINDOWS_H
    FD_SET(fd,&self->m_fd_set);
    return OK;
#else
    struct epoll_event ev;
    ev.events= EPOLLIN | EPOLLRDHUP;
    ev.data.fd= fd;
    int ret = epoll_ctl(self->m_EpollHandle,EPOLL_CTL_ADD,fd,&ev);
    return ret == 0;
#endif
}

status_t epoll_del_fd(struct epoll *self,int fd)
{
#if HAVE_WINDOWS_H
    FD_CLR(fd,&self->m_fd_set);
    return ERROR;
#else
    int r = epoll_ctl(self->m_EpollHandle,EPOLL_CTL_DEL,fd,NULL);
    return r == 0;
#endif
}

#if HAVE_WINDOWS_H
static status_t copy_fd_set(fd_set *to, fd_set *from)
{
    uint32_t i;
    to->fd_count = from->fd_count;
    for(i = 0; i < from->fd_count; i++)
    {
        to->fd_array[i] = from->fd_array[i];
    }
    return OK;
}
#endif

status_t epoll_auto_remove_hungup_fds(struct epoll *self,uint32_t opt)
{
#if HAVE_WINDOWS_H
    
#else
    for(int i = 0; i < self->m_CurEventsLen; i++)
    {
        if(self->m_Events[i].events&EPOLLRDHUP)
        {
            if(epoll_del_fd(self,self->m_Events[i].data.fd))
            {
                if(opt & C_WAIT_OPT_OPEN_LOG)
                {
                    XLOG(LOG_MODULE_COMMON,LOG_LEVEL_INFO,
                        "socket %d is delete from epoll",
                        self->m_Events[i].data.fd
                    );
                }
            }
        }
    }
#endif
    return OK;
}

int epoll_sleep_with_opt(struct epoll *self,int ms, uint32_t opt)
{
#if HAVE_WINDOWS_H	
    fd_set read_set,error_set;
    struct timeval timeout; 
    int res,i;

    copy_fd_set(&read_set,&self->m_fd_set);
    copy_fd_set(&error_set,&self->m_fd_set);
        
    timeout.tv_sec = (ms/1000); 
    timeout.tv_usec =(ms%1000)*1000;
    
    res = select(0,&read_set,NULL,&error_set,&timeout);
    if(res < 0)crt_msleep(ms);
    
    if(opt & C_WAIT_OPT_AUTO_REMOVE)
    {        
        for(i = 0; i < (int)error_set.fd_count; i++)
        {
            FD_CLR(error_set.fd_array[i],&self->m_fd_set);
            if(opt & C_WAIT_OPT_OPEN_LOG)
            {
                XLOG(LOG_MODULE_COMMON,LOG_LEVEL_INFO,
                    "socket %d is delete from epoll",
                    error_set.fd_array[i]
                );
            }
        }
    }
    return OK;
#else
    int ret = epoll_wait(self->m_EpollHandle,self->m_Events,self->m_MaxEventsLen,ms);
    self->m_CurEventsLen = ret;
    ASSERT(self->m_CurEventsLen < self->m_MaxEventsLen);
    if(opt & C_WAIT_OPT_AUTO_REMOVE)
    {
        epoll_auto_remove_hungup_fds(self,opt);
    }
    return ret;
#endif
}

int epoll_sleep(struct epoll *self,int ms)
{
    return epoll_sleep_with_opt(self,ms,C_WAIT_OPT_AUTO_REMOVE|C_WAIT_OPT_OPEN_LOG);
}

