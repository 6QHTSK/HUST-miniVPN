//
// Created by csepsk on 2022/5/15.
//

#include "SockServer.h"

int SockServer::fd() const {
    return sockfd;
}

SockServer::SockServer(int port_number) {
    struct sockaddr_in server{};
    char buff[100];

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port_number);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        printf("Create socket failed! (%d: %s)\n", errno, strerror(errno));
        exit(-1);
    }

    auto bindVal = bind(sockfd, (struct sockaddr *) &server, sizeof(server));
    if (bindVal == -1){
        printf("Bind socket failed! (%d: %s)\n", errno, strerror(errno));
        exit(-1);
    }

    listen(sockfd,1);

    // Wait for the VPN client to "connect".
    bzero(buff, 100);
    socklen_t peerAddrLen = sizeof(struct sockaddr_in);

    recvfrom(sockfd, buff, 100, 0,(struct sockaddr *) &peerAddr, &peerAddrLen);

    printf("Accept connect from client %s: %s\n", inet_ntoa(peerAddr.sin_addr), buff);
}

ssize_t SockServer::sockSend(const char *buff, size_t len) {
    return sendto(sockfd, buff, len, 0, (struct sockaddr *) &peerAddr, sizeof(peerAddr));
}

ssize_t SockServer::sockRecv(char *buff, size_t size) const {
    return recvfrom(sockfd, buff, size, 0, nullptr, nullptr);
}

SockServer::~SockServer() {
    close(sockfd);
}
