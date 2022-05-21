//
// Created by csepsk on 2022/5/15.
//

#ifndef MINIVPN_SOCKCLIENT_H
#define MINIVPN_SOCKCLIENT_H

#include <cerrno>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "SockBase.h"

class SockClient : public SockBase {
private:
    SSL_CTX *ctx;
public:
    ~SockClient();
    void init(const char *svrip, int port_number);
};


#endif //MINIVPN_SOCKCLIENT_H
