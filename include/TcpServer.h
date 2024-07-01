#pragma once

#include <sys/socket.h>

class ThreadPool;
class EventLoop;

class TcpServer {
public:
    TcpServer(int threadNum, unsigned short port);
    ~TcpServer();

    // 初始化监听
    void setListen();
    // 开启服务器
    void start();
    // 接收新连接
    void acceptNewConnection();

private:
    // 线程数量
    int threadNum_;
    // 线程池
    ThreadPool* pool_;
    // 主反应堆
    EventLoop* mainLoop_;
    // 监听套接字
    int listenFd_;
    // 端口号
    unsigned short port_;
};