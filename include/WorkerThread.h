#pragma once

#include <thread>
#include <string>
#include <mutex>
#include <condition_variable>

class EventLoop;

class WorkerThread {
public:
    WorkerThread(int index);
    ~WorkerThread();

    void run();
    void workFunc();
    EventLoop* getEventLoop() const;

    std::string getThreadName() const;

private:
    std::thread* thread_;
    EventLoop* loop_;
    std::thread::id threadId_;
    std::string threadName_;
    std::mutex mutex_;
    std::condition_variable cond_;
};