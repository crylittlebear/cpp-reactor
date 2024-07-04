#pragma once

#include <sys/select.h>

#include "Poller.h"

class EventLoop;
class Channel;

class SelectPoller : public Poller {
public:
    SelectPoller();
    virtual ~SelectPoller();

    virtual int add(Channel* channel) override;

    virtual int remove(Channel* channel) override;

    virtual int modify(Channel* channel) override;

    virtual int poll(EventLoop* evLoop, int timeout);

private:
    int maxfd_;
    fd_set readSet_;
    fd_set writeSet_;
};