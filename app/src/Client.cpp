//
// Created by csepsk on 2022/5/13.
//

#include "Client.h"

void Client::Init(const char *peerAddr, int port_number = 4443){
    sock = new SSLClient();
    tun = new Tun;
    sock->Init(peerAddr, port_number);
    HelloPacket packet;
    auto len=sock->Recv(&packet,sizeof(HelloPacket));
    if(len == 0){
        printf("检查客户端证书（例如是否过期），服务端认证失败！\n");
        exit(-1);
    }
}

bool Client::Verify(const char *username, const char *password) {
    if(sock == nullptr || tun == nullptr){
        throw std::invalid_argument("sock/tun 未定义");
    }
    VerifyPacket verify;
    uint32_t usernameLen =(uint8_t) strlen(username) + 1;
    uint32_t passwordLen =(uint8_t) strlen(password) + 1;
    memcpy(verify.verifyInfo,username,usernameLen);
    memcpy(verify.verifyInfo+usernameLen,password,passwordLen);
    verify.usernameLen = usernameLen;
    verify.passwordLen = passwordLen;
    sock->Send((const char *) &verify, sizeof(verify));
    VpnInitPacket initPacket{};
    auto length = sock->Recv((char *) &initPacket, sizeof(vpnInitPacket));
    if(length == 0){
        printf("\n验证失败！\n");
        return false;
    }
    printf("\n验证成功！\n");
    tun->init(initPacket.virtualIP, initPacket.mask);
    for(int i=0;i<initPacket.routeCnt;i++){
        tun->addRouteTo(initPacket.routes[i].IP,initPacket.routes[i].netmask);
        printf("TLS服务器代理的子网 %s ",inet_ntoa(initPacket.routes[i].IP));
        printf("子网掩码: %s\n", inet_ntoa(initPacket.routes[i].netmask));
    }
    return true;
}

void Client::sendPackage() {
    long len;
    char buff[buff_size];

    bzero(buff, buff_size);
    len = tun->recv(buff,buff_size);
    printf("Got a packet from TUN\n");
    sock->Send(buff, len);
}

void Client::recvPackage(){
    long len;
    char buff[buff_size];

    bzero(buff, buff_size);
    len = sock->Recv(buff, buff_size);
    if(len == 0){
        printf("服务端停止了连接\n");
        exit(0);
    }
    printf("Got a packet from the tunnel\n");
    tun->send(buff,len);
}

[[noreturn]] void Client::Listen() {
    if(sock == nullptr || tun == nullptr){
        throw std::invalid_argument("sock/tun 未定义");
    }
    Epoll epoll;
    epoll.Add(sock->Fd(), EPOLLIN | EPOLLET);
    epoll.Add(tun->fd(), EPOLLIN | EPOLLET);
    while (true) {
       int eventCnt = epoll.Wait();
       for(int i=0;i<eventCnt;i++){
           int eventfd = epoll.Events[i].data.fd;
           if(eventfd == sock->Fd())
               recvPackage();
           else if(eventfd == tun->fd())
               sendPackage();
       }
    }
}

Client::~Client() {
    delete tun;
    delete sock;
}
