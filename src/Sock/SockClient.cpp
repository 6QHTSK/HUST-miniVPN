//
// Created by csepsk on 2022/5/15.
//

#include "SockClient.h"

void SockClient::init(const char *svrip, int port_number){
    SockBase::init();

    struct sockaddr_in peerAddr{};
    memset(&peerAddr, 0, sizeof(peerAddr));
    peerAddr.sin_family = AF_INET;
    peerAddr.sin_port = htons(port_number);
    peerAddr.sin_addr.s_addr = inet_addr(svrip);

    auto connectVal = connect(sockfd,(struct sockaddr*)&peerAddr,sizeof(peerAddr));
    if (connectVal == -1){
        printf("Connect to Server failed! (%d: %s)\n", errno, strerror(errno));
        exit(-1);
    }
    printf("Connect to Server %s\n", inet_ntoa(peerAddr.sin_addr));
}