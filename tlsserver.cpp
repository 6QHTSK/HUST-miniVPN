//
// Created by csepsk on 2022/5/13.
//
#include "Server/Server.h"

using namespace std;
int main(){
    auto tlsServer = Server(55555);
    tlsServer.listen();
}