#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

enum StatusCode {
    Unknown,
    OK = 200,
    MovedPermanently = 301,
    MovedTemporarily = 302,
    BadRequest = 400,
    NotFound = 404
};

class Buffer;
class Channel;

class HttpResponse {
public:
    using SendFunc = std::function<void(std::string, Buffer*, Channel*)>;

    HttpResponse();
    ~HttpResponse();

    // 设置响应行中的状态码
    void setStatusCode(int code);
    // 设置要发送文件的名称
    void setFileName(const std::string& fileName);
    // 添加响应头
    void addResponseHeader(const std::string& key, 
                           const std::string& value);
    // 准备响应行、响应头及空行信息并发送
    void prepareMsg(Buffer* buf, Channel* channel);
    // 设置响应消息的各个成员
    void fillResponseMembers(const std::string& fileName,
                             int statusCode,
                             const std::string& fileType,
                             SendFunc func);

public:
    // 响应消息中发送消息体的回调函数
    SendFunc sendDataFunc_;

private:
    // 响应行中的状态码
    int statusCode_;
    // 要发送的文件或文件夹的名称
    std::string fileName_;
    // 响应消息中的响应头
    std::unordered_map<std::string, std::string> responseHeaders_;
    // 状态码对应的状态信息
    const std::unordered_map<int, std::string> statusMap_ = {
        {OK, "OK"},
        {MovedPermanently, "MovedPermanently"},
        {MovedTemporarily, "MovedTemporarily"},
        {BadRequest, "BadRequest"},
        {NotFound, "NotFound"}
    };
};