#include<iostream>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

void do_service(int clnt_sock);

int main() {
    //创建套接字
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    //将套接字和IP、端口绑定
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    serv_addr.sin_port = htons(8888);  //端口
    bind(serv_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    //进入监听状态，等待用户发起请求
    listen(serv_sock, 20);
    //接收客户端请求
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);

    pid_t pid;
    while (1) {
        cout << "我在等你连接" << endl;
        int clnt_sock = accept(serv_sock, (struct sockaddr *) &clnt_addr, &clnt_addr_size);
        pid = fork();
        if (pid == 0) {
            // 关闭监听套接字
            close(serv_sock);
            sleep(10);
            do_service(clnt_sock);
            exit(-6);
        } else {
            // 关闭连接套接字
            close(clnt_sock);
        }
    }
    return 0;
}

void do_service(int clnt_sock) {
    char buf[1024] = {0};
    int read_len = read(clnt_sock, buf, sizeof(buf));
    write(clnt_sock, buf, read_len);
    cout << "关闭" << endl;
    //关闭连接套接字
    close(clnt_sock);
}