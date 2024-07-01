#include "HttpResponse.h"
#include "Buffer.h"
#include "Logger.h"

void HttpResponse::setStatusCode(int code) {
    statusCode_ = code;
}

void HttpResponse::setFileName(std::string fileName) {
    fileName_ = fileName;
}

void HttpResponse::addResponseHeader(std::string header) {
    responseHeaders_.push_back(header);
}

void HttpResponse::prepareMsg(Buffer* buf, int socket) {
    std::string responseLine = "HTTP/1.1 ";
    responseLine = std::to_string(statusCode_) + " ";
    responseLine += statusMap_.at(statusCode_);
    buf->append(responseLine);
    buf->append("\r\n");
    for (int i = 0; i < responseHeaders_.size(); ++i) {
        buf->append(responseHeaders_[i]);
        buf->append("\r\n");
    }   
    // 添加空行
    buf->append("\r\n");
    // 发送数据
    sendDataFunc_(fileName_, buf, socket);
}