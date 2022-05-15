//
// Created by csepsk on 2022/5/15.
//

#include "IoctlIfSock.h"

IoctlIfSock::IoctlIfSock(struct ifreq ifr):ifr(ifr){
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1){
        printf("Create ioctlSock failed! (%d: %s)\n", errno, strerror(errno));
        exit(-1);
    }
}

void IoctlIfSock::adapterAssignIp(const char* runIP) {
    struct sockaddr_in addr{};
    bzero(&addr,sizeof(addr));
    addr.sin_family = AF_INET;
    if(inet_aton(runIP,&addr.sin_addr) < 0){
        printf("Cannot parse runIP");
        exit(-1);
    }

    memcpy(&ifr.ifr_addr,&addr,sizeof(struct sockaddr));
    int ret = ioctl(sockfd,SIOCSIFADDR,&ifr);
    if(ret == -1){
        printf("Assign IP Address failed! (%d: %s)\n", errno, strerror(errno));
        exit(-1);
    }
}

void IoctlIfSock::adapterAssignNetmask(const char* netmask) {
    struct sockaddr_in addr{};
    bzero(&addr,sizeof(addr));
    addr.sin_family = AF_INET;
    if(inet_aton(netmask,&addr.sin_addr) < 0){
        printf("Cannot parse netmask");
        exit(-1);
    }

    memcpy(&ifr.ifr_addr,&addr,sizeof(struct sockaddr));

    int ret = ioctl(sockfd,SIOCSIFNETMASK,&ifr);
    if(ret == -1){
        printf("Assign netmask to TUN interface failed! (%d: %s)\n", errno, strerror(errno));
        exit(-1);
    }
}

void IoctlIfSock::adapterActivate() {
    int ret = ioctl(sockfd,SIOCGIFFLAGS,&ifr);
    if(ret == -1){
        printf("Get Flag from TUN interface failed! (%d: %s)\n", errno, strerror(errno));
        exit(-1);
    }
    ifr.ifr_flags |= IFF_UP;
    ifr.ifr_flags |= IFF_RUNNING;
    ret = ioctl(sockfd,SIOCSIFFLAGS,&ifr);
    if(ret == -1){
        printf("Start TUN interface failed! (%d: %s)\n", errno, strerror(errno));
        exit(-1);
    }
}

void IoctlIfSock::adapterAddRouteTo(const char *IP, const char *mask) {
    struct sockaddr_in s_addr = {};
    struct sockaddr_in s_mask = {};
    struct rtentry rt{};
    auto fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
    {
        printf("addRoute: socket   error\n");
        exit(-1);
    }
    //网关
    memset(&rt, 0, sizeof(struct rtentry));
    memset(&s_addr, 0, sizeof(struct sockaddr_in));
    memset(&s_mask, 0, sizeof(struct sockaddr_in));
    s_addr.sin_family = AF_INET;
    s_mask.sin_family = AF_INET;
    if(inet_aton(IP, &s_addr.sin_addr) < 0){
        printf( "addRoute:  cannot parse netAddr\n" );
        exit(-1);
    }
    if(inet_aton(mask,&s_mask.sin_addr) < 0){
        printf( "addRoute:  cannot parse mask\n" );
        exit(-1);
    }
    memcpy(&rt.rt_dst,&s_addr,sizeof(s_addr));
    memcpy(&rt.rt_genmask,&s_mask,sizeof(s_addr));
    rt.rt_dev = getIfName();
    //rt.rt_flags = RTF_GATEWAY;
    if (ioctl(fd, SIOCADDRT, &rt)<0)
    {
        printf( "ioctl(SIOCADDRT) error in set_route (%d: %s)\n", errno, strerror(errno));
        exit(-1);
    }
}

IoctlIfSock::~IoctlIfSock() {
    close(sockfd);
}

char *IoctlIfSock::getIfName() {
    return ifr.ifr_name;
}
