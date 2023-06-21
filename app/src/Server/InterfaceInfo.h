//
// Created by csepsk on 2022/5/27.
//

#ifndef MINIVPN_INTERFACEINFO_H
#define MINIVPN_INTERFACEINFO_H

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "../Model/Packet.h"

class InterfaceInfo{
private:
    uint8_t intranetCnt = 0;
    Subnet intranet[256]{};
    uint8_t  extraIPCnt = 0;
    in_addr extraIP[256]{};
    static bool isIntranet(in_addr subnet);
public:
    void Init();
    uint8_t GetIntranet(Subnet* dest);
    void PrintExtraIP(uint16_t portNum);
};

#endif //MINIVPN_INTERFACEINFO_H
