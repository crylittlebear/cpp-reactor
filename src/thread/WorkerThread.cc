#include "WorkerThread.h"
#include "EventLoop.h"

WorkerThread::WorkerThread(int index) {
    threadName_ = "SubThread-" + std::to_string(index);
    loop_ = nullptr;
    thread_ = nullptr;
    threadId_ = std::thread::id();
}

WorkerThread::~WorkerThread() {
    if (loop_) {
        delete loop_;
    }
    if (thread_) {
        delete thread_;
    }
}

void WorkerThread::workFunc() {
    {
        std::unique_lock<std::mutex> locker;
        loop_ = new EventLoop(threadName_);
    }
    cond_.notify_all();
    loop_->loop();
}

void WorkerThread::run() {
    thread_ = new std::thread(&WorkerThread::workFunc, this);
    std::unique_lock<std::mutex> locker;
    cond_.wait(locker);
}

EventLoop* WorkerThread::getEventLoop() const { return loop_; }

std::string WorkerThread::getThreadName() const { return threadName_; }