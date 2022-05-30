//
// Created by csepsk on 2022/5/13.
//

#ifndef MINIVPN_CLIENT_H
#define MINIVPN_CLIENT_H

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <stdexcept>
#include "Tun/Tun.h"
#include "Epoll/Epoll.h"
#include "Client/SSLClient.h"
#include "Model/Packet.h"

class Client {
private:
    const int buff_size = 2000;
    Tun* tun = nullptr;
    SSLClient* sock = nullptr;

    void sendPackage();
    void recvPackage();

public:
    ~Client();
    void Init(const char *peerAddr, int port_number);
    bool Verify(const char *username, const char *password);
    [[noreturn]] void Listen();
};


#endif //MINIVPN_CLIENT_H
