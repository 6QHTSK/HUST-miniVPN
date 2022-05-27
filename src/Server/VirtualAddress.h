//
// Created by csepsk on 2022/5/27.
//

#ifndef MINIVPN_VIRTUALADDRESS_H
#define MINIVPN_VIRTUALADDRESS_H

#include <arpa/inet.h>
#include <unistd.h>

class VirtualAddress{
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
    in_addr generate(uint8_t poolIndex) const;
public:
    VirtualAddress();
    ~VirtualAddress();
    void Init(in_addr net_in);
    in_addr Alloc(in_addr peer);
    void Free(in_addr peer);
    in_addr GetGateWay() const;
    in_addr GetNetmask();
};


#endif //MINIVPN_VIRTUALADDRESS_H
