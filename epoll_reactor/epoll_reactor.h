#ifndef EPOLL_REACTOR_H
#define EPOLL_REACTOR_H

#include "base.h"

#define EPOLL_ON    0x0001      // 在epoll上

typedef struct my_event
{
    int32_t fd;
    uint32_t events;
    void (*callback)(void *arg);
    void *arg;
    void *args;
    uint32_t epoll_flag;    // 标记该event节点是否在epoll上，0x0000=不在，0x0001=在
} my_event_s;

int32_t eventset(my_event_s *ev, int32_t fd, void (*callback)(void *arg), void *arg, void *args);

int32_t eventadd(int epfd, int events, my_event_s *ev);

int32_t eventmod(int epfd, int events, my_event_s *ev);

int32_t eventdel(int epfd, my_event_s *ev);

#endif