//
// Created by csepsk on 2022/5/13.
//

#include "Server.h"

in_addr VirAddrPool::generateViraddr(uint8_t poolIndex) const{
    uint32_t t = ntohl(net.s_addr);
    t |= poolOffset + poolIndex;
    return in_addr{.s_addr=htonl(t) };
}

VirAddrPool::VirAddrPool() {
    viraddr = new struct viraddr[maxPoolSize];
}

VirAddrPool::~VirAddrPool() {
    delete viraddr;
}

void VirAddrPool::init(in_addr net_in) {
    net = net_in;
}

in_addr VirAddrPool::AllocVirAddr(in_addr peer) {
    for(uint8_t i=0;i<poolSize;i++){
        if(viraddr[i].peer.s_addr == peer.s_addr){
            viraddr[i].enable = true;
            return generateViraddr(i);
        }
    }
    if(poolSize < maxPoolSize){
        viraddr[poolSize].peer = peer;
        viraddr[poolSize].enable = true;
        return generateViraddr(poolSize);
    }
    for(uint8_t i=0;i<poolSize;i++){
        if(!viraddr[i].enable){
            viraddr[poolSize].peer = peer;
            viraddr[i].enable = true;
            return generateViraddr(i);
        }
    }
    return in_addr{};
}

void VirAddrPool::FreeVirAddr(in_addr peer) {
    for(uint8_t i=0;i<poolSize;i++){
        if(viraddr[i].peer.s_addr == peer.s_addr){
            viraddr[i].enable = false;
        }
    }
}

in_addr VirAddrPool::GetGateWay() const {
    uint32_t t = ntohl(net.s_addr);
    t |= 1;
    return in_addr{.s_addr=htonl(t) };
}

in_addr VirAddrPool::GetNetmask() {
    return mask;
}

int getPrefixLength(in_addr mask){
    uint32_t n = ntohl(mask.s_addr);
    int i = 0;
    for(i=0; n > 0; i++){
        n <<= 1;
    }
    return i;
}

bool IfAddrInfo::isIntranet(const in_addr subnet){
    const in_addr intranet2{.s_addr=htonl(0xAC100000)};
    const in_addr intranet2Mask{.s_addr=htonl(0xFFF00000)};
    const in_addr intranet3{.s_addr=htonl(0xC0A80000)};
    const in_addr intranet3Mask{.s_addr=htonl(0xFFFF0000)};
    if((subnet.s_addr & intranet2Mask.s_addr) == intranet2.s_addr)
        return true;
    if((subnet.s_addr & intranet3Mask.s_addr) == intranet3.s_addr)
        return true;
    return false;
}

