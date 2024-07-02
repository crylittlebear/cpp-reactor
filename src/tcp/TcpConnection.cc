#include "TcpConnection.h"
#include "EventLoop.h"
#include "Buffer.h"
#include "Channel.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Logger.h"

#include <functional>

const int InitBufSize = 10240;

TcpConnection::TcpConnection(int fd, EventLoop* evLoop) {
    loop_ = evLoop;
    readBuf_ = new Buffer(InitBufSize);
    writeBuf_ = new Buffer(InitBufSize);
    request_ = new HttpRequest;
    response_ = new HttpResponse;
    name_ = "Connection-" + std::to_string(fd);
    auto readCallback = std::bind(&TcpConnection::processRead, this);
    auto writeCallback = std::bind(&TcpConnection::processWrite, this);
    auto destroyCallback = std::bind(&TcpConnection::processDestroy, this);
    channel_ = new Channel(fd, ReadEvent, readCallback, writeCallback
        , destroyCallback);
    loop_->addTask(channel_, TYPE_ADD);
}

TcpConnection::~TcpConnection() {
    if (readBuf_ && readBuf_->readableSise() == 0 && writeBuf_  
                 && writeBuf_->readableSise() == 0) {
        delete readBuf_;
        delete writeBuf_;
        delete request_;
        delete response_;
        loop_->freeChannel(channel_);
    }
    LOG_DEBUG("Tcp连接: %s 已断开", name_.c_str());
}

void TcpConnection::processRead() {
    LOG_DEBUG("TcpConnection::processRead(), threadName: %s", loop_->threadName_.c_str());
    int sock = channel_->fd();
    int len = readBuf_->readFromFd(sock);
    // LOG_DEBUG("接收到的HTTP请求消息为: %s", readBuf_->readPtr());
    if (len > 0) {
    #ifdef MSG_SEND_AUTO
        channel_->writeEnable(true);
        loop_->addTask(channel_, TYPE_MOD);
    #endif
        bool flag = request_->parseHttpRequest(readBuf_, 
            response_, writeBuf_, sock);
        if (!flag) {
            // 解析失败，发送一个简单的HTML
            LOG_DEBUG("解析HTTP请求失败");
            std::string msg = "HTTP/1.1 400 Bad Request\r\n\r\n";
            writeBuf_->append(msg);
        }
    } else {
    #ifdef MSG_SEND_AUTO
        loop_->addTask(channel_, TYPE_DEL);
    #endif
    }
#ifndef MSG_SEND_AUTO
    // 断开连接
    loop_->addTask(channel_, TYPE_DEL);
#endif 
}

void TcpConnection::processWrite() {
    // LOG_DEBUG("开始发送数据了...");
    int len = writeBuf_->writeToFd(channel_->fd());
    if (len > 0) {
        // 判断数据是否全部发送
        if (writeBuf_->readableSise() == 0) {
            channel_->writeEnable(false);
            loop_->addTask(channel_, TYPE_MOD);
            loop_->addTask(channel_, TYPE_DEL);
        }
    }
}

void TcpConnection::processDestroy() {
    this->~TcpConnection();
}