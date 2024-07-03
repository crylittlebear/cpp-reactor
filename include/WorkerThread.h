#pragma once

#include <thread>
#include <string>
#include <mutex>
#include <unordered_map>
#include <condition_variable>

class EventLoop;
class TcpConnection;

class WorkerThread {
public:
    WorkerThread(int index);
    ~WorkerThread();

    void run();
    void workFunc();
    EventLoop* getEventLoop() const;
    std::string getThreadName() const;

public:
    std::unordered_map<int, TcpConnection*> connections_;

private:
    EventLoop* loop_;
    std::thread* thread_;
    std::thread::id threadId_;
    std::string threadName_;
    std::mutex mutex_;
    std::condition_variable cond_;
};