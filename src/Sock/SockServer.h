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

typedef enum connection_status{NO_CONN,TCP_ESTAB,SSL_ESTAB,VPN_ESTAB} connection_status;

class SockConnection : public SockBase{
public:
    SockConnection();

    struct sockaddr_in peerAddr;
    struct in_addr virtualAddr;
    connection_status status;
    explicit SockConnection(int listenfd);
    void init(int listenfd);
};

class SockServer : public SockBase {
public:
    void init(int port_number);
    SockConnection * sockAccept() const;
};


#endif //MINIVPN_SOCKSERVER_H
