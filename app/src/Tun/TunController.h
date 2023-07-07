//
// Created by csepsk on 2022/5/15.
//

#ifndef MINIVPN_TUNCONTROLLER_H
#define MINIVPN_TUNCONTROLLER_H

#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <linux/route.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <cstdio>

// Control Socket to TUN Device
class TunController {
private:
    int sockfd;
    struct ifreq ifr;
public:
    explicit TunController(struct ifreq ifr);
    ~TunController();
    void init();
    char *getIfName();
    void adapterActivate();
    void adapterAssignIp(in_addr runIP);
    void adapterAssignNetmask(in_addr netmask);
    void adapterAddRouteTo(in_addr IP, in_addr mask);
};


#endif //MINIVPN_TUNCONTROLLER_H
