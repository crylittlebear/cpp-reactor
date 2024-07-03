#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Buffer.h"
#include "Logger.h"

HttpRequest::HttpRequest() {
    parseState_ = PARSE_REQ_LINE;
}

HttpRequest::~HttpRequest() {}

int HttpRequest::hexToDec(char c) {
    if ( c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return 0;
}

void HttpRequest::addHeader(const std::string& key,
                            const std::string& value) {
    if (reqHeaders_.find(key) == reqHeaders_.end()) {
        reqHeaders_[key] = value;
    }
}

const std::string HttpRequest::getHeader(const std::string& key) {
    if (reqHeaders_.find(key) != reqHeaders_.end()) {
        return reqHeaders_[key];
    }
    return "";
}

bool HttpRequest::parseRequestLine(Buffer* buf) {
    LOG_INFO("func = %s", __FUNCTION__);
    LOG_DEBUG("HttpRequest::parseRequestLine()");
    assert(parseState_ == PARSE_REQ_LINE);
    // 从buf中获取与一个HTTP行
    std::string reqLine = buf->retriveHttpLine();
    LOG_DEBUG("接收到的HTTP请求行为: %s", reqLine.c_str());
    int begin = 0;
    auto space = reqLine.find_first_of(' ', begin);
    assert(space != reqLine.npos);
    method_ = reqLine.substr(begin, space - begin);
    begin = space + 1;
    space = reqLine.find_first_of(' ', begin);
    assert(space != reqLine.npos);
    url_ = reqLine.substr(begin, space - begin);
    begin = space + 1;
    version_ = reqLine.substr(begin, reqLine.size() - begin);
    parseState_ = PARSE_REQ_HEADER;
    return true;
}

bool HttpRequest::parseRequestHeader(Buffer* buf) {
    LOG_INFO("func = %s", __FUNCTION__);
    LOG_DEBUG("HttpRequest::parseRequestHeader()");
    std::string header;
    while ((header = buf->retriveHttpLine()) != "") {
        auto pos = header.find(": ", 0);
        assert(pos != header.npos);
        std::string key = header.substr(0, pos);
        std::string value = header.substr(pos + 2, header.size() - pos - 2);
        // LOG_DEBUG("header: key = %s, value = %s", key.c_str(), value.c_str());
        addHeader(key, value);
    } 
    parseState_ = PARSE_REQ_DONE;
    return true;
}

bool HttpRequest::parseHttpRequest(Buffer* readBuf, 
                                   HttpResponse* response,
                                   Buffer* writeBuf,
                                   int socket) {
    LOG_INFO("func = %s", __FUNCTION__);
    LOG_DEBUG("HttpRequest::parseHttpRequest()");
    assert(parseState_ == PARSE_REQ_LINE);
    bool flag = false;
    while (parseState_ != PARSE_REQ_DONE) {
        switch(parseState_) {
        case PARSE_REQ_LINE:
            flag = parseRequestLine(readBuf);
            break;
        case PARSE_REQ_HEADER:
            flag = parseRequestHeader(readBuf);
            break;
        case PARSE_REQ_BODY:
        case PARSE_REQ_DONE:
            break;;
        }
    }
    if (!flag) {
        return flag;
    }
    assert(parseState_ == PARSE_REQ_DONE);
    processHttpRequest(response);
    response->prepareMsg(writeBuf, socket);
    return true;
}

bool HttpRequest::processHttpRequest(HttpResponse* response) {
    LOG_INFO("func = %s", __FUNCTION__);
    LOG_DEBUG("HttpRequest::processHttpRequest()");
    assert(method_ == "get" || method_ == "GET");
    // 解码URL,防止中文乱码
    decodeMsg(url_);
    std::string file;
    if (url_ == "/")
        file = "./";
    else 
        file = url_.substr(1, url_.size() - 1);
    // 获取文件属性
    struct stat st;
    int ret = stat(file.c_str(), &st);
    if (ret == -1) {
        // 文件不存在
        auto sendFunc = std::bind(&HttpRequest::sendFile, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        response->fillResponseMembers("404.html", NotFound, getFileType("404.html"), 
            sendFunc);
        return true;
    }
    // 文件存在
    if (S_ISDIR(st.st_mode)) {
        // 是文件夹
        auto sendFunc = std::bind(&HttpRequest::sendDir, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        response->fillResponseMembers(file, OK, getFileType(".html"), sendFunc);
    } else {
        // 是文件
        auto sendFunc = std::bind(&HttpRequest::sendFile, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        response->fillResponseMembers(file, OK, getFileType(file), sendFunc);
        response->addResponseHeader("Content-length", std::to_string(st.st_size));
    }
    return true;
}

void HttpRequest::decodeMsg(std::string& msg) {
    int index = 0;
    for (int i = 0; i < msg.size(); ++i) {
        if (msg[i] == '%') {
            msg[index++] = hexToDec(msg[i + 1]) * 16 + hexToDec(msg[i + 2]);
            i += 2;
        } else {
            msg[index++] = msg[i];
        }
    }
    msg.resize(index);
}

const std::string HttpRequest::getFileType(const std::string name) {
    const char* dot = strrchr(name.data(), '.');
    if (dot == NULL)
        return "text/plain; charset=utf-8";	// 纯文本
    if (strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0)
        return "text/html; charset=utf-8";
    if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0)
        return "image/jpeg";
    if (strcmp(dot, ".gif") == 0)
        return "image/gif";
    if (strcmp(dot, ".png") == 0)
        return "image/png";
    if (strcmp(dot, ".css") == 0)
        return "text/css";
    if (strcmp(dot, ".au") == 0)
        return "audio/basic";
    if (strcmp(dot, ".wav") == 0)
        return "audio/wav";
    if (strcmp(dot, ".avi") == 0)
        return "video/x-msvideo";
    if (strcmp(dot, ".mov") == 0 || strcmp(dot, ".qt") == 0)
        return "video/quicktime";
    if (strcmp(dot, ".mpeg") == 0 || strcmp(dot, ".mpe") == 0)
        return "video/mpeg";
    if (strcmp(dot, ".vrml") == 0 || strcmp(dot, ".wrl") == 0)
        return "model/vrml";
    if (strcmp(dot, ".midi") == 0 || strcmp(dot, ".mid") == 0)
        return "audio/midi";
    if (strcmp(dot, ".mp3") == 0)
        return "audio/mpeg";
    if (strcmp(dot, ".ogg") == 0)
        return "application/ogg";
    if (strcmp(dot, ".pac") == 0)
        return "application/x-ns-proxy-autoconfig";

    return "text/plain; charset=utf-8";
}

void HttpRequest::sendFile(std::string fileName, Buffer* sendBuf, int socket) {
    LOG_DEBUG("HttpRequest::sendFile()");
    // 1. 打开文件
    int fd = open(fileName.data(), O_RDONLY);
    assert(fd > 0);
#if 1
    while (1) {
        char* buf = new char[4096];
        int len = read(fd, buf, 4096);
        if (len > 0) {
            // send(cfd, buf, len, 0);
            sendBuf->append(buf, len);
#ifndef MSG_SEND_AUTO
            sendBuf->writeToFd(socket);
            delete buf;
#endif
        } else if (len == 0) {
            break;
        } else {
            LOG_ERROR("func = %s, 读取文件失败", __FUNCTION__);
            close(fd);
            return;
        }
    }
#else
    off_t offset = 0;
    int size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    while (offset < size) {
        int ret = sendfile(cfd, fd, &offset, size - offset);
        printf("ret value: %d\n", ret);
        if (ret == -1 && errno == EAGAIN) {
            printf("没数据...\n");
        }
    }
#endif
    close(fd);
}
void HttpRequest::sendDir(std::string dirName, Buffer* sendBuf, int socket) {
    LOG_DEBUG("HttpRequest::sendDir()");
	char buf[4096] = {0};
	sprintf(buf, "<html><head><title>%s</title></head><body><table>", dirName.c_str());
    for (const auto& entry : std::filesystem::directory_iterator(dirName)) {
        auto name = entry.path().filename();
        if (entry.is_directory()) {
            sprintf(buf + strlen(buf), 
				 "<tr><td><a href=\"%s/\">%s</a></td><td>%d</td></tr>", 
				 name.c_str(), name.c_str(), 0);
        } else {
			sprintf(buf + strlen(buf), 
				 "<tr><td><a href=\"%s\">%s</a></td><td>%ld</td></tr>", 
				 name.c_str(), name.c_str(), entry.file_size());            
        }
        sendBuf->append(buf);
        sendBuf->writeToFd(socket);
        memset(buf, 0, sizeof(buf));
    }
    sprintf(buf, "</table></body></html>");
    sendBuf->append(buf);
    sendBuf->writeToFd(socket);
}

void HttpRequest::setMethod(const std::string& method) { method_ = method; }

void HttpRequest::setUrl(const std::string& url) { url_ = url; }

void HttpRequest::setVersion(const std::string& version) { version_ = version; }

ParseState HttpRequest::getState() const { return parseState_; }