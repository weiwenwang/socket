#include "common.h"

#define MAXEVENTS 64

void do_service(int connect_sock) {
    char buffer[1024];

    memset(buffer, 0, sizeof(buffer));
    int ret = read(connect_sock, buffer, sizeof(buffer));
    if (ret == 0) { // 客户端关闭
        printf("client close\n");
    }
    if (ret == -1) {
        ERR_EXIT("read");
    }
    fputs(buffer, stdout);
    write(connect_sock, buffer, ret);
 
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

    int epollfd = epoll_create1(0);
    struct epoll_event event;
    struct epoll_event *events;
    event.data.fd = listen_sock;
    event.events = EPOLLIN | EPOLLET; // 注册监听套接字的可读事件
    epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &event);

    events = calloc(MAXEVENTS, sizeof event);

    while (1) {
        int n, i;
        n = epoll_wait(epollfd, events, MAXEVENTS, -1);
        for (i = 0; i < n; i++) {
            if (
                    (events[i].events & EPOLLERR) ||
                    (events[i].events & EPOLLHUP) ||
                    (!(events[i].events & EPOLLIN))
                    ) {
                ERR_EXIT("WANG");

            } else if (listen_sock == events[i].data.fd) { //监听套接字，有事件到来
                int infd;
                if ((infd = accept(listen_sock, (struct sockaddr *) &clnt_addr, &clnt_addr_size)) < 0) {
                    ERR_EXIT("accept");
                } else {
                    printf("ip = %s, port = %d \n", inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));
                    fflush(stdout);
                }
                event.data.fd = infd;
                event.events = EPOLLIN | EPOLLET;
                epoll_ctl(epollfd, EPOLL_CTL_ADD, infd, &event);

            } else { //已连接套接字，有事件到来
                do_service(events[i].data.fd);
            }
        }
    }
}