//
// Created by csepsk on 2022/5/15.
//

#ifndef MINIVPN_SOCKSERVER_H
#define MINIVPN_SOCKSERVER_H

#include <cerrno>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <map>
#include "SockBase.h"
#include <sys/epoll.h>
#include <poll.h>

typedef enum connection_status{TCP_ACCEPT,SSL_ESTAB,VPN_ESTAB} connection_status;

class SockConnection : public SockBase{
private:
    SSL_CTX *ctx;
    bool initSSL();
public:
    explicit SockConnection(SSL_CTX *ctx_in);
    struct sockaddr_in peerAddr;
    struct in_addr virtualAddr;
    connection_status status;
    void init(int listenfd);
};

class SockServer : public SockBase {
private:
    SSL_CTX *ctx;
public:
    ~SockServer();
    void init(int port_number);
    SockConnection * sockAccept() const;
};


#endif //MINIVPN_SOCKSERVER_H
