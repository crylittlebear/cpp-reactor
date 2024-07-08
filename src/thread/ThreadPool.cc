#include "ThreadPool.h"
#include "WorkerThread.h"
#include "EventLoop.h"
#include "Logger.h"

#include <iostream>
#include <chrono>

ThreadPool::ThreadPool(EventLoop* mainLoop, int size) {
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
    LOG_DEBUG("ThreadPoll::start(), 准备开启线程池");
    assert(!isRuning_);
    if (mainLoop_->threadId_ != std::this_thread::get_id()) {
        LOG_FATAL("func = %s, 线程池的运行线程与主线程不符", __FUNCTION__);
    }
    isRuning_ = true;
    for (int i = 0; i < threadSize_; ++i) {
        WorkerThread* worker = new WorkerThread(i);
        worker->run();
        threads_.push_back(worker);
        LOG_DEBUG("ThreadPoll::start(), 成功创建工作线程%s", worker->getThreadName().c_str());
    }
    LOG_DEBUG("ThreadPoll::start(), 线程池已开启...");
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
    LOG_DEBUG("ThreadPoll::takeWorkerThread(), 准备从线程池中获取线程");
    assert(isRuning_);
    WorkerThread* worker = nullptr;
    if (threadSize_ > 0) {
        for (int i = 0; i < threadSize_; ++i) {
            if (busyThreadSet_.find(i) == busyThreadSet_.end()) {
                worker = threads_[i];
                busyThreadSet_.insert(i);
                break;
            }
        }
    }
    if (worker != nullptr) {
        LOG_DEBUG("ThreadPoll::takeWorkerThread(), 成功获取工作线程: %s", worker->getThreadName().c_str());
    } else {
        LOG_DEBUG("ThreadPoll::takeWorkerThread(), 从线程池中获取线程失败");
    }
    return worker;
}

void ThreadPool::setThreadBack(WorkerThread* thread) {
    LOG_DEBUG("------------------------------------------------------------");
    LOG_DEBUG("ThreadPool::setThreadBack(), 准备归还线程");
    for (int i = 0; i < threadSize_; ++i) {
        if (threads_[i] == thread) {
            busyThreadSet_.erase(i);
            LOG_DEBUG("ThreadPool::setThreadBack(), 成功将线程 %s 归还给线程池", thread->getThreadName().c_str());
            LOG_DEBUG("------------------------------------------------------------");
            break;
        }
    }
    if (busyThreadSet_.empty()) {
        std::cout << "CurBusyThread: empty!!!!" << std::endl;
    } else {
        for (auto c : busyThreadSet_) {
            std::cout << "CurBusyThread: ";
            std::cout << c << " ";
            std::cout << std::endl;
        }
    }
}