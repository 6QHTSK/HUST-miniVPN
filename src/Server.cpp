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

uint8_t initSubNet(Subnet subnets[]){
    struct ifaddrs *ifList;
    uint8_t subnetCnt = 0;

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
                subnets[subnetCnt++]={.IP=subnet,.netmask=mask};
                fprintf(stderr,"代理子网: %s/%d\n",inet_ntoa(subnet), getPrefixLength(mask));
            }
        }
    }
    return subnetCnt;
}

void Server::init(int port_number) {
    subnetCnt = initSubNet(subnets);
    in_addr runIP{}, netmask{};
    inet_aton("192.168.53.1",&runIP);
    inet_aton("255.255.255.0",&netmask);
    tun = new Tun;
    tun->init(runIP,netmask);
    sock = new SockServer;
    sock->init(port_number);
}

void Server::tunSelected() {
    struct IPHeader{
        uint8_t frontPadding[12];
        uint32_t srcIP;
        uint32_t destIP;
    } *header;
    char buff[buff_size];
    in_addr srcIP{},destIP{};

    header = (IPHeader*) buff;
    bzero(buff, buff_size);
    auto len = tun->recv(buff,buff_size);
    // byte[16-20] is des.ip
    srcIP.s_addr = header->srcIP;
    destIP.s_addr = header->destIP;
    auto conn = connPoolVirAddr[destIP.s_addr];
    if(conn == nullptr){
        printf("Unknown Destination Packet to %s, Throw it.",inet_ntoa(destIP));
        return;
    }

    printf("Got a packet from TUN! %s", inet_ntoa(srcIP));
    printf(" => %s",inet_ntoa(destIP));
    printf("(%s)\n", inet_ntoa(conn->peerAddr.sin_addr));
    conn->sockSend(buff, len);
}

// listen socket 有回应，进行三次握手建立连接
void Server::listenSocketSelected() {
    fprintf(stdout,"Got a packet handsharking...\n");
    auto conn = sock->sockAccept();
    connPoolFd[conn->fd()] = conn;    // 在socket池中注册该socket
    epoll.add(conn->fd());
}

void Server::tcpEstabSelected(SockConnection *conn) {
    // 此部分未来接受账户密码
    char buff[buff_size];
    conn->sockRecv(buff,buff_size);
    fprintf(stdout,"Receieve Hello Message \"%s\"\n",buff);
    // 分配虚拟地址
    VpnInitPacket initPacket{};
    inet_aton("192.168.53.5",&initPacket.virtualIP);
    inet_aton("255.255.255.0",&initPacket.mask);
    initPacket.routeCnt = subnetCnt;
    memcpy(initPacket.routes, subnets, sizeof(Subnet) * subnetCnt);
    uint32_t packetSize = sizeof(in_addr)*2 + sizeof (uint32_t) + sizeof (Subnet) * subnetCnt;
    conn->sockSend((const char*)&initPacket, packetSize);
    conn->virtualAddr = initPacket.virtualIP;
    connPoolVirAddr[initPacket.virtualIP.s_addr] = conn; // 增加到虚拟地址簿中

    conn->status = VPN_ESTAB;
}

void Server::vpnEstabSelected(SockConnection *conn){
    char buff[buff_size];

    printf("Got a packet from the tunnel(%s)\n", inet_ntoa(conn->peerAddr.sin_addr));

    bzero(buff, buff_size);
    auto len = conn->sockRecv(buff, buff_size);
    if(len == 0){
        printf("Client(%s) Stop the tunnel!\n", inet_ntoa(conn->peerAddr.sin_addr));
        conn->status = NO_CONN;
        connPoolFd.erase(conn->fd());
        if(conn->virtualAddr.s_addr != 0){
            connPoolVirAddr.erase(conn->virtualAddr.s_addr);
        }
        delete conn;
    }
    tun->send(buff,len);
}

void Server::listen() {
    epoll.add(sock->fd());
    epoll.add(tun->fd());
    while (true) {
        int eventCnt = epoll.wait();
        for (int i=0;i<eventCnt;i++){
            int eventfd = epoll.events[i].data.fd;
            if(eventfd == sock->fd())
                listenSocketSelected();
            else if(eventfd == tun->fd())
                tunSelected();
            else{
                auto conn = connPoolFd[eventfd];
                if (conn != nullptr){
                    switch(conn->status){
                        case TCP_ESTAB:
                            tcpEstabSelected(conn);
                            break;
                        case VPN_ESTAB:
                            vpnEstabSelected(conn);
                            break;
                        default:
                            printf("Get Nothing package!! status:%d",conn->status);
                    }
                }
            }
        }
    }
}

Server::~Server() {
    delete tun;
    delete sock;
}