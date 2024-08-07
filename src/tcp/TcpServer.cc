#include "TcpServer.h"
#include "TcpConnection.h"
#include "ThreadPool.h"
#include "EventLoop.h"
#include "Logger.h"
#include "Channel.h"
#include "WorkerThread.h"

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
    // 设置监听套接字非阻塞
    int flag = fcntl(listenFd_, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(listenFd_, F_SETFL, flag);
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
    // 接收新连接
    LOG_DEBUG("===================================================");
    LOG_DEBUG("                新连接分界线");
    LOG_DEBUG("===================================================");
    LOG_DEBUG("TcpServer::acceptNewConnection(), 有新的TCP连接到达");
    int cfd = accept(listenFd_, nullptr, nullptr);
    if (cfd == -1) {
        LOG_ERROR("func: %s, 接受新连接失败", __FUNCTION__);
    }

    // 设置套接字为非阻塞模式
    int flag = fcntl(cfd, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(cfd, F_SETFL, flag);
    LOG_DEBUG("TcpServer::acceptNewConnection(), 线程名: %s, 套接字: %d", 
        mainLoop_->threadName_.c_str(), cfd);
    
    // 将这个新连接放到线程池中的线程上运行
    WorkerThread* worker = pool_->takeWorkerThread();
    // TO DO(当线程满的时候可以重新开辟线程处理连接任务)
    assert(worker != nullptr);
    if (worker->connections_.find(cfd) == worker->connections_.end()) {
        // 如果工作线程中没有保存cfd相应的TCP链接
        (worker->connections_)[cfd] = 
            new TcpConnection(cfd, worker->getEventLoop(), worker, pool_);
        LOG_DEBUG("TcpServer::acceptNewConnection(), 成功创建新的TCP连接, 线程名 %s, 套接字 %d", 
            worker->getEventLoop()->threadName_.c_str(), cfd);
    }
}

void TcpServer::start() {
    LOG_DEBUG("TcpServer::start(), 准备启动TCP服务器");
    pool_->start();
    // 初始化一个Channel用于保存监听套接字
    auto func = std::bind(&TcpServer::acceptNewConnection, this);
    Channel* channel = new Channel(listenFd_, ReadEvent, func,
        nullptr, nullptr);
    mainLoop_->addTask(channel, TYPE_ADD);
    LOG_DEBUG("TcpServer::start(), 开启服务器事件循环");
    mainLoop_->loop();
}