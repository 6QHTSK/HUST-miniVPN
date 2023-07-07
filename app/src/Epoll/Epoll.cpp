//
// Created by csepsk on 2022/5/15.
//

#include "Epoll.h"

Epoll::Epoll(int size) {
    epollFd = epoll_create(size);
    if(epollFd == -1){
        printf("Create Epoll Failed!");
        exit(-1);
    }
}
void Epoll::Add(int eventFd, int listenEvent){
    struct epoll_event event{};
    event.data.fd = eventFd;
    event.events = listenEvent;
    int r = epoll_ctl(epollFd,EPOLL_CTL_ADD,eventFd,&event);
    if(r != 0){
        printf("epoll_ctl mod failed (%d,%s)", errno, strerror(errno));
    }
    epollListen[eventFd] = event.events;
    // set nonblocking
    int old_option = fcntl( eventFd, F_GETFL );
    int new_option = old_option | O_NONBLOCK;
    fcntl( eventFd, F_SETFL, new_option );
}
int Epoll::Wait(){
    int eventCnt = epoll_wait(epollFd, Events, maxEpollEvent, -1);
    if (eventCnt < 0){
        printf("Epoll Failed!");
        exit(-1);
    }
    return eventCnt;
}
