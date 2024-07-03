#include "HttpResponse.h"
#include "Buffer.h"
#include "Logger.h"

HttpResponse::HttpResponse() {}

HttpResponse::~HttpResponse() {}

void HttpResponse::setStatusCode(int code) {
    statusCode_ = code;
}

void HttpResponse::setFileName(const std::string& fileName) {
    fileName_ = fileName;
}

void HttpResponse::addResponseHeader(const std::string& key,
                                     const std::string& value) {
    responseHeaders_[key] = value;
}

void HttpResponse::prepareMsg(Buffer* buf, int socket) {
    LOG_INFO("func = %s", __FUNCTION__);
    LOG_DEBUG("HttpResponse::prepareMsg()");
    // 添加响应行
    std::string responseLine = "HTTP/1.1 ";
    responseLine += std::to_string(statusCode_);
    responseLine += " ";
    responseLine += statusMap_.at(statusCode_);
    buf->append(responseLine);
    buf->append("\r\n");
    // 添加响应头
    for (auto& header : responseHeaders_) {
        buf->append(header.first);
        buf->append(": ");
        buf->append(header.second);
        buf->append("\r\n");
    }   
    // 添加空行
    buf->append("\r\n");
    //发送数据
    buf->writeToFd(socket);
    // 发送数据
    sendDataFunc_(fileName_, buf, socket);
}

void HttpResponse::fillResponseMembers(const std::string& fileName,
                                       int statusCode,
                                       const std::string& fileType,
                                       SendFunc func) {
    setFileName(fileName);
    setStatusCode(statusCode);
    addResponseHeader("Content-type", fileType);
    sendDataFunc_ = func;
}