//
// Created by csepsk on 2022/5/15.
//

#include "SSLClient.h"

/* define HOME to be dir for key and cert files... */
#define HOME	"./cert/"

/* Make these what you want for cert & key files */
#define CERTF	HOME"client.crt"
#define KEYF	HOME"client.key"
#define CACERT	HOME"ca.crt"

ssize_t SSLClient::Send(const void *buff, size_t len) const {
    return SSL_write(ssl, buff, (int)len);
}

ssize_t SSLClient::Recv(void *buff, size_t size) const {
    return SSL_read(ssl,buff,(int)size);
}

int SSLClient::Fd() const {
    return sockfd;
}

auto initSSLCtx(){
    // Step 0: OpenSSL library initialization
    // This step is no longer needed as of version 1.1.0.
    SSL_library_init();
    SSL_load_error_strings();
    SSLeay_add_ssl_algorithms();

    auto meth = SSLv23_client_method();
    auto ctx = SSL_CTX_new(meth);

    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, nullptr);
    // 加载CA证书
    if(SSL_CTX_load_verify_locations(ctx,CACERT, nullptr) != 1){
        ERR_print_errors_fp(stderr);
        exit(-1);
    }
    if(SSL_CTX_set_default_verify_paths(ctx) != 1){
        ERR_print_errors_fp(stderr);
        exit(-1);
    }
    // 加载客户端证书
    if (SSL_CTX_use_certificate_file(ctx, CERTF, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(-2);
    }
    // 加载客户端私钥
    if (SSL_CTX_use_PrivateKey_file(ctx, KEYF, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(-3);
    }
    // 验证私钥
    if (!SSL_CTX_check_private_key(ctx)) {
        printf("Private key does not match the certificate public keyn");
        exit(-4);
    }
    return ctx;
}

void SSLClient::Init(const char *svrip, int port_number){
    ctx = initSSLCtx();
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Create socket failed! (%d: %s)\n", errno, strerror(errno));
        exit(-1);
    }

    struct sockaddr_in peerAddr{};
    memset(&peerAddr, 0, sizeof(peerAddr));
    peerAddr.sin_family = AF_INET;
    peerAddr.sin_port = htons(port_number);
    peerAddr.sin_addr.s_addr = inet_addr(svrip);

    auto connectVal = connect(sockfd,(struct sockaddr*)&peerAddr,sizeof(peerAddr));
    if (connectVal == -1){
        printf("连接到 TLSServer 失败 (%d: %s)\n", errno, strerror(errno));
        exit(-1);
    }
    printf("已连接到 TLSServer %s\n", inet_ntoa(peerAddr.sin_addr));

    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sockfd);
    auto r = SSL_connect(ssl);

    if(r <= 0){
        auto err = SSL_get_error(ssl, r);
        printf("验证失败！(%d),(%d: %s)\n",err, errno, strerror(errno));
        exit(-1);
    }else{
        printf("服务器证书验证成功\n");
    }
}

SSLClient::~SSLClient() {
    if(ssl != nullptr){
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }
    close(sockfd);
    SSL_CTX_free(ctx);
}
