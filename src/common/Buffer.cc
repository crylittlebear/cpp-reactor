#include "Buffer.h"
#include "Logger.h"
#include "Channel.h"

#include <sys/socket.h>

Buffer::Buffer(size_t initSize) {
    data_ = new char[initSize];
    readPos_ = 0;
    writePos_ = 0;
    capacity_ = initSize;
}

Buffer::~Buffer() {
    if (data_ != nullptr) {
        delete[] data_;
        data_ = nullptr;
    }
}

size_t Buffer::readableSise() const {
    assert(writePos_ >= readPos_);   
    return writePos_ - readPos_;
}

size_t Buffer::writableSize() const {
    assert(capacity_ >= writePos_);
    return capacity_ - writePos_;
}

const char* Buffer::readPtr() const { return data_ + readPos_; }

char* Buffer::writePtr() { return data_ + writePos_; }

void Buffer::append(const char* str, size_t len) {
    makeSpace(len);
    memcpy(writePtr(), str, len);
    writePos_ += len;
}

void Buffer::append(const char* str) {
    int len = strlen(str);
    append(str, len);
}

void Buffer::append(const std::string& str) {
    append(str.c_str());
}

void Buffer::append(const Buffer& buf) {
    append(buf.readPtr(), buf.readableSise());
}

void Buffer::makeSpace(size_t size) {
    // 如果可写数据长度大于等于size
    if (writableSize() >= size) { return; }
    // 如果合并后的长度大于等于size
    if (writableSize() + readPos_ >= size) {
         memcpy(data_, readPtr(), readableSise());
         writePos_ = readableSise();
         readPos_ = 0;
         return;
    }
    // 如果剩余的可写空间小于size
    std::size_t newSize = capacity_ + size;
    char* temp = new char[newSize];
    memset(temp, 0, newSize);
    memcpy(temp, data_, writePos_);
    delete[] data_;
    data_ = temp;
    capacity_ = newSize;
}

ssize_t Buffer::readFromFd(int fd) {
    char buf[65536];
    int writable = writableSize();
    struct iovec vec[2];
    vec[0].iov_base = writePtr();
    vec[0].iov_len = writable;
    vec[1].iov_base = buf;
    vec[1].iov_len = 65536;
    // 接收数据
    int len = readv(fd, vec, 2);
    if (len < 0) {
        LOG_ERROR("file=Buffer.cc, line=%d, msg: readv() error!", __LINE__);
    } else if (len <= writable) {
        writePos_ += len;
    } else {
        writePos_ += capacity_;
        append(buf, len - writable);
    }
    return len;
}

// ssize_t Buffer::writeToFd(Channel* channel) {
//     int totalLen = 0;
//     int fd = channel->fd();
//     while (readableSise() > 0) {
//         auto len = send(fd, readPtr(), readableSise(), MSG_NOSIGNAL);
//         if (len > 0) {
//             // LOG_DEBUG("Buffer::writeToFd(), 成功发送 %d 字节的数据", (int)len);
//             readPos_ += len;
//             totalLen += len;
//         } 
//         else {
//             break;
//         }
//     }
//     return totalLen;
// }

ssize_t Buffer::writeToFd(Channel* channel) {
    int totalLen = 0;
    int fd = channel->fd();
    while (readableSise() > 0) {
        auto len = send(fd, readPtr(), readableSise(), MSG_NOSIGNAL);
        if (len > 0) {
            readPos_ += len;
            totalLen += len;
        } 
        else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 套接字不可写，稍后再试
                break;
            } 
            else if (errno == EPIPE || errno == ECONNRESET) {
                // 对端关闭连接
                LOG_ERROR("Connection closed by peer");
                return -1; // 发送失败，返回 -1
            } 
            else {
                // 其他错误
                LOG_ERROR("send failed: %s", strerror(errno));
                return -1; // 发送失败，返回 -1
            }
        }
    }
    return totalLen;
}


std::string Buffer::retriveHttpLine() {
    std::string_view sv(readPtr(), readableSise());
    auto pos = sv.find("\r\n", 0);
    if (pos == sv.npos) { return ""; }
    readPos_ += pos;
    readPos_ += 2;
    return std::string(sv.substr(0, pos));
}

void Buffer::print() {
    LOG_DEBUG("Buffer::print(), 打印HTTP请求信息:");
    std::string_view sv(readPtr(), readableSise());
    std::cout << sv << std::endl;
    LOG_DEBUG("Buffer::print(), 打印HTTP请求信息结束..............");
}
