#include "epoll_tree.h"

//设置struct my_event
int eventset(struct my_event *ev,int fd,void (*callback)(void *arg),void *arg,void *args){
	ev->fd = fd;
	ev->events = 0;
	ev->callback = callback;
	ev->arg = arg;
	ev->args = args;
}

//将事件添加到树上
int eventadd(int epfd,int events,struct my_event *ev){
	struct epoll_event epv;
	bzero(&epv,sizeof(epv));
	epv.data.ptr = ev;
	epv.events = ev->events = events;
	if(ev->status == 0)       //当status为1时 代表文件描述符已经在树上 eventadd失败
    {
        //将status置1
        ev->status = 1;
    }
    else return -1;
	if(epoll_ctl(epfd,EPOLL_CTL_ADD,ev->fd,&epv)!=0){
		perror("epoll_ctl");
		return -1;
	}
	return 0;
}

//修改树上的事件
int eventmod(int epfd,int events,struct my_event *ev){
	struct epoll_event epv;
	bzero(&epv,sizeof(epv));
	epv.data.ptr = ev;
	epv.events = ev->events = events;
	if(ev->status == 0)       //当status为0时 代表文件描述符不在树上 eventmod失败
    {
        return -1;
    }
	if(epoll_ctl(epfd,EPOLL_CTL_MOD,ev->fd,&epv)!=0){
		perror("epoll_ctl");
		return -1;
	}
	return 0;
}

//删除树上的事件
int eventdel(int epfd,struct my_event *ev){
	if(ev->status == 0){
		return -1;
	}
	ev->status = 0;
	ev = NULL;
	epoll_ctl(epfd,EPOLL_CTL_DEL,ev->fd,NULL);
	return 0;
}
