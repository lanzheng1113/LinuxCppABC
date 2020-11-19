#include <stdio.h>
#include <iostream>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

using namespace std;

#define PORT 7838
#define MAXBUF 1024

int main()
{
    cout << "Hello SSL!" << endl;
    //
    // 下面这段是初始化工作。
    //
    ERR_load_ERR_strings();
    OpenSSL_add_ssl_algorithms();
    SSL_CTX *ctx = SSL_CTX_new (SSLv23_server_method());
    if (!ctx)
    {
        cerr << "FSSL_CTX_new Failed." << endl;
        return 1;
    }

    //SSL_CTX_set_verify(ctx,SSL_VERIFY_PEER,NULL); //验证与否
    //SSL_CTX_load_verify_locations(ctx,CACERT,NULL); //若验证,则放置CA证书

    // 载入用户的数字证书， 此证书用来发送给客户端。 证书里包含有公钥
    if (SSL_CTX_use_certificate_file(ctx, "/home/duocore/work/openssl/server.crt", SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stdout);
        return 3;
    }

    // 载入服务器的私钥
    if (SSL_CTX_use_PrivateKey_file(ctx, "/home/duocore/work/openssl/server.key", SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stdout);
        return 4;
    }

    /* 检查服务器的私钥是否正确 */
    if (!SSL_CTX_check_private_key(ctx))
    {
        printf("Private key does not match the certificate public key\n");
        ERR_print_errors_fp(stdout);
        return 5;
    }

    // SSL_CTX_set_cipher_list(ctx,"RC4-MD5");

    //
    // 初始化完成
    //
    const int INVALID_SOCKET = -1;
    int sockfd = socket (AF_INET, SOCK_STREAM, 0);
    if(sockfd == INVALID_SOCKET)
    {
        perror("socket");
        return 6;
    }

    //创建地址
    sockaddr_in sin = {0};
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons (PORT);

    //套接字绑定
    int err = bind(sockfd, (struct sockaddr*) &sin,sizeof(sin));
    if(err == -1)
    {
        cout << "监听套接字绑定失败" << endl;
        perror("bind");
        return 7;
    }

    //接受客户端连接
    const int lisnum = 2;
    if (listen(sockfd, lisnum) == -1)
    {
        perror("listen");
        return 8;
    } else
        printf("SSL Server is listening on 127.0.0.1:%d\n", PORT);

    while (1)
    {
        socklen_t len = sizeof(struct sockaddr);
        /* 等待客户端连上来，这儿是DEMO，只能一个一个来，实际应用这里要开线程池。*/
        int new_fd = -1;
        struct sockaddr_in their_addr;
        if ((new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &len)) == -1)
        {
            perror("accept");
            exit(errno);
        }
        else
            printf("server: got connection from %s, port %d, socket %d\n", inet_ntoa(their_addr.sin_addr), ntohs(their_addr.sin_port), new_fd);

        /* 基于ctx产生一个新的SSL */
        SSL *ssl = SSL_new(ctx);
        /* 将连接用户的socket加入到SSL */
        SSL_set_fd(ssl, new_fd);
        /* 建立SSL连接 */
        if (SSL_accept(ssl) == -1)
        {
            perror("accept");
            close(new_fd);
            break;
        }
        /* 开始处理每个新连接上的数据收发 */
        char buf[MAXBUF + 1];
        bzero(buf, MAXBUF + 1);
        strcpy(buf, "server->client");
        /* 发消息给客户端 */
        len = SSL_write(ssl, buf, strlen(buf));
        if (len <= 0)
        {
            printf("消息'%s'发送失败！错误代码是%d，错误信息是'%s'\n", buf, errno, strerror(errno));
            goto finish;
        } else
            printf("消息'%s'发送成功，共发送了%d个字节！\n", buf, len);

        bzero(buf, MAXBUF + 1);
        /* 接收客户端的消息 */
        len = SSL_read(ssl, buf, MAXBUF);
        if (len > 0)
            printf("接收消息成功:'%s'，共%d个字节的数据\n", buf, len);
        else
            printf("消息接收失败！错误代码是%d，错误信息是'%s'\n", errno, strerror(errno));
        /* 处理每个新连接上的数据收发结束 */
      finish:
        /* 关闭 SSL 连接 */
        SSL_shutdown(ssl);
        /* 释放 SSL */
        SSL_free(ssl);
        /* 关闭 socket */
        close(new_fd);
    }

    /* 关闭监听的 socket */
    close(sockfd);
    /* 释放 CTX */
    SSL_CTX_free(ctx);

    return 0;
}
