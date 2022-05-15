//
// Created by csepsk on 2022/5/15.
//

#ifndef MINIVPN_IOCTLIFSOCK_H
#define MINIVPN_IOCTLIFSOCK_H

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
class IoctlIfSock {
private:
    int sockfd;
    struct ifreq ifr;
public:
    explicit IoctlIfSock(struct ifreq ifr);
    ~IoctlIfSock();
    char *getIfName();
    void adapterAssignIp(const char* runIP);
    void adapterAssignNetmask(const char* netmask);
    void adapterActivate();
    void adapterAddRouteTo(const char *IP, const char *mask);
};


#endif //MINIVPN_IOCTLIFSOCK_H
