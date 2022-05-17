//
// Created by csepsk on 2022/5/17.
//

#ifndef MINIVPN_SOCKBASE_H
#define MINIVPN_SOCKBASE_H

#include <cstdio>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cerrno>

class SockBase {
protected:
    int sockfd;
public:
    SockBase();
    void init();
    ~SockBase();
    ssize_t sockSend(const char* buff, size_t len) const;
    ssize_t sockRecv(char* buff, size_t size) const;
    int fd() const;
};


#endif //MINIVPN_SOCKBASE_H
