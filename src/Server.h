//
// Created by csepsk on 2022/5/13.
//

#ifndef MINIVPN_SERVER_H
#define MINIVPN_SERVER_H

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ifaddrs.h>
#include "Epoll/Epoll.h"
#include "Tun/Tun.h"
#include "Sock/SockServer.h"
#include "Model/Packet.h"

class Server {
private:
    const int buff_size = 2000;
    Tun* tun{};
    SockServer* sock{};
    Subnet subnets[256]{};
    uint8_t subnetCnt{};
    Epoll epoll;
    std::map<int,SockConnection*> connPoolFd;          // sockfd -> connSocket
    std::map<uint32_t ,SockConnection*> connPoolVirAddr;  // virtual addr -> connSocket

    void tunSelected();
    void listenSocketSelected();
    void tcpEstabSelected(SockConnection *pConnection);
    void vpnEstabSelected(SockConnection *conn);

public:
    ~Server();
    void init(int port_number);
    [[noreturn]] void listen();
};


#endif //MINIVPN_SERVER_H
