//
// Created by csepsk on 2022/5/14.
//

#include "Tun.h"

// NOT INIT
Tun::Tun():tunfd(-1) {}

void Tun::init(in_addr runIP, in_addr netmask) {
    struct ifreq ifr{};
    memset(&ifr, 0, sizeof(ifr));

    // 1. Open the device
    tunfd = open("/dev/net/tun", O_RDWR);
    if (tunfd == -1) {
        printf("Open /dev/net/tun failed! (%d: %s)\n", errno, strerror(errno));
        exit(-1);
    }

    // 2. setup the virtual tun interface tun0
    ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
    if (ioctl(tunfd, TUNSETIFF, &ifr) == -1) {
        printf("Setup TUN interface by ioctl failed! (%d: %s)\n", errno, strerror(errno));
        exit(-1);
    }

    // 3. use socket to connect the tun
    ctlSock = new TunController(ifr);
    ctlSock->init();

    // 4. Assign IP Address
    ctlSock->adapterAssignIp(runIP);
    // 5. Assign NetMask
    ctlSock->adapterAssignNetmask(netmask);
    // 6. Run TUN Device
    ctlSock->adapterActivate();

    printf("TLS虚拟网卡(%s)已就绪，",ctlSock->getIfName());
    printf("IP地址：%s，",inet_ntoa(runIP));
    printf("子网掩码：%s\n", inet_ntoa(netmask));
}


ssize_t Tun::send(const char *buff, size_t len) const {
    return write(tunfd, buff, len);
}

ssize_t Tun::recv(char *buff, size_t size) const {
    return read(tunfd, buff, size);
}

int Tun::fd() const {
    return tunfd;
}

Tun::~Tun() {
    close(tunfd);
    delete ctlSock;
}

void Tun::addRouteTo(in_addr destIP, in_addr netmask) {
    if(ctlSock == nullptr){
        throw std::invalid_argument("ctlSock 未定义");
    }
    ctlSock->adapterAddRouteTo(destIP,netmask);
}

