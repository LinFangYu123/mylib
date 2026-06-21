#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>

#include "epoll_reactor.h"
#include "Socket.h"

// 设置my_event_s
int32_t eventset(my_event_s *ev, int32_t fd, void (*callback)(void *arg), void *arg, void *args)
{
    ev->fd = fd;
    ev->events = 0;
    ev->callback = callback;
    ev->arg = arg;
    ev->args = args;
    ev->epoll_flag = 0;     // 初始状态：不在epoll上

    return 0;
}

// 将事件添加到树上
int32_t eventadd(int32_t epfd, int32_t events, my_event_s *ev)
{
    struct epoll_event epv;

    memset(&epv, 0, sizeof(epv));
    epv.data.ptr = ev;
    ev->events = events;
    epv.events = events;

    if (!TEST_BITS(ev->epoll_flag, EPOLL_ON))       // 当epoll_flag为EPOLL_ON时 代表文件描述符已经在树上 eventadd失败
    {
        // 将epoll_flag置为EPOLL_ON
        SET_BITS(ev->epoll_flag, EPOLL_ON);
    }
    else
    {
        return -1;
    }

    if (0 != epoll_ctl(epfd, EPOLL_CTL_ADD, ev->fd, &epv))
    {
        perror("epoll_ctl");
        CLEAR_BITS(ev->epoll_flag, EPOLL_ON);
        return -1;
    }

    return 0;
}

// 修改树上的事件
int32_t eventmod(int32_t epfd, int32_t events, my_event_s *ev)
{
    struct epoll_event epv;

    memset(&epv, 0, sizeof(epv));
    epv.data.ptr = ev;
    epv.events = ev->events = events;

    if (!TEST_BITS(ev->epoll_flag, EPOLL_ON))       // 当epoll_flag为EPOLL_OFF时 代表文件描述符不在树上 eventmod失败
    {
        return -1;
    }

    if (0 != epoll_ctl(epfd, EPOLL_CTL_MOD, ev->fd, &epv))
    {
        perror("epoll_ctl");
        return -1;
    }

    return 0;
}

// 删除树上的事件
int32_t eventdel(int32_t epfd, my_event_s *ev)
{
    if (!TEST_BITS(ev->epoll_flag, EPOLL_ON))
    {
        return -1;
    }

    epoll_ctl(epfd, EPOLL_CTL_DEL, ev->fd, NULL);
    CLEAR_BITS(ev->epoll_flag, EPOLL_ON);

    return 0;
}
