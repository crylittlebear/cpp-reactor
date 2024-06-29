#pragma once

#include <vector>
#include <string>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/uio.h>

const size_t initBufSize = 1024;

class Buffer {
public:
    Buffer(size_t initSize = initBufSize);
    ~Buffer();

    // 可写字节数
    size_t writableSize() const;
    // 可读取字节数
    size_t readableSise() const;
    // 读指针
    char* readPtr() const;
    // 写指针
    char* writePtr() const;
    // 写数据
    void append(const char* str, size_t len);
    void append(const char* str);
    void append(const std::string& str);
    void append(const Buffer& buf);
    // 从文件描述符中读数据
    ssize_t readFromFd(int fd);
    // 向文件描述符中写数据
    ssize_t writeToFd(int fd);
private:
    void makeSpace(size_t size);

private:
    char* data_;
    size_t readPos_;
    size_t writePos_;
    size_t capacity_;
};