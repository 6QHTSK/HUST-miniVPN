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
#include <shadow.h>
#include <crypt.h>

class VirAddrPool{
private:
    in_addr net{};
    const in_addr mask = {.s_addr=0x00FFFFFF};  // 255.255.255.0
    const uint8_t poolOffset = 2; // start From .2
    const uint8_t maxPoolSize = 252;
    struct viraddr{
        in_addr peer;
        bool enable;
    }*viraddr;
    uint8_t poolSize = 0;
    in_addr generateViraddr(uint8_t poolIndex) const;
public:
    VirAddrPool();
    ~VirAddrPool();
    void init(in_addr net_in);
    in_addr AllocVirAddr(in_addr peer);
    void FreeVirAddr(in_addr peer);
    in_addr GetGateWay() const;
    in_addr GetNetmask();
};

class IfAddrInfo{
private:
    uint8_t intranetCnt = 0;
    Subnet intranet[256]{};
    uint8_t  extraIPCnt = 0;
    in_addr extraIP[256]{};
    static bool isIntranet(in_addr subnet);
public:
    void init();
    uint8_t getIntranet(Subnet* dest);
    void printExtraIP(uint16_t portNum);
};

class Server {
private:
    const int buff_size = 2000;
    Tun* tun{};
    SockServer* sock{};
    IfAddrInfo ifInfo;
    Epoll epoll;
    VirAddrPool virAddrPool;
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
