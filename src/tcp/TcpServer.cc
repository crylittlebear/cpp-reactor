#include "TcpServer.h"
#include "TcpConnection.h"
#include "ThreadPool.h"
#include "EventLoop.h"
#include "Logger.h"
#include "Channel.h"

#include <arpa/inet.h>
#include <functional>
#include <fcntl.h> 

TcpServer::TcpServer(int threadNum, unsigned short port) {
    threadNum_ = threadNum;
    port_ = port;
    mainLoop_ = new EventLoop;
    pool_ = new ThreadPool(mainLoop_, threadNum);
    setListen();
}

TcpServer::~TcpServer() {
    if (mainLoop_) {
        delete mainLoop_;
    }
    if (pool_) {
        delete pool_;
    }
}

void TcpServer::setListen() {
    // 创建监听套接字
    listenFd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd_ == -1) {
        LOG_FATAL("func: %s, 创建套接字失败", __FUNCTION__);
    }
    // 设置端口复用
    int opt = 1;
    int ret = setsockopt(listenFd_, SOL_SOCKET, SO_REUSEADDR, 
        &opt, sizeof opt);
    if (ret == -1) {
        LOG_FATAL("func: %s, 设置端口复用失败", __FUNCTION__);
    }
    // 绑定本地IP和端口
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    addr.sin_addr.s_addr = INADDR_ANY;
    ret = bind(listenFd_, (sockaddr*)&addr, sizeof addr);
    if (ret == -1) {
        LOG_FATAL("func: %s, 绑定本地端口失败", __FUNCTION__);
    }
    ret = listen(listenFd_, 128);
    if (ret == -1) {
        LOG_FATAL("func: %s, 监听端口失败", __FUNCTION__);
    }
}

void TcpServer::acceptNewConnection() {
    LOG_INFO("func: %s", __FUNCTION__);
    int cfd = accept(listenFd_, nullptr, nullptr);
    // 设置套接字为非阻塞模式
    int flag = fcntl(cfd, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(cfd, F_SETFL, flag);
    if (cfd == -1) {
        LOG_ERROR("func: %s, 接受新连接失败", __FUNCTION__);
    }
    // 将这个新连接放到线程池中的线程上运行
    EventLoop* evLoop = pool_->takeWorkerEventLoop();
    new TcpConnection(cfd, evLoop);
}

void TcpServer::start() {
    LOG_INFO("func: %s, 服务器启动...", __FUNCTION__);
    pool_->start();
    // 初始化一个Channel用于保存监听套接字
    auto func = std::bind(&TcpServer::acceptNewConnection, this);
    Channel* channel = new Channel(listenFd_, ReadEvent, func,
        nullptr, nullptr);
    mainLoop_->addTask(channel, TYPE_ADD);
    mainLoop_->loop();
}