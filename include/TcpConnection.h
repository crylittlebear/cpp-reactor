#pragma once

#include <string>

class Buffer;
class Channel;
class EventLoop;
class WorkerThread;
class HttpRequest;
class HttpResponse;
class ThreadPool;

class TcpConnection {
public:
    TcpConnection(int fd, EventLoop* evLoop, WorkerThread* thread, ThreadPool* pool);
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
    WorkerThread* workerThread_;
    ThreadPool* pool_;
    // http
    HttpRequest* request_;
    HttpResponse* response_;
};