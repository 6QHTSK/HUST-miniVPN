//
// Created by csepsk on 2022/5/17.
//

#ifndef MINIVPN_SOCKBASE_H
#define MINIVPN_SOCKBASE_H

#include <cstdio>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

class SockBase {
protected:
    int sockfd;
    SSL *ssl;
public:
    SockBase();
    void init();
    ~SockBase();
    ssize_t sockSend(const void *buff, size_t len) const;
    ssize_t sockRecv(void *buff, size_t size) const;
    int fd() const;
};


#endif //MINIVPN_SOCKBASE_H
