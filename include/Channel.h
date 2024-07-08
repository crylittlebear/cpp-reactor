#pragma once

#include <functional>

enum EventType { 
    Timeout, 
    ReadEvent = 0x01, 
    WriteEvent = 0x02,
    ErrorEvent = 0x04
};

class Channel {
public:
    using Callback = std::function<void()>;

    Channel(int fd, int events, Callback rc, Callback wc, Callback dc);
    ~Channel() = default;

    // 开启写事件检测
    void writeEnable(bool flag = true);
    // 写事件检测是否开启
    bool isWriteEnable() const;
    // getters
    int fd() const { return fd_; }
    int events() const { return events_; }

public:
    Callback readCallback_;
    Callback writeCallback_;
    Callback destroyCallback_;

private:
    int fd_;
    int events_;
};