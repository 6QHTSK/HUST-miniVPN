//
// Created by csepsk on 2022/5/15.
//

#include "SockServer.h"

void SockServer::init(int port_number) {
    SockBase::init();
    struct sockaddr_in server{};

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port_number);

    auto bindVal = bind(sockfd, (struct sockaddr *) &server, sizeof(server));
    if (bindVal == -1){
        printf("Bind socket failed! (%d: %s)\n", errno, strerror(errno));
        exit(-1);
    }

    auto listenVal = listen(sockfd, 5);
    if(listenVal == -1){
        printf("Listen port failed! (%d: %s)\n", errno, strerror(errno));
        exit(-1);
    }
    printf("Listening for handshark\n", errno, strerror(errno));
}

SockConnection* SockServer::sockAccept() const {
    auto conn = new SockConnection();
    conn->init(sockfd);
    return conn;
}

SockConnection::SockConnection() : peerAddr(),virtualAddr(),status(NO_CONN){}

void SockConnection::init(int listenfd) {
    socklen_t peerAddrLen = sizeof( peerAddr );
    sockfd = accept(listenfd, ( struct sockaddr* )&peerAddr, &peerAddrLen );
    if(sockfd == -1){
        printf("Cannot accept connection (%d:%s)\n",errno, strerror(errno));
        exit(-1);
    }
    status = TCP_ESTAB;
    printf("Accept Connection From %s\n",inet_ntoa( peerAddr.sin_addr ));
}

