#pragma once

class Channel;
class EventLoop;

class Poller {
public:
    virtual ~Poller() {}
    virtual int add(Channel* channel) = 0;
    virtual int remove(Channel* channel) = 0;
    virtual int modify(Channel* channel) = 0;
    virtual int dispatch(EventLoop* evLoop, int timeout) = 0;
};