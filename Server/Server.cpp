//
// Created by csepsk on 2022/5/13.
//

#include "Server.h"

int getPrefixLength(in_addr mask){
    uint32_t n = ntohl(mask.s_addr);
    int i = 0;
    while(n > 0){
        n <<= 1;
        i++;
    }
    return i;
}

bool isIntranet(in_addr subnet){
    in_addr intranet2{};
    inet_aton("172.16.0.0",&intranet2);
    in_addr intranet2Mask{};
    inet_aton("255.240.0.0",&intranet2Mask);
    in_addr intranet3{};
    inet_aton("192.168.0.0",&intranet3);
    in_addr intranet3Mask{};
    inet_aton("255.255.0.0",&intranet3Mask);
    if((subnet.s_addr & intranet2Mask.s_addr) == intranet2.s_addr)
        return true;
    if((subnet.s_addr & intranet3Mask.s_addr) == intranet3.s_addr)
        return true;
    return false;
}

Subnets* addSubNet(Subnets *cur, in_addr IP, in_addr netmask) {
    auto* newSubnet = new Subnets;
    newSubnet->IP = IP;
    newSubnet->netmask = netmask;
    newSubnet->next = nullptr;
    if(cur != nullptr)
        cur->next = newSubnet;
    return newSubnet;
}

Subnets* initSubNet(){
    struct ifaddrs *ifList;
    Subnets* subnets = nullptr;
    auto cur = subnets;

    if(getifaddrs(&ifList) < 0){
        exit(-1);
    }

    fprintf(stderr,"Start Getting subnet info\n");
    for(auto ifa = ifList; ifa != nullptr; ifa = ifa->ifa_next){
        if(ifa->ifa_addr != nullptr && ifa->ifa_addr->sa_family == AF_INET){
            in_addr mask = ((struct sockaddr_in*)ifa->ifa_netmask)->sin_addr;
            in_addr ip = ((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            in_addr subnet = {.s_addr = mask.s_addr & ip.s_addr};
            // 自动代理172.16.0.0/12 192.168.0.0/16 网络内容
            if(isIntranet(subnet)){
                fprintf(stderr,"代理子网: %s/%d\n",inet_ntoa(subnet), getPrefixLength(mask));
                cur = addSubNet(cur,subnet,mask);
                if(subnets == nullptr)subnets = cur;
            }
        }
    }
    return subnets;
}

Server::Server(int port_number){
    subnets = initSubNet();
    tun = new Tun;
    tun->up("192.168.53.1","255.255.255.0");
    sock = new SockServer(port_number);
}

void Server::tunSelected() {
    char buff[buff_size];

    printf("Got a packet from TUN\n");

    bzero(buff, buff_size);
    auto len = tun->recv(buff,buff_size);
    sock->sockSend(buff, len);
}

void Server::socketSelected() {
    char buff[buff_size];

    printf("Got a packet from the tunnel\n");

    bzero(buff, buff_size);
    auto len = sock->sockRecv(buff, buff_size);
    if(len == 0){
        printf("Client Stop the tunnel!\n");
        exit(0);
    }
    tun->send(buff,len);
}

void Server::listen() {
    Epoll epoll;
    epoll.add(sock->fd());
    epoll.add(tun->fd());
    while (true) {
        int eventCnt = epoll.wait();
        for (int i=0;i<eventCnt;i++){
            int eventfd = epoll.events[i].data.fd;
            if(eventfd == sock->fd())
                socketSelected();
            else if(eventfd == tun->fd())
                tunSelected();
        }
    }
}

Server::~Server() {
    delete tun;
    delete sock;
    delete subnets;
}


Subnets::~Subnets() {
    delete next;
}
