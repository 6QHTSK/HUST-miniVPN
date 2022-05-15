//
// Created by csepsk on 2022/5/15.
//

#ifndef MINIVPN_SOCKSERVER_H
#define MINIVPN_SOCKSERVER_H

#include <cerrno>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>

class SockServer {
    private:
        int sockfd;
        struct sockaddr_in peerAddr;
    public:
        int fd() const;
        explicit SockServer(int port_number);
        ssize_t send(const char* buff,size_t len);
        ssize_t recv(char* buff,size_t size) const;
};


#endif //MINIVPN_SOCKSERVER_H
