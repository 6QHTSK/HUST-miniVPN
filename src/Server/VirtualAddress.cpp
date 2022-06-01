//
// Created by csepsk on 2022/5/27.
//

#include "VirtualAddress.h"

in_addr VirtualAddress::generate(uint8_t poolIndex) const{
    uint32_t t = ntohl(net.s_addr);
    t |= poolOffset + poolIndex;
    return in_addr{.s_addr=htonl(t) };
}

VirtualAddress::VirtualAddress() {
    viraddr = new struct viraddr[maxPoolSize];
}

VirtualAddress::~VirtualAddress() {
    delete viraddr;
}

void VirtualAddress::Init(in_addr net_in) {
    net = net_in;
}

in_addr VirtualAddress::Alloc(in_addr peer) {
    for(uint8_t i=0;i<poolSize;i++){
        if(viraddr[i].peer.s_addr == peer.s_addr){
            viraddr[i].enable = true;
            return generate(i);
        }
    }
    if(poolSize < maxPoolSize){
        viraddr[poolSize].peer = peer;
        viraddr[poolSize].enable = true;
        return generate(poolSize++);
    }
    for(uint8_t i=0;i<poolSize;i++){
        if(!viraddr[i].enable){
            viraddr[i].peer = peer;
            viraddr[i].enable = true;
            return generate(i);
        }
    }
    return in_addr{};
}

void VirtualAddress::Free(in_addr peer) {
    for(uint8_t i=0;i<poolSize;i++){
        if(viraddr[i].peer.s_addr == peer.s_addr){
            viraddr[i].enable = false;
        }
    }
}

in_addr VirtualAddress::GetGateWay() const {
    uint32_t t = ntohl(net.s_addr);
    t |= 1;
    return in_addr{.s_addr=htonl(t) };
}

in_addr VirtualAddress::GetNetmask() {
    return mask;
}