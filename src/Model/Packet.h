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


#endif //MINIVPN_PACKET_H
