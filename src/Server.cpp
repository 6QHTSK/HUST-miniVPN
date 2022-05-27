//
// Created by csepsk on 2022/5/13.
//

#include "Server.h"

void Server::init(int port_number) {
    ifInfo.Init();
    in_addr virNet{};
    inet_aton("192.168.53.0",&virNet);
    virAddrPool.Init(virNet);
    tun = new Tun;
    tun->init(virAddrPool.GetGateWay(),virAddrPool.GetNetmask());
    sock = new SSLServer;
    sock->Init(port_number);
    ifInfo.PrintExtraIP(port_number);
}

void Server::sendPackage() {
    char buff[buff_size];
    in_addr srcIP{},destIP{};

    auto header = (IPHeader*) buff;
    bzero(buff, buff_size);
    auto len = tun->recv(buff,buff_size);

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
    conn->Send(buff, len);
}

// listen socket 有回应，进行三次握手建立连接
void Server::acceptNewConnection() {
    auto conn = sock->Accept();
    if(conn == nullptr){
        return;
    }
    connPoolFd[conn->Fd()] = conn;    // 在socket池中注册该socket
    epoll.Add(conn->Fd(), EPOLLIN | EPOLLET);
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

void Server::connectionEstab(SSLConnection *conn) {
    // 此部分接受账户密码
    VerifyPacket verifyPacket;
    conn->Recv((char *) &verifyPacket, sizeof(verifyPacket));
    if(!verify(verifyPacket)){
        fprintf(stdout,"TLS客户端(%s)验证失败\n",inet_ntoa(conn->peerAddr.sin_addr));
        deleteConn(conn);
        return;
    }
    // 分配虚拟地址
    VpnInitPacket initPacket{
        .virtualIP = virAddrPool.Alloc(conn->peerAddr.sin_addr),
        .mask = virAddrPool.GetNetmask(),
    };
    initPacket.routeCnt = ifInfo.GetIntranet(initPacket.routes);
    uint32_t packetSize = sizeof(in_addr)*2 + sizeof (uint32_t) + sizeof (Subnet) * initPacket.routeCnt;

    conn->Send((const char *) &initPacket, packetSize);
    conn->virtualAddr = initPacket.virtualIP;
    connPoolVirAddr[initPacket.virtualIP.s_addr] = conn; // 增加到虚拟地址簿中
    fprintf(stdout,"已建立通往%s",inet_ntoa(conn->peerAddr.sin_addr));
    fprintf(stdout,"(%s)的TLS通道！\n",inet_ntoa(conn->virtualAddr));
    conn->status = VPN_ESTAB;
}

void Server::recvPackage(SSLConnection *conn){
    char buff[buff_size];
    bzero(buff, buff_size);
    auto len = conn->Recv(buff, buff_size);
    if(len == 0){
        printf("客户端(%s) 停止了连接\n", inet_ntoa(conn->peerAddr.sin_addr));
        deleteConn(conn);
        return;
    }
    printf("Got a packet from the tunnel(%s)\n", inet_ntoa(conn->peerAddr.sin_addr));
    tun->send(buff,len);
}

void Server::listen() {
    if(sock == nullptr || tun == nullptr){
        throw std::invalid_argument("sock/tun 未定义");
    }
    epoll.Add(sock->Fd(), EPOLLIN | EPOLLET);
    epoll.Add(tun->fd(), EPOLLIN | EPOLLET);
    while (true) {
        int eventCnt = epoll.Wait();
        for (int i=0;i<eventCnt;i++){
            int eventfd = epoll.events[i].data.fd;
            if(eventfd == sock->Fd())
                acceptNewConnection();
            else if(eventfd == tun->fd())
                sendPackage();
            else{
                auto conn = connPoolFd[eventfd];
                if (conn == nullptr)
                    continue;
                switch(conn->status){
                    case SSL_ESTAB:
                        connectionEstab(conn);
                        break;
                    case VPN_ESTAB:
                        recvPackage(conn);
                        break;
                }
            }
        }
    }
}

Server::~Server() {
    delete tun;
    delete sock;
}

void Server::deleteConn(SSLConnection *conn){
    connPoolFd.erase(conn->Fd());
    if(conn->status == VPN_ESTAB){
        if(conn->virtualAddr.s_addr != 0){
            connPoolVirAddr.erase(conn->virtualAddr.s_addr);
            virAddrPool.Free(conn->peerAddr.sin_addr);
        }
    }
    delete conn;
}