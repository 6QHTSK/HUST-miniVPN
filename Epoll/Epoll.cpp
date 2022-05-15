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
void Epoll::add(int eventFd) const{
    struct epoll_event event{};
    event.data.fd = eventFd;
    event.events = EPOLLIN;
    epoll_ctl(epollFd,EPOLL_CTL_ADD,eventFd,&event);
    // set nonblocking
    int old_option = fcntl( eventFd, F_GETFL );
    int new_option = old_option | O_NONBLOCK;
    fcntl( eventFd, F_SETFL, new_option );
}
int Epoll::wait(){
    int eventCnt = epoll_wait(epollFd,events,maxEpollEvent,-1);
    if (eventCnt < 0){
        printf("Epoll Failed!");
        exit(-1);
    }
    return eventCnt;
}