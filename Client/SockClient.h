//
// Created by csepsk on 2022/5/15.
//

#ifndef MINIVPN_SOCKCLIENT_H
#define MINIVPN_SOCKCLIENT_H

#include <cerrno>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>

class SockClient {
private:
    struct sockaddr_in peerAddr{};
    int sockfd;

public:
    int fd() const;
    SockClient(const char *svrip, int port_number);
    ssize_t send(const char* buff,size_t len);
    ssize_t recv(char* buff,size_t size) const;
};


#endif //MINIVPN_SOCKCLIENT_H
