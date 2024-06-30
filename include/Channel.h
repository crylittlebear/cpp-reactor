#pragma once

#include <functional>

enum EventType { Timeout, ReadEvent, WriteEvent };

class Channel {
public:
    using Callback = std::function<void()>;

    Channel(int fd, int events, Callback rc, Callback wc, Callback dc);

    void writeEnable(bool flag = true);

    bool isWriteEnable() const;

    int fd() const { return fd_; }
    int events() const {return events_; }

public:
    Callback readCallback_;
    Callback writeCallback_;
    Callback destroyCallback_;

private:
    int fd_;
    int events_;
};