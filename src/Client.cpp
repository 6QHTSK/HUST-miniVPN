//
// Created by csepsk on 2022/5/13.
//

#include "Client.h"

void Client::init(const char *peerAddr, int port_number = 4443){
    sock = new SockClient();
    tun = new Tun;
    sock->init(peerAddr, port_number);
}

bool Client::verify(){
    const char* verify = "hello";
    sock->sockSend(verify,sizeof(verify));
    VpnInitPacket initPacket{};
    sock->sockRecv((char*)&initPacket,sizeof(vpnInitPacket));
    tun->init(initPacket.virtualIP, initPacket.mask);
    for(int i=0;i<initPacket.routeCnt;i++)
        tun->addRouteTo(initPacket.routes[i].IP,initPacket.routes[i].netmask);
    return true;
}

void Client::tunSelected() {
    long len;
    char buff[buff_size];

    printf("Got a packet from TUN\n");

    bzero(buff, buff_size);
    len = tun->recv(buff,buff_size);
    sock->sockSend(buff, len);
}

void Client::socketSelected(){
    long len;
    char buff[buff_size];

    printf("Got a packet from the tunnel\n");

    bzero(buff, buff_size);
    len = sock->sockRecv(buff, buff_size);
    if(len == 0){
        printf("Server Stop the tunnel!\n");
        exit(0);
    }
    tun->send(buff,len);
}

[[noreturn]] void Client::listen() {
    Epoll epoll;
    epoll.add(sock->fd());
    epoll.add(tun->fd());
    while (true) {
       int eventCnt = epoll.wait();
       for(int i=0;i<eventCnt;i++){
           int eventfd = epoll.events[i].data.fd;
           if(eventfd == sock->fd())
               socketSelected();
           else if(eventfd == tun->fd())
               tunSelected();
       }
    }
}

Client::~Client() {
    delete tun;
    delete sock;
}