void IfAddrInfo::init(){
    struct ifaddrs *ifList;

    if(getifaddrs(&ifList) < 0){
        exit(-1);
    }

    for(auto ifa = ifList; ifa != nullptr; ifa = ifa->ifa_next){
        if(ifa->ifa_addr != nullptr && ifa->ifa_addr->sa_family == AF_INET){
            in_addr mask = ((struct sockaddr_in*)ifa->ifa_netmask)->sin_addr;
            in_addr ip = ((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            in_addr subnet = {.s_addr = mask.s_addr & ip.s_addr};
            // 自动代理172.16.0.0/12 192.168.0.0/16 网络内容
            if(isIntranet(subnet)){
                intranet[intranetCnt++]={.IP=subnet,.netmask=mask};
                fprintf(stdout,"已代理子网: %s/%d\n",inet_ntoa(subnet), getPrefixLength(mask));
            }else if(subnet.s_addr != 0){
                extraIP[extraIPCnt++]=ip;
            }
        }
    }
}

uint8_t IfAddrInfo::getIntranet(Subnet *dest) {
    memcpy(dest,intranet,sizeof(Subnet)*intranetCnt);
    return intranetCnt;
}

void IfAddrInfo::printExtraIP(uint16_t portNum) {
    for(int i=0;i<extraIPCnt;i++){
        printf("%s:%d\n", inet_ntoa(extraIP[i]),portNum);
    }
}

void Server::init(int port_number) {
    ifInfo.init();
    in_addr virNet{};
    inet_aton("192.168.53.0",&virNet);
    virAddrPool.init(virNet);
    tun = new Tun;
    tun->init(virAddrPool.GetGateWay(),virAddrPool.GetNetmask());
    sock = new SockServer;
    sock->init(port_number);
    ifInfo.printExtraIP(port_number);
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
void Server::acceptNewConnection() {
    auto conn = sock->sockAccept();
    connPoolFd[conn->fd()] = conn;    // 在socket池中注册该socket
    epoll.Add(conn->fd(), EPOLLIN | EPOLLET | EPOLLRDHUP);
}

bool verify(const VerifyPacket packet){
    struct spwd *pw;
    char* epasswd;
    pw = getspnam(packet.username());
    if (pw == nullptr){
        return false;
    }
    epasswd = crypt(packet.password(),pw->sp_pwdp);
    return strcmp(epasswd,pw->sp_pwdp) == 0;
}

void Server::connectionEstab(SockConnection *conn) {
    // 此部分接受账户密码
    VerifyPacket verifyPacket;
    conn->sockRecv((char*)&verifyPacket,sizeof(verifyPacket));
    if(!verify(verifyPacket)){
        fprintf(stdout,"TLS客户端(%s)验证失败\n",inet_ntoa(conn->peerAddr.sin_addr));
        deleteConn(conn);
        return;
    }
    // 分配虚拟地址
    VpnInitPacket initPacket{};
    initPacket.virtualIP = virAddrPool.AllocVirAddr(conn->peerAddr.sin_addr);
    initPacket.mask = virAddrPool.GetNetmask();
    initPacket.routeCnt = ifInfo.getIntranet(initPacket.routes);
    uint32_t packetSize = sizeof(in_addr)*2 + sizeof (uint32_t) + sizeof (Subnet) * initPacket.routeCnt;
    conn->sockSend((const char*)&initPacket, packetSize);
    conn->virtualAddr = initPacket.virtualIP;
    connPoolVirAddr[initPacket.virtualIP.s_addr] = conn; // 增加到虚拟地址簿中
    fprintf(stdout,"已建立通往%s",inet_ntoa(conn->peerAddr.sin_addr));
    fprintf(stdout,"(%s)的TLS通道！\n",inet_ntoa(conn->virtualAddr));
    conn->status = VPN_ESTAB;
}

void Server::getSocketData(SockConnection *conn){
    char buff[buff_size];
    bzero(buff, buff_size);
    auto len = conn->sockRecv(buff, buff_size);
    if(len == 0){
        printf("客户端(%s) 停止了连接\n", inet_ntoa(conn->peerAddr.sin_addr));
        deleteConn(conn);
        return;
    }
    printf("Got a packet from the tunnel(%s)\n", inet_ntoa(conn->peerAddr.sin_addr));
    tun->send(buff,len);
}

void Server::listen() {
    epoll.Add(sock->fd(), EPOLLIN | EPOLLET | EPOLLRDHUP);
    epoll.Add(tun->fd(), EPOLLIN | EPOLLET);
    while (true) {
        int eventCnt = epoll.Wait();
        for (int i=0;i<eventCnt;i++){
            int eventfd = epoll.events[i].data.fd;
            if(eventfd == sock->fd())
                acceptNewConnection();
            else if(eventfd == tun->fd())
                tunSelected();
            else{
                auto conn = connPoolFd[eventfd];
                if (conn != nullptr){
                    if(epoll.events[i].events & EPOLLHUP){
                        printf("Socket Closed!\n");
                        deleteConn(conn);
                        continue;
                    }
                    switch(conn->status){
                        case SSL_ESTAB:
                            connectionEstab(conn);
                            break;
                        case VPN_ESTAB:
                            getSocketData(conn);
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

void Server::deleteConn(SockConnection *conn){
    connPoolFd.erase(conn->fd());
    if(conn->status == VPN_ESTAB){
        if(conn->virtualAddr.s_addr != 0){
            connPoolVirAddr.erase(conn->virtualAddr.s_addr);
            virAddrPool.FreeVirAddr(conn->peerAddr.sin_addr);
        }
    }
    conn->status = TCP_ACCEPT;
    delete conn;
}