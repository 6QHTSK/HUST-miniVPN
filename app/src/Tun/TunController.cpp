//
// Created by csepsk on 2022/5/15.
//

#include "TunController.h"

TunController::TunController(struct ifreq ifr): ifr(ifr), sockfd(-1){}

void TunController::init(){
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1){
        printf("Create ioctlSock failed! (%d: %s)\n", errno, strerror(errno));
        exit(-1);
    }
}

void TunController::adapterAssignIp(in_addr runIP){
    struct sockaddr_in addr{};
    bzero(&addr,sizeof(addr));
    addr.sin_family = AF_INET;
    memcpy(&addr.sin_addr,&runIP,sizeof(runIP));

    memcpy(&ifr.ifr_addr,&addr,sizeof(struct sockaddr));
    int ret = ioctl(sockfd,SIOCSIFADDR,&ifr);
    if(ret == -1){
        printf("Assign IP Address failed! (%d: %s)\n", errno, strerror(errno));
        exit(-1);
    }
}

void TunController::adapterAssignNetmask(in_addr netmask) {
    struct sockaddr_in addr{};
    bzero(&addr,sizeof(addr));
    addr.sin_family = AF_INET;
    memcpy(&addr.sin_addr,&netmask,sizeof(netmask));
    memcpy(&ifr.ifr_addr,&addr,sizeof(struct sockaddr));

    int ret = ioctl(sockfd,SIOCSIFNETMASK,&ifr);
    if(ret == -1){
        printf("Assign netmask to TUN interface failed! (%d: %s)\n", errno, strerror(errno));
        exit(-1);
    }
}

void TunController::adapterActivate() {
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

void TunController::adapterAddRouteTo(in_addr IP, in_addr mask){
    struct sockaddr_in s_addr{};
    bzero(&s_addr,sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    memcpy(&s_addr.sin_addr,&IP,sizeof(IP));

    struct sockaddr_in s_mask{};
    bzero(&s_mask,sizeof(s_mask));
    s_mask.sin_family = AF_INET;
    memcpy(&s_mask.sin_addr,&mask,sizeof(mask));

    struct rtentry rt{};
    memset(&rt, 0, sizeof(struct rtentry));
    memcpy(&rt.rt_dst,&s_addr,sizeof(s_addr));
    memcpy(&rt.rt_genmask,&s_mask,sizeof(s_addr));
    rt.rt_dev = getIfName();
    //rt.rt_flags = RTF_GATEWAY;
    if (ioctl(sockfd, SIOCADDRT, &rt)<0)
    {
        printf( "ioctl(SIOCADDRT) error in set_route (%d: %s)\n", errno, strerror(errno));
        exit(-1);
    }
}

TunController::~TunController() {
    close(sockfd);
}

char *TunController::getIfName() {
    return ifr.ifr_name;
}
