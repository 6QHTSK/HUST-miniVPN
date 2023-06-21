//
// Created by csepsk on 2022/5/13.
//
#include "src/Server.h"

using namespace std;

int main() {
    Server tlsServer;
    tlsServer.Init(PORT);
    tlsServer.Listen();
}