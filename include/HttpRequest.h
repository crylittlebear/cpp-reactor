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

    void addHeader(const std::string& key, const std::string& value);
    const std::string getHeader(const std::string& key);
    bool parseRequestLine(Buffer* buf);
    bool parseRequestHeader(Buffer* buf);
    bool parseHttpRequest(Buffer* buf, 
                          HttpResponse* response, 
                          Buffer* writeBuf, 
                          int socket);
    bool processHttpRequest(HttpResponse* response);
    void decodeMsg(std::string& msg);
    const std::string getFileType(const std::string name);
    void sendFile(std::string fileName, Buffer* buf, int socket);
    void sendDir(std::string dirName, Buffer* buf, int socket);
    
    // setters and getters
    void setMethod(const std::string& method);
    void setUrl(const std::string& url);
    void setVersion(const std::string& version);
    ParseState getState() const;

private:
    int hexToDec(char c);

private:
    ParseState parseState_;
    Channel* channel_;
    std::string method_;
    std::string url_;
    std::string version_;
    std::unordered_map<std::string, std::string> reqHeaders_;
};