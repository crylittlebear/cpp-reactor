#pragma once

#include <unordered_map>
#include <queue>
#include <thread>
#include <string>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <mutex>
#include <sys/socket.h>

enum Type{ TYPE_ADD, TYPE_DEL, TYPE_MOD };

class Channel;
class Poller;

struct ChannelElem {
    ChannelElem(Channel* channel, int type) 
        : channel_(channel), type_(type) {}

    Channel* channel_;
    int type_;
};

class EventLoop {
public:
    EventLoop(const std::string& name = "");
    ~EventLoop();

    // 唤醒线程处理相应任务
    void taskWakeup();
    // 读取本地数据
    void readLocalMsg();
    // 启动反应堆模型
    int loop();
    // 处理fd上的事件
    int handleEvent(int fd, int event);
    // 向任务队列中添加任务
    int addTask(Channel* channel, int type);
    // 处理任务队列中的任务
    int processTask();
    // 处理Channel上的读写事件
    int add(Channel* channel);
    int remove(Channel* channel);
    int modify(Channel* channel);
    // getters
    std::unordered_map<int, Channel*>& channelMap();

public:
    // poller
    Poller* poller_;
    // 运行状态
    bool isQuit_;
    // 文件描述符-channel映射表
    std::unordered_map<int, Channel*> channelMap_;
    // 任务队列
    std::queue<ChannelElem*> taskQue_;
    // 队列访问互斥锁
    std::mutex mutex_;
    // 所在线程的线程ID
    std::thread::id threadId_;
    // 所在线程名
    std::string threadName_;
    // 存储本地通信套接字
    int socketPair_[2];
};