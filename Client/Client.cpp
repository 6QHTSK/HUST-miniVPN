//
// Created by csepsk on 2022/5/13.
//

#include "Client.h"

Client::Client(const char *peerAddr, int port_number = 4443): sock(peerAddr, port_number){
    tun.up("192.168.53.5","255.255.255.0");
    tun.addRouteTo("192.168.60.0","255.255.255.0");
}

void Client::tunSelected() {
    long len;
    char buff[buff_size];

    printf("Got a packet from TUN\n");

    bzero(buff, buff_size);
    len = tun.recv(buff,buff_size);
    sock.send(buff,len);
}

void Client::socketSelected(){
    long len;
    char buff[buff_size];

    printf("Got a packet from the tunnel\n");

    bzero(buff, buff_size);
    len = sock.recv(buff,buff_size);
    tun.send(buff,len);
}

[[noreturn]] void Client::listen() {
    while (true) {
        fd_set readFDSet;

        FD_ZERO(&readFDSet);
        FD_SET(sock.fd(), &readFDSet);
        FD_SET(tun.fd(), &readFDSet);
        select(FD_SETSIZE, &readFDSet, nullptr, nullptr, nullptr);

        if (FD_ISSET(tun.fd(), &readFDSet))
            tunSelected();
        if (FD_ISSET(sock.fd(), &readFDSet))
            socketSelected();
    }
}