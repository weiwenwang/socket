#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>


#define ERR_EXIT(m) \
    do \
    { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while(0)

void do_service(int connect_sock) {
    char buffer[1024];
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int ret = read(connect_sock, buffer, sizeof(buffer));
        if (ret == 0) { // 客户端关闭
            printf("client close\n");
            break;
        }
        if (ret == -1) {
            ERR_EXIT("read");
        }
        fputs(buffer, stdout);
        write(connect_sock, buffer, ret);
    }
}

void handle_sigchld(int signo) {
    int status;
    while (waitpid(-1, &status, WNOHANG) > 0) {
        printf("子进程退出,exit(%d)", signo);
    }
}

int main() {
    printf("welcome to thomas's socket \n");

    //创建套接字
    int listen_sock;
    if ((listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        ERR_EXIT("socket");
    }

    //将套接字和IP、端口绑定
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    serv_addr.sin_port = htons(1234);  //端口

    // 绑定之前，需要开启地址重复利用(利用TIMEWAIT的未关闭的连接), 解决Address already in use
    int on = 1;
    if (setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        ERR_EXIT("setsockopt");
    }

    if (bind(listen_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        ERR_EXIT("bind");
    }

    //进入监听状态，等待用户发起请求
    if (listen(listen_sock, SOMAXCONN) < 0) {
        ERR_EXIT("listen");
    }
    //接收客户端请求
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int connect_sock;

    signal(SIGCHLD, handle_sigchld);
    pid_t pid;
    while (1) { // 监听套接字循环接受连接
        if ((connect_sock = accept(listen_sock, (struct sockaddr *) &clnt_addr, &clnt_addr_size)) < 0) {
            ERR_EXIT("accept");
        } else {
            printf("ip = %s, port = %d \n", inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));
            fflush(stdout);
        }

        pid = fork();
        if (pid == -1) {
            ERR_EXIT("fork");
        }
        if (pid == 0) { // 子进程 向客户端发送数据
            close(listen_sock); // 关闭监听套接字
            do_service(connect_sock);
            exit(EXIT_SUCCESS);
        } else {
            close(connect_sock); // 父进程 关闭连接套接字
        }
    }

    //关闭套接字
    close(connect_sock);
    close(listen_sock);
    return 0;
}
