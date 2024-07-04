#include "ThreadPool.h"
#include "WorkerThread.h"
#include "EventLoop.h"
#include "Logger.h"

ThreadPool::ThreadPool(EventLoop* mainLoop, int size)
    : sem_(size) {
    threadSize_ = size;
    threadIndex_ = 0;
    isRuning_ = false;
    mainLoop_ = mainLoop;
}

ThreadPool::~ThreadPool() {
    isRuning_ = false;
    for (int i = 0; i < threadSize_; ++i) {
        delete threads_[i];
    }
}

void ThreadPool::start() {
    LOG_DEBUG("ThreadPool::start(), 开启线程池");
    assert(!isRuning_);
    if (mainLoop_->threadId_ != std::this_thread::get_id()) {
        LOG_FATAL("func = %s, 线程池的运行线程与主线程不符", __FUNCTION__);
    }
    isRuning_ = true;
    for (int i = 0; i < threadSize_; ++i) {
        WorkerThread* worker = new WorkerThread(i);
        worker->run();
        threads_.push_back(worker);
    }
}

EventLoop* ThreadPool::takeWorkerEventLoop() {
    assert(isRuning_);
    if (mainLoop_->threadId_ != std::this_thread::get_id()) {
        LOG_FATAL("func = %s, 线程池的运行线程与主线程不符", __FUNCTION__);
    }
    EventLoop* evLoop = mainLoop_;
    if (threadSize_ > 0) {
        evLoop = threads_[threadIndex_]->getEventLoop();
        threadIndex_ = ++threadIndex_ % threadSize_;
    }
    LOG_DEBUG("从线程池中取出线程: %s 的EventLoop", evLoop->threadName_.c_str());
    return evLoop; 
}

WorkerThread* ThreadPool::takeWorkerThread() {
    assert(isRuning_);
    WorkerThread* worker = nullptr;
    if (threadSize_ > 0) {
        sem_.wait();
        for (int i = 0; i < threadSize_; ++i) {
            if (busyThreadSet_.find(i) == busyThreadSet_.end()) {
                worker = threads_[i];
                busyThreadSet_.insert(i);
                break;
            }
        }
    }
    if (worker != nullptr) {
        LOG_DEBUG("从线程池中取出线程: %s", worker->getEventLoop()->threadName_.c_str());
    }
    return worker;
}

void ThreadPool::setThreadBack(WorkerThread* thread) {
    for (int i = 0; i < threadSize_; ++i) {
        if (threads_[i] == thread) {
            busyThreadSet_.erase(i);
            sem_.post();
            LOG_DEBUG("将线程: %s 归还给线程池", threads_[i]->getEventLoop()->threadName_.c_str());
            break;
        }
    }
}