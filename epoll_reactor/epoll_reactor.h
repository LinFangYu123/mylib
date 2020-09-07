#ifndef EPOLL_REACTOR_H
#define EPOLL_REACTOR_H
#include <stdio.h>
#include <sys/epoll.h>
#include "Socket.h"

struct my_event{
	int fd;
	uint32_t events;
	void (*callback)(void *arg);
	void *arg;
	int status;
	void *args;
};

int eventset(struct my_event *ev,int fd,void callback(void *arg),void *arg);

int eventadd(int epfd,int events,struct my_event *ev);

int eventmod(int epfd,int events,struct my_event *ev);

int eventdel(int epfd,struct my_event *ev);

#endif