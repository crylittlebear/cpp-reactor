#include "EventLoop.h"
#include "EpollPoller.h"
#include "Channel.h"
#include "Logger.h"

#include <fcntl.h>

EventLoop::EventLoop(const std::string& name) {
    isQuit_ = false;
    threadId_ = std::this_thread::get_id();
    threadName_ = name == ""? "MainThread" : name;
    poller_ = new EpollPoller;
    int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, socketPair_);
    if (ret == -1) {
        // LOG_ERROR
        LOG_ERROR("file=EventLoop.cc, line=%d, msg: EventLoop() error!", 
            __LINE__);
    }
    // 创建用于本地通信的Channel
    Channel* channel = new Channel(socketPair_[1], ReadEvent, 
        std::bind(&EventLoop::readLocalMsg, this), nullptr, nullptr);
    // 设置套接字为非阻塞模式
    int flag = fcntl(socketPair_[1], F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(socketPair_[1], F_SETFL, flag);
    addTask(channel, TYPE_ADD);
}

EventLoop::~EventLoop() {
    isQuit_ = true;
    if (poller_)
        delete poller_;
}

int EventLoop::loop() {
    assert(isQuit_ != true);
    while (!isQuit_) {
        poller_->poll(this, 3);
        processTask();
    }
    return 0;
}

int EventLoop::handleEvent(int fd, int event) {
    if (fd < 0) return -1;
    Channel* channel = channelMap_[fd];
    assert(channel->fd() == fd);
    if ((event & ReadEvent) && channel->readCallback_ != nullptr) {
        channel->readCallback_();
    }
    if ((event & WriteEvent) && channel->writeCallback_ != nullptr) {
        channel->writeCallback_();
    }
    return 0;
}

int EventLoop::addTask(Channel* channel, int type) {
    {
        std::unique_lock<std::mutex> locker(mutex_);
        ChannelElem* elem = new ChannelElem(channel, type);
        taskQue_.push(elem);
    }
    // // 添加任务后需要唤醒响应的线程处理任务
    // if (threadId_ == std::this_thread::get_id()) {
    //     // 添加任务的线程是子线程
    //     processTask();
    // } else {
    //     // 处理任务
    //     taskWakeup();
    // }
    taskWakeup();
    processTask();
    return 0;
}

int EventLoop::processTask() {
    std::unique_lock<std::mutex> locker(mutex_);
    while (!taskQue_.empty()) {
        auto elem = taskQue_.front();
        if (elem->type_ == TYPE_ADD) {
            add(elem->channel_);
        } else if (elem->type_ == TYPE_DEL) {
            remove(elem->channel_);
        } else if (elem->type_ == TYPE_MOD) {
            modify(elem->channel_);
        }
        taskQue_.pop();
        delete elem;
    }
    return 0;
}

int EventLoop::add(Channel* channel) {
    poller_->add(channel);
    channelMap_[channel->fd()] = channel;
    return 0;
}

int EventLoop::remove(Channel* channel) {
    poller_->remove(channel);
    return 0;
}

int EventLoop::modify(Channel* channel) {
    poller_->modify(channel);
    return 0;
}

int EventLoop::freeChannel(Channel* channel) {
    // 从映射表中将channel删除
    channelMap_.erase(channel->fd());
    // 关闭channel中保存的文件描述符
    close(channel->fd());
    // 释放channel内存
    delete channel;
    return 0;
}

const std::unordered_map<int, Channel*>&
EventLoop::channelMap() const {
    return channelMap_;
}

void EventLoop::taskWakeup() {
    const char* msg = "hello world";
    int len = send(socketPair_[0], msg, strlen(msg), 0);
    if (len < 0) {
        // LOG_ERROR
        LOG_ERROR("file=EventLoop.cc, line=%d, msg: taskWakeup() error!", 
            __LINE__);
    }    
}

void EventLoop::readLocalMsg() {
    char buf[64];
    int len = recv(socketPair_[1], buf, sizeof buf, 0);
    if (len < 0) {
        // LOG_ERROR
        LOG_ERROR("file=EventLoop.cc, line=%d, msg: readLocalMsg() error!", 
            __LINE__);
    }
}