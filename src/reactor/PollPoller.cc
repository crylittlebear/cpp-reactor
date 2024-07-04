#include "PollPoller.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Logger.h"

const int PollFdSize = 1024;

PollPoller::PollPoller() {
    maxfd_ = -1;
    fds_ = new pollfd[PollFdSize];
    for (int i = 0; i < PollFdSize; ++i) {
        fds_[i].fd = -1;
        fds_[i].events = 0;
        fds_[i].revents = 0;
    }
}

PollPoller::~PollPoller() {
    if (fds_ != nullptr) {
        delete [] fds_;
    }
}

int PollPoller::add(Channel* channel) {
    int events = 0;
    if (channel->events() & ReadEvent) {
        events |= POLLIN;
    }   
    if (channel->events() & WriteEvent) {
        events |= POLLOUT;
    }
    for (int i = 0; i < PollFdSize; ++i) {
        if (fds_[i].fd == -1) {
            fds_[i].fd = channel->fd();
            fds_[i].events = events;
            maxfd_ = channel->fd() > maxfd_ ? channel->fd() : maxfd_;
            break;
        }
    }
    return 0;
}

int PollPoller::remove(Channel* channel) {
    for (int i = 0; i <= maxfd_; ++i) {
        if (fds_[i].fd == channel->fd()) {
            fds_[i].fd = -1;
            fds_[i].events = 0;
            fds_[i].revents = 0;
            break;
        }
    }
    return 0;
}

int PollPoller::modify(Channel* channel) {
    for (int i = 0; i <= maxfd_; ++i) {
        if (fds_[i].fd == channel->fd()) {
            if (channel->events() & ReadEvent) {
                fds_[i].events |= POLLIN;
            } else {
                fds_[i].events &= ~POLLIN;
            }
            if (channel->events() & WriteEvent) {
                fds_[i].events |= POLLOUT;
            } else {
                fds_[i].events &= ~POLLOUT;
            }
            break;
        }
    }
    return 0;
}

int PollPoller::dispatch(EventLoop* evLoop, int timeout) {
    int num = poll(fds_, PollFdSize, timeout);
    if (num < 0) {
        LOG_ERROR("SelcetPoller::loop() error");
        return -1;
    }
    for (int i = 0; i < PollFdSize; ++i) {
        if (fds_[i].fd == -1) {
            continue;
        }
        if (fds_[i].revents & POLLIN) {
            evLoop->handleEvent(fds_[i].fd, ReadEvent);
        }
        if (fds_[i].revents & POLLOUT) {
            evLoop->handleEvent(fds_[i].fd, WriteEvent);
        }
    }
    return 0;
}
