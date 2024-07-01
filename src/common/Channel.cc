#include "Channel.h"

Channel::Channel(int fd, 
                 int events,  
                 Callback rc,  
                 Callback wc,  
                 Callback dc)
    : fd_(fd)
    , events_(events)
    , readCallback_(rc)
    , writeCallback_(wc)
    , destroyCallback_(dc) {}

void Channel::writeEnable(bool flag) {
    if (flag) {
        events_ |= WriteEvent;
    } else {
        events_ &= ~WriteEvent;
    }
}

bool Channel::isWriteEnable() const {
    return events_ & WriteEvent;
}