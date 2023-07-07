//
// Created by csepsk on 2022/5/15.
//

#ifndef MINIVPN_SSLCLIENT_H
#define MINIVPN_SSLCLIENT_H

#include <cerrno>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

class SSLClient{
private:
    int sockfd;
    SSL *ssl;
    SSL_CTX *ctx;
public:
    ~SSLClient();
    void Init(const char *svrip, int port_number);
    ssize_t Send(const void *buff, size_t len) const;
    ssize_t Recv(void *buff, size_t size) const;
    int Fd() const;
};


#endif //MINIVPN_SSLCLIENT_H
