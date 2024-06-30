#pragma once

#include <sys/epoll.h>

#include "Poller/Poller.h"

struct EpollData {
    EpollData() : epfd_(-1), evs_(nullptr) {}
    int epfd_;
    struct epoll_event* evs_;
};

class Channel;
class EventLoop;

class EpollPoller : public Poller {
public:
    EpollPoller();
    virtual ~EpollPoller();

    int epollCtl(Channel* channel, int op);

    virtual int add(Channel* channel) override;
    virtual int remove(Channel* channel) override;
    virtual int modify(Channel* channel) override;
    virtual int poll(EventLoop* evLoop, int timeout) override;

private:
    EpollData* data_;
};