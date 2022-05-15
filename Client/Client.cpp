//
// Created by csepsk on 2022/5/13.
//

#include "Client.h"

Client::Client(const char *peerAddr, int port_number = 4443){
    tun = new Tun;
    tun->up("192.168.53.5","255.255.255.0");
    tun->addRouteTo("192.168.60.0","255.255.255.0");
    sock = new SockClient(peerAddr, port_number);
}

void Client::tunSelected() {
    long len;
    char buff[buff_size];

    printf("Got a packet from TUN\n");

    bzero(buff, buff_size);
    len = tun->recv(buff,buff_size);
    sock->send(buff,len);
}

void Client::socketSelected(){
    long len;
    char buff[buff_size];

    printf("Got a packet from the tunnel\n");

    bzero(buff, buff_size);
    len = sock->recv(buff,buff_size);
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
