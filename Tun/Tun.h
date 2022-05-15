//
// Created by csepsk on 2022/5/14.
//

#ifndef MINIVPN_TUN_H
#define MINIVPN_TUN_H

#include <fcntl.h>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <linux/route.h>
#include <sys/ioctl.h>
#include <cstdlib>
#include "IoctlIfSock.h"

class Tun {
private:
    int tunfd;
    IoctlIfSock* ctlSock;
public:
    Tun();
    ~Tun();
    int fd() const;
    const char* TunIfName();
    void up(const char *runIP, const char *netmask);
    void addRouteTo(const char* destIP, const char* netmask);
    ssize_t send(const char* buff,size_t len) const;
    ssize_t recv(char* buff, size_t size) const;
};


#endif //MINIVPN_TUN_H
