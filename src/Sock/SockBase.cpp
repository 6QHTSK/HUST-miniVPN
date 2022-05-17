//
// Created by csepsk on 2022/5/17.
//

#include "SockBase.h"

ssize_t SockBase::sockSend(const char *buff, size_t len) const {
    return send(sockfd, buff, len, 0);
}

ssize_t SockBase::sockRecv(char *buff, size_t size) const {
    return recv(sockfd, buff, size, 0);
}

int SockBase::fd() const {
    return sockfd;
}

SockBase::~SockBase() {
    close(sockfd);
}

void SockBase::init() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Create socket failed! (%d: %s)\n", errno, strerror(errno));
        exit(-1);
    }
}

// NOT INIT!
SockBase::SockBase():sockfd(-1) {}
