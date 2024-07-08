#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <filesystem>

enum ParseState {
    PARSE_REQ_LINE,     // 解析请求行
    PARSE_REQ_HEADER,   // 解析请求头
    PARSE_REQ_BODY,     // 解析消息体
    PARSE_REQ_DONE      // 解析完毕
};

class Buffer;
class Channel;
class HttpResponse;

class HttpRequest {
public:
    HttpRequest(Channel* channel);
    ~HttpRequest();

    // 将从套接字接收来的Http请求消息中的请求头保存到HttpRequest
    void addHeader(const std::string& key, const std::string& value);
    // 通过key找到相应的Http请求头的值
    const std::string getHeader(const std::string& key);
    // 解析请求行
    bool parseRequestLine(Buffer* buf);
    // 解析请求头
    bool parseRequestHeader(Buffer* buf);
    // 解析Http请求
    bool parseHttpRequest(Buffer* buf, 
                          HttpResponse* response, 
                          Buffer* writeBuf, 
                          int socket);
    // 处理解析完成的Http请求
    bool processHttpRequest(HttpResponse* response);
    // 对中文字符进行解码
    void decodeMsg(std::string& msg);
    // 从文件名得出相应的文件类型
    const std::string getFileType(const std::string name);
    // 发送文件
    void sendFile(std::string fileName, Buffer* buf, Channel* channel);
    // 发送目录
    void sendDir(std::string dirName, Buffer* buf, Channel* channel);
    
    // setters and getters
    void setMethod(const std::string& method);
    void setUrl(const std::string& url);
    void setVersion(const std::string& version);
    ParseState getState() const;

private:
    // 将十六进制字符转换成十进制数字
    int hexToDec(char c);

private:
    // 解析状态
    ParseState parseState_;
    // Http请求对应的Channel
    Channel* channel_;
    // 请求行中的请求方式
    std::string method_;
    // 请求行中的请求资源路径
    std::string url_;
    // 请求行中的Http版本
    std::string version_;
    // Http请求中的请求头
    std::unordered_map<std::string, std::string> reqHeaders_;
};