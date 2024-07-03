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

class HttpResponse {
public:
    using SendFunc = std::function<void(std::string, Buffer*, int)>;

    HttpResponse();
    ~HttpResponse();

    void setStatusCode(int code);
    void setFileName(const std::string& fileName);
    void addResponseHeader(const std::string& key, 
                           const std::string& value);
    void prepareMsg(Buffer* buf, int socket);
    void fillResponseMembers(const std::string& fileName,
                             int statusCode,
                             const std::string& fileType,
                             SendFunc func);

public:
    SendFunc sendDataFunc_;

private:
    int statusCode_;
    std::string fileName_;
    std::unordered_map<std::string, std::string> responseHeaders_;
    const std::unordered_map<int, std::string> statusMap_ = {
        {OK, "OK"},
        {MovedPermanently, "MovedPermanently"},
        {MovedTemporarily, "MovedTemporarily"},
        {BadRequest, "BadRequest"},
        {NotFound, "NotFound"}
    };
};