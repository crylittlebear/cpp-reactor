#include "SelectPoller.h"
#include "EventLoop.h"
#include "Channel.h"
#include "Logger.h"

SelectPoller::SelectPoller() {
    maxfd_ = -1;
    FD_ZERO(&readSet_);
    FD_ZERO(&writeSet_);
}

SelectPoller::~SelectPoller() {}

int SelectPoller::add(Channel* channel) {
    int fd = channel->fd();
    FD_CLR(fd, &readSet_);
    FD_CLR(fd, &writeSet_);
    if (channel->events() & ReadEvent) {
        FD_SET(fd, &readSet_);
    }
    if (channel->events() & WriteEvent) {
        FD_SET(fd, &writeSet_);
    }
    maxfd_ = fd > maxfd_ ? fd : maxfd_;
    return 0;
}

int SelectPoller::remove(Channel* channel) {
    int fd = channel->fd();
    if (channel->events() & ReadEvent) {
        FD_CLR(fd, &readSet_);
    }
    if (channel->events() & WriteEvent) {
        FD_CLR(fd, &writeSet_);
    }
    return 0;
}

int SelectPoller::modify(Channel* channel) {
    int fd = channel->fd();
    if (channel->events() & ReadEvent) {
        FD_SET(fd, &readSet_);
    } else {
        FD_CLR(fd, &readSet_);
    }
    if (channel->events() & WriteEvent) {
        FD_SET(fd, &writeSet_);
    } else {
        FD_CLR(fd, &writeSet_);
    }
    return 0;
}

int SelectPoller::dispatch(EventLoop* evLoop, int timeout) {
    timeval tv;
    tv.tv_sec = timeout;
    tv.tv_usec = 0;
    fd_set tempReadSet = readSet_;
    fd_set tempWriteSet = writeSet_;
    int num = select(maxfd_ + 1, &tempReadSet, &tempWriteSet, 
        nullptr, &tv);
    if (num < 0) {
        LOG_ERROR("SelcetPoller::loop() error");
        return -1;
    }
    for (int i = 0; i <= maxfd_; ++i) {
        if (FD_ISSET(i, &tempReadSet)) {
            evLoop->handleEvent(i, ReadEvent);
        }
        if (FD_ISSET(i, &tempWriteSet)) {
            evLoop->handleEvent(i, WriteEvent);
        }
    }
    return 0;
}