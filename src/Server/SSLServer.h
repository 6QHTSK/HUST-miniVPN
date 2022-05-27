//
// Created by csepsk on 2022/5/15.
//

#ifndef MINIVPN_SSLSERVER_H
#define MINIVPN_SSLSERVER_H

#include <cerrno>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <map>
#include <sys/epoll.h>
#include <poll.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

typedef enum connection_status{SSL_ESTAB,VPN_ESTAB} connection_status;

class SSLConnection{
private:
    int sockfd;
    SSL *ssl;
public:
    struct sockaddr_in peerAddr;
    struct in_addr virtualAddr{};
    connection_status status = SSL_ESTAB;

    SSLConnection(int fd, SSL *ssl_in, sockaddr_in peerAddr_in);
    ~SSLConnection();
    ssize_t Send(const void *buff, size_t len) const;
    ssize_t Recv(void *buff, size_t size) const;
    int Fd() const;
};

class SSLServer{
private:
    int sockfd;
    SSL_CTX *ctx;
public:
    ~SSLServer();
    void Init(int port_number);
    SSLConnection * Accept() const;
    int Fd() const;
};


#endif //MINIVPN_SSLSERVER_H
