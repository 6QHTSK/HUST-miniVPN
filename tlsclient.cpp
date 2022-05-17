//
// Created by csepsk on 2022/5/13.
//
#include "src/Client.h"

using namespace std;
int main(int argc, char *argv[]){
    if(argc == 2){
        Client tlsClient;
        tlsClient.init(argv[1], 55555);
        tlsClient.verify();
        tlsClient.listen();
    }
    return 0;
}