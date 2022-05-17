//
// Created by csepsk on 2022/5/13.
//

#ifndef MINIVPN_CLIENT_H
#define MINIVPN_CLIENT_H

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "Tun/Tun.h"
#include "Epoll/Epoll.h"
#include "Sock/SockClient.h"
#include "Model/Packet.h"

class Client {
private:
    const int buff_size = 2000;
    Tun* tun;
    SockClient* sock;

    void tunSelected();
    void socketSelected();

public:
    ~Client();
    void init(const char *peerAddr, int port_number);
    bool verify();
    [[noreturn]] void listen();
};


#endif //MINIVPN_CLIENT_H
