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
    HttpResponse();
    ~HttpResponse();

    void setStatusCode(int code);
    void setFileName(std::string fileName);
    void addResponseHeader(std::string header);
    void prepareMsg(Buffer* buf, int socket);

public:
    std::function<void(std::string, Buffer*, int)> sendDataFunc_;

private:
    int statusCode_;
    std::string fileName_;
    std::vector<std::string> responseHeaders_;
    const std::unordered_map<int, std::string> statusMap_ = {
        {OK, "OK"},
        {MovedPermanently, "MovedPermanently"},
        {MovedTemporarily, "MovedTemporarily"},
        {BadRequest, "BadRequest"},
        {NotFound, "NotFound"}
    };
};