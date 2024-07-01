#include "EpollPoller.h"
#include "EventLoop.h"
#include "Channel.h"
#include "Logger.h"

const int EpollEventSize = 1024;

EpollPoller::EpollPoller() {
    data_ = new EpollData;
    data_->epfd_ = epoll_create1(EPOLL_CLOEXEC);
    data_->evs_ = new epoll_event[EpollEventSize];
}

EpollPoller::~EpollPoller() {
    if (data_) {
        if (data_->evs_) {
            delete[] data_->evs_;
        }
        delete data_;
    }
}

int EpollPoller::epollCtl(Channel* channel, int op) {
    epoll_event ev;
    ev.data.fd = channel->fd();
    int events = 0;
    if (channel->events() & ReadEvent) {
        events |= EPOLLIN;
    }
    if (channel->events() & WriteEvent) {
        events |= EPOLLOUT;
    }
    ev.events = events;
    return epoll_ctl(data_->epfd_, op, channel->fd(), &ev);
}

int EpollPoller::add(Channel* channel) {
    int ret = epollCtl(channel, EPOLL_CTL_ADD);
    if (ret == -1) {
        // LOG_ERROR
        LOG_ERROR("file=EpollPoller.cc, line=%d, msg: epoll_ADD() error!", 
            __LINE__);
    }
    return ret;
}

int EpollPoller::remove(Channel* channel) {
    int ret = epollCtl(channel, EPOLL_CTL_DEL);
    if (ret == -1) {
        // LOG_ERROR
        LOG_ERROR("file=EpollPoller.cc, line=%d, msg: epoll_DEL() error!", 
            __LINE__);
    }
    return ret;
}

int EpollPoller::modify(Channel* channel) {
    int ret = epollCtl(channel, EPOLL_CTL_MOD);
    if (ret == -1) {
        // LOG_ERROR
        LOG_ERROR("file=EpollPoller.cc, line=%d, msg: epoll_MOD() error!", 
            __LINE__);
    }
    return 0;
}

int EpollPoller::poll(EventLoop* evLoop, int timeout) {
    int numActive = epoll_wait(data_->epfd_, data_->evs_, 
        EpollEventSize, timeout);
    if (numActive == -1) {
        // LOG_ERROR
        LOG_ERROR("file=EpollPoller.cc, line=%d, msg: epoll_wait() error!", 
            __LINE__);
    } else {
        for (int i = 0; i < numActive; ++i) {
            int fd = data_->evs_[i].data.fd;
            int events = data_->evs_[i].events;
            // EventLoop中保存有文件描述符到Channel的映射表，因此由EventLoop处理
            if (events & EPOLLIN) { 
                evLoop->handleEvent(fd, ReadEvent);
            }
            if (events & EPOLLOUT) {
                evLoop->handleEvent(fd, WriteEvent);
            }
        }
    }
    return 0;
}