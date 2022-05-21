//
// Created by csepsk on 2022/5/15.
//

#include "SockServer.h"

#define HOME	"./openssl/"
#define CERTF	HOME"server.crt"
#define KEYF	HOME"server.key"
#define CACERT	HOME"ca.crt"

void SockServer::init(int port_number) {
    // Step 0: OpenSSL library initialization
    // This step is no longer needed as of version 1.1.0.
    SSL_library_init();
    SSL_load_error_strings();
    SSLeay_add_ssl_algorithms();

    // Step 1: SSL context initialization
    const SSL_METHOD *meth = SSLv23_server_method();
    ctx = SSL_CTX_new(meth);
    SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, nullptr);
    //SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
    SSL_CTX_load_verify_locations(ctx, CACERT, nullptr);

    // Step 2: Set up the server certificate and private key
    if (SSL_CTX_use_certificate_file(ctx, CERTF, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(3);
    }
    if (SSL_CTX_use_PrivateKey_file(ctx, KEYF, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(4);
    }
    if (!SSL_CTX_check_private_key(ctx)) {
        fprintf(stderr, "Private key does not match the certificate public key\n");
        exit(5);
    }
    printf("SSL 部分初始化完成！\n");

    SockBase::init();
    struct sockaddr_in server{};

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port_number);

    auto bindVal = bind(sockfd, (struct sockaddr *) &server, sizeof(server));
    if (bindVal == -1){
        printf("Bind socket failed! (%d: %s)\n", errno, strerror(errno));
        exit(-1);
    }

    auto listenVal = listen(sockfd, 5);
    if(listenVal == -1){
        printf("Listen port failed! (%d: %s)\n", errno, strerror(errno));
        exit(-1);
    }
    printf("TLS服务器已启动，通过下述IP地址连接：\n");
}

SockConnection* SockServer::sockAccept() const {
    auto conn = new SockConnection(ctx);
    conn->init(sockfd);
    return conn;
}

SockServer::~SockServer(){
    SSL_CTX_free(ctx);
}

SockConnection::SockConnection(SSL_CTX* ctx_in) : peerAddr(),virtualAddr(),ctx(ctx_in){}

void SockConnection::init(int listenfd) {
    socklen_t peerAddrLen = sizeof( peerAddr );
    sockfd = accept(listenfd, ( struct sockaddr* )&peerAddr, &peerAddrLen );
    if(sockfd == -1){
        printf("无法接受连接 (%d:%s)\n",errno, strerror(errno));
        exit(-1);
    }
    ssl = SSL_new(ctx); // ctx
    if(ssl == nullptr){
        printf("SSL_new failed!");
        exit(-1);
    }
    int r = SSL_set_fd(ssl,sockfd);
    if(r == 0){
        printf("SSL_set_fd failed!,(%d,%s)",errno, strerror(errno));
        exit(-1);
    }
    r = SSL_accept(ssl);
    if( r <= 0 ){
        printf("无法建立SSL连接(%d)\n", SSL_get_error(ssl,r));
        exit(-1);
    }else{
        printf("完成SSL连接\n");
    }
    status = SSL_ESTAB;
    printf("TLS客户端 %s 开始连接\n",inet_ntoa( peerAddr.sin_addr ));
}