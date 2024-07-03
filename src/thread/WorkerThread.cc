#include "WorkerThread.h"
#include "EventLoop.h"
#include "Logger.h"

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
    loop_ = new EventLoop(threadName_);
    LOG_DEBUG("线程: %s 开始运行...",  threadName_.c_str());
    loop_->loop();
}

void WorkerThread::run() { 
    thread_ = new std::thread(&WorkerThread::workFunc, this);
}

EventLoop* WorkerThread::getEventLoop() const { return loop_; }

std::string WorkerThread::getThreadName() const { return threadName_; }