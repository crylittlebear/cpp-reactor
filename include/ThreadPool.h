#pragma once

#include <thread>
#include <vector>
#include <unordered_set>
#include <assert.h>
#include <mutex>
#include <condition_variable>

class WorkerThread;
class EventLoop;

// class Semaphore {
// public:
//     Semaphore(int resSize) {
//         resLimit_ = resSize;
//     }
//     ~Semaphore() {}

//     void wait() {
//         std::unique_lock<std::mutex> locker(mutex_);
//         cond_.wait(locker, [&] () {
//             return resLimit_ > 0;
//         });
//         --resLimit_;
//     }
//     void post() {
//         std::unique_lock<std::mutex> locker(mutex_);
//         ++resLimit_;
//     }
// private:
//     int resLimit_;
//     std::mutex mutex_;
//     std::condition_variable cond_;
// };

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
    // 线程任务结束，重新放入备选队列中
    
    void setThreadBack(WorkerThread* thread);
private:
    bool isRuning_;
    int threadSize_;
    int threadIndex_;
    EventLoop* mainLoop_;
    std::vector<WorkerThread*> threads_;
    std::unordered_set<int> busyThreadSet_;
    // Semaphore sem_;
};