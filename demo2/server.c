#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    int listenfd;
    listenfd = socket(PF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        printf("error");
    }
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5188);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    listen(listenfd, 20);

    struct sockaddr_in peeraddr;
    socklen_t peerlen = sizeof(peeraddr);
    // conn 已连接套接字
    int conn = accept(listenfd, (struct sockaddr *) &peeraddr, &peerlen);

    char buf[1024];
    while (1) {
        memset(&buf, 0, sizeof(buf));
        read(conn, buf, sizeof(buf));
        fputs(buf, stdout);
        write(conn, buf, strlen(buf));
    }

    return 0;

}