#pragma once

#include "Poller.h"

#include <poll.h>

class EventLoop;
class Channel;

class PollPoller : public Poller {
public:
    PollPoller();
    ~PollPoller();

    virtual int add(Channel* channel) override;

    virtual int remove(Channel* channel) override;

    virtual int modify(Channel* channel) override;

    virtual int dispatch(EventLoop* evLoop, int timeout);

private:
    int maxfd_;
    pollfd* fds_;
};