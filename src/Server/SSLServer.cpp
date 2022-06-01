//
// Created by csepsk on 2022/5/15.
//

#include "SSLServer.h"

#define HOME	"./cert/"
#define CERTF	HOME"server.crt"
#define KEYF	HOME"server.key"
#define CACERT	HOME"ca.crt"

auto initSSLCtx(){
    // Step 0: OpenSSL library initialization
    // This step is no longer needed as of version 1.1.0.
    SSL_library_init();
    SSL_load_error_strings();
    SSLeay_add_ssl_algorithms();

    // Step 1: SSL context initialization
    const SSL_METHOD *meth = SSLv23_server_method();
    auto ctx = SSL_CTX_new(meth);
    //SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, nullptr);
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, nullptr);
    // 加载CA
    if(SSL_CTX_load_verify_locations(ctx, CACERT, nullptr) != 1){
        ERR_print_errors_fp(stderr);
        exit(1);
    }

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
        fprintf(stderr, "私钥与证书之间验证失败！\n");
        exit(5);
    }
    return ctx;
}

void SSLServer::Init(int port_number) {
    ctx = initSSLCtx();
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Create socket failed! (%d: %s)\n", errno, strerror(errno));
        exit(-1);
    }
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

SSLConnection* SSLServer::Accept() const {
    SSL* ssl = SSL_new(ctx); // ctx
    if(ssl == nullptr){
        printf("SSL_new failed!");
        return nullptr;
    }
    struct sockaddr_in peerAddr{};
    socklen_t peerAddrLen = sizeof( peerAddr );
    int connfd = accept(sockfd, ( struct sockaddr* )&peerAddr, &peerAddrLen );
    if(sockfd == -1){
        printf("无法接受连接 (%d:%s)\n",errno, strerror(errno));
        return nullptr;
    }
    int r = SSL_set_fd(ssl,connfd);
    if(r == 0){
        printf("SSL_set_fd failed!,(%d,%s)",errno, strerror(errno));
        close(connfd);
        return nullptr;
    }
    r = SSL_accept(ssl);
    if( r <= 0 ){
        printf("无法建立SSL连接(%d)\n", SSL_get_error(ssl,r));
        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(connfd);
        return nullptr;
    }else{
        auto conn = new SSLConnection(connfd, ssl, peerAddr);
        printf("TLS客户端 %s 开始连接\n",inet_ntoa( peerAddr.sin_addr ));
        return conn;
    }
}

int SSLServer::Fd() const {
    return sockfd;
}

SSLServer::~SSLServer(){
    close(sockfd);
    SSL_CTX_free(ctx);
}

SSLConnection::SSLConnection(int fd, SSL* ssl_in, sockaddr_in peerAddr_in) : peerAddr(peerAddr_in), sockfd(fd), ssl(ssl_in){}

SSLConnection::~SSLConnection(){
    if(ssl != nullptr){
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }
    close(sockfd);
}

ssize_t SSLConnection::Send(const void *buff, size_t len) const {
    return SSL_write(ssl, buff, (int)len);
}

ssize_t SSLConnection::Recv(void *buff, size_t size) const {
    return SSL_read(ssl,buff,(int)size);
}

int SSLConnection::Fd() const {
    return sockfd;
}