#pragma once

#include <sys/epoll.h>

#include "Poller.h"

class Channel;
class EventLoop;

class EpollPoller : public Poller {
public:
    EpollPoller();
    virtual ~EpollPoller();

    // epoll中add/remove/modify的操作函数
    int epollCtl(Channel* channel, int op);
    // 将channel中的fd添加到epoll树中
    virtual int add(Channel* channel) override;
    // 将channel中的fd从epoll树中删除
    virtual int remove(Channel* channel) override;
    // 修改channel中的fd在epoll树中监听的事件
    virtual int modify(Channel* channel) override;
    // 检测epoll树中的就绪的事件并处理
    virtual int dispatch(EventLoop* evLoop, int timeout) override;

private:
    int epfd_;
    epoll_event* evs_;
};