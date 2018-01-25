#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>

#define ERR_EXIT(m) \
    do \
    { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while(0)

int main() {
    printf("welcome to thomas's socket \n");
    char *host = "127.0.0.1";
    // char *host = "wcc.push.com";  // host file: 127.0.0.1 wcc.push.com
    int port = 1234;
    //创建套接字
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    //向服务器（特定的IP和端口）发起请求
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用'\0'填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址

    unsigned long inaddr = inet_addr(host);  //具体的IP地址或者域名
    if (inaddr != INADDR_NONE) {
        memcpy(&serv_addr.sin_addr, &inaddr, sizeof(inaddr));
    } else {
        struct hostent *hp = gethostbyname(host);
        memcpy(&serv_addr.sin_addr, hp->h_addr, hp->h_length);
    }

    serv_addr.sin_port = htons(port);  //端口
    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        ERR_EXIT("connect");
    }

    char read_buf[1024] = {0};
    char send_buf[1024] = {0};

    while (fgets(send_buf, sizeof(send_buf), stdin) != NULL) {
        write(sock, send_buf, strlen(send_buf));
        read(sock, read_buf, sizeof(read_buf));
        fputs(read_buf, stdout);
        memset(read_buf, 0, sizeof(read_buf));
        memset(send_buf, 0, sizeof(send_buf));
    }

    //关闭套接字
    close(sock);
    return 0;
}
