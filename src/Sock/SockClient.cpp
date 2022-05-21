//
// Created by csepsk on 2022/5/15.
//

#include "SockClient.h"

/* define HOME to be dir for key and cert files... */
#define HOME	"./openssl/"

/* Make these what you want for cert & key files */
#define CERTF	HOME"client.crt"
#define KEYF	HOME"client.key"
#define CACERT	HOME"ca.crt"


void SockClient::init(const char *svrip, int port_number){
    // Step 0: OpenSSL library initialization
    // This step is no longer needed as of version 1.1.0.
    SSL_library_init();
    SSL_load_error_strings();
    SSLeay_add_ssl_algorithms();

    auto meth = SSLv23_client_method();
    ctx = SSL_CTX_new(meth);

    SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, nullptr);
    if (SSL_CTX_use_certificate_file(ctx, CERTF, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(-2);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, KEYF, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(-3);
    }

    if (!SSL_CTX_check_private_key(ctx)) {
        printf("Private key does not match the certificate public keyn");
        exit(-4);
    }
    printf("SSL 部分初始化完成！\n");

    SockBase::init();

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

    if(r <= 0 && SSL_get_verify_result(ssl) == X509_V_OK){
        auto err = SSL_get_error(ssl, r);
        printf("验证失败！(%d),(%d: %s)\n",err, errno, strerror(errno));
        exit(-1);
    }else{
        printf("验证成功\n");
    }
}

SockClient::~SockClient() {
    SSL_CTX_free(ctx);
}
