//
// Created by csepsk on 2022/5/13.
//

#ifndef MINIVPN_SERVER_H
#define MINIVPN_SERVER_H

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ifaddrs.h>
#include <shadow.h>
#include <crypt.h>
#include <thread>
#include <stdexcept>
#include "Epoll/Epoll.h"
#include "Tun/Tun.h"
#include "Server/SSLServer.h"
#include "Model/Packet.h"
#include "Server/VirtualAddress.h"
#include "Server/InterfaceInfo.h"

class Server {
private:
    const int buff_size = 2000;
    Tun* tun{};
    SSLServer* sock{};
    InterfaceInfo ifInfo;
    Epoll epoll;
    VirtualAddress virAddrPool;
    std::map<int,SSLConnection*> connPoolFd;          // sockfd -> connSocket
    std::map<uint32_t ,SSLConnection*> connPoolVirAddr;  // virtual addr -> connSocket

    void sendPackage();
    void acceptNewConnection();
    void connectionEstab(SSLConnection *conn);
    void recvPackage(SSLConnection *conn);

public:
    ~Server();
    void init(int port_number);
    [[noreturn]] void listen();

    void deleteConn(SSLConnection *conn);
};


#endif //MINIVPN_SERVER_H
