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
#include "SockBase.h"

class SockClient : public SockBase {
public:
    void init(const char *svrip, int port_number);
};


#endif //MINIVPN_SOCKCLIENT_H
