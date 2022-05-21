//
// Created by csepsk on 2022/5/15.
//

#ifndef MINIVPN_EPOLL_H
#define MINIVPN_EPOLL_H

#include <sys/epoll.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <map>

class Epoll {
private:
    int epollFd;
    static const int maxEpollEvent = 1024;
    std::map<int,uint32_t >epollListen;
public:
    struct epoll_event events[maxEpollEvent]{}; // epoll的event data中存放fd
    explicit Epoll(int size=5);
    void Add(int eventFd, int listenEvent);
    int Wait();
    void Modify(int eventFd, int enableEvent, int disableEvent);
};


#endif //MINIVPN_EPOLL_H
