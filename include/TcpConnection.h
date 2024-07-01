#pragma once

#include <string>

class Buffer;
class Channel;
class EventLoop;
class HttpRequest;
class HttpResponse;

class TcpConnection {
public:
    TcpConnection(int fd, EventLoop* evLoop);
    ~TcpConnection();

    void processRead();
    void processWrite();
    void processDestroy();
private:
    std::string name_;
    Buffer* readBuf_;
    Buffer* writeBuf_;
    EventLoop* loop_;
    Channel* channel_;
    // http
    HttpRequest* request_;
    HttpResponse* response_;
};