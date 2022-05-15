//
// Created by csepsk on 2022/5/13.
//
#include "Client/Client.h"

using namespace std;
int main(int argc, char *argv[]){
    if(argc == 2){
        auto tlsClient = Client(argv[1], 55555);
        tlsClient.listen();
    }
    return 0;
}