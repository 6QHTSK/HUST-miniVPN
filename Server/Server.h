//
// Created by csepsk on 2022/5/13.
//

#ifndef MINIVPN_SERVER_H
#define MINIVPN_SERVER_H

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ifaddrs.h>
#include "../Epoll/Epoll.h"
#include "../Tun/Tun.h"
#include "SockServer.h"

class Subnets{
public:
    in_addr IP;
    in_addr netmask;
    struct Subnets* next;
    ~Subnets();
};

class Server {
private:
    const int buff_size = 2000;
    Tun* tun;
    SockServer* sock;
    Subnets* subnets;

    void tunSelected();
    void socketSelected();

public:
    ~Server();
    explicit Server(int port_number);
    [[noreturn]] void listen();
};


#endif //MINIVPN_SERVER_H
