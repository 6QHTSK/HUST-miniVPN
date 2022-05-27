//
// Created by csepsk on 2022/5/17.
//

#ifndef MINIVPN_PACKET_H
#define MINIVPN_PACKET_H

#include <arpa/inet.h>

typedef struct subnet{
    in_addr IP;
    in_addr netmask;
}Subnet;

typedef struct vpnInitPacket{
    in_addr virtualIP;
    in_addr mask;
    uint32_t routeCnt;
    Subnet routes[256];
}VpnInitPacket;

typedef struct verifyPacket{
    uint8_t usernameLen;
    __attribute__((unused)) uint8_t passwordLen;
    char verifyInfo[128];   // max length 128
    const char* username() const{
        return verifyInfo;
    }
    const char* password() const{
        return verifyInfo + usernameLen;
    }
}VerifyPacket;

typedef struct ipHeader{
    __attribute__((unused)) uint8_t frontPadding[12];
    uint32_t srcIP;
    uint32_t destIP;
} IPHeader;

#endif //MINIVPN_PACKET_H
