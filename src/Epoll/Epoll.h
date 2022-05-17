//
// Created by csepsk on 2022/5/15.
//

#ifndef MINIVPN_EPOLL_H
#define MINIVPN_EPOLL_H

#include <sys/epoll.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>
class Epoll {
private:
    int epollFd;
    static const int maxEpollEvent = 1024;
public:
    struct epoll_event events[maxEpollEvent]{}; // epoll的event data中存放fd
    explicit Epoll(int size=5);
    void add(int eventFd) const;
    int wait();
};


#endif //MINIVPN_EPOLL_H
