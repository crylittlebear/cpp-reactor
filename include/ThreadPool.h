#pragma once

#include <thread>
#include <vector>
#include <assert.h>

class WorkerThread;
class EventLoop;

class ThreadPool {
public:
    ThreadPool(EventLoop* mainLoop, int size);
    ~ThreadPool();

    // 开启线程池
    void start();
    /**
     * 从线程池中抽取一个线程并返回其反应堆模型
     * 用于向该线程提交任务
    */
    EventLoop* takeWorkerEventLoop();
    // 从线程池中取出一个线程
    WorkerThread* takeWorkerThread();
private:
    bool isRuning_;
    int threadSize_;
    int threadIndex_;
    EventLoop* mainLoop_;
    std::vector<WorkerThread*> threads_;
};