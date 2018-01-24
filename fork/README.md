### 一，简介
    回射程序, 服务端采用fork子进程来处理客户端的请求

### 二，使用方法

#### 1. 编译

    cd fork/
    cmake .
    make


#### 2. 启动服务端

    ./server

#### 3. 启动客户端

    ./client
    
### 三, 涉及的知识点

#### 1.僵尸进程的处理
    
    客户端退出，处理这个客户端连接的服务端子进程也会消亡，等待父进程读取，这时候子进程就是僵尸进程，子进程消亡，系统会发送信号给父进程，父进程接到信号就用waitpid处理子进程，但是信号是不可靠的，用WNOHANG,并循环接收
    
#### 2.地址重复利用

    当服务端先关闭的时候，服务端会出现FIN_WAIT2然后是TIME_WAIT状态，这个时候，服务端再次bind这个端口就会出现EADDRINUSE错误，解决办法是setsockopt设置套接字地址重复利用:

```apple js
int on = 1;
if (setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
    ERR_EXIT("setsockopt");
}
查看客户端，服务器所处的状态：netstat -nat
```