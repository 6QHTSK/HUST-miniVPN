//
// Created by csepsk on 2022/5/15.
//

#include "SockClient.h"

SockClient::SockClient(const char *svrip, int port_number){
    auto hello = "Hello";

    memset(&peerAddr, 0, sizeof(peerAddr));
    peerAddr.sin_family = AF_INET;
    peerAddr.sin_port = htons(port_number);
    peerAddr.sin_addr.s_addr = inet_addr(svrip);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        printf("Create socket failed! (%d: %s)\n", errno, strerror(errno));
        exit(-1);
    }
    // Send a hello message to "connect" with the VPN Server
    send(hello,strlen(hello));
    printf("Connect to Server %s: %s\n", inet_ntoa(peerAddr.sin_addr), hello);
}

ssize_t SockClient::send(const char *buff, size_t len) {
    return sendto(sockfd, buff, len, 0, (struct sockaddr *) &peerAddr, sizeof(peerAddr));
}

ssize_t SockClient::recv(char *buff, size_t size) const {
    return recvfrom(sockfd, buff, size, 0, nullptr, nullptr);
}

int SockClient::fd() const {
    return sockfd;
}

SockClient::~SockClient() {
    close(sockfd);
}
