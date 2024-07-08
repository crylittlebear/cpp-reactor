#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Buffer.h"
#include "Channel.h"
#include "Logger.h"

#include <chrono>
#include <sys/socket.h>

using namespace std::chrono;

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

HttpRequest::HttpRequest(Channel* channel) {
    channel_ = channel;
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
    LOG_DEBUG("HttpRequest::parseRequestLine(), 开始解析Http请求行部分");
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
    LOG_DEBUG("HttpRequest::parseRequestHeader(), 开始解析HTTP请求头部分");
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
    LOG_DEBUG("HttpRequest::parseHttpRequest(), 开始解析HTTP请求");
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
    LOG_DEBUG("HttpRequest::parseHttpRequest(), 解析HTTP请求完成");
    processHttpRequest(response);
    response->prepareMsg(writeBuf, channel_);
    return true;
}

bool HttpRequest::processHttpRequest(HttpResponse* response) {
    LOG_DEBUG("HttpRequest::processHttpRequest(), 开始处理Http请求");
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
        LOG_DEBUG("HttpRequest::processHttpRequest(), 文件 %s 不存在", file.c_str());
        // 文件不存在
        auto sendFunc = std::bind(&HttpRequest::sendFile, this, _1, _2, _3);
        response->fillResponseMembers("404.html", NotFound, getFileType("404.html"), 
            sendFunc);
        return true;
    }
    // 文件存在
    if (S_ISDIR(st.st_mode)) {
        LOG_DEBUG("HttpRequest::processHttpRequest(), 请求的内容是目录");
        // 传输的是文件夹
        auto sendFunc = std::bind(&HttpRequest::sendDir, this, _1, _2, _3);
        response->fillResponseMembers(file, OK, getFileType(".html"), sendFunc);
    } else {
        LOG_DEBUG("HttpRequest::processHttpRequest(), 请求的内容是文件, 文件名是: %s",
            file.c_str());
        // 传输的是文件
        auto sendFunc = std::bind(&HttpRequest::sendFile, this, _1, _2, _3);
        response->fillResponseMembers(file, OK, getFileType(file), sendFunc);
        response->addResponseHeader("Content-length", std::to_string(st.st_size));
    }
    LOG_DEBUG("HttpRequest::processHttpRequest(), Http请求处理完毕");
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

void HttpRequest::sendFile(std::string fileName, Buffer* sendBuf, Channel* channel) {
    LOG_DEBUG("HttpRequest::sendFile(), 开始发送文件");
    // 1. 打开文件
    int fd = open(fileName.data(), O_RDONLY);
    assert(fd > 0);
#if 1
    while (1) {
        char recvBuf[1];
        int recvLen = recv(channel->fd(), recvBuf, sizeof recvBuf, MSG_PEEK | MSG_DONTWAIT);
        if (recvLen == 0) {
            LOG_DEBUG("HttpRequest::sendFile(), 对端关闭了连接");
            break;
        } else if (recvLen == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
            LOG_ERROR("recv failed: %s", strerror(errno));
            break;
        }
        char buf[40960];
        int len = read(fd, buf, 40960);
        if (len > 0) {
            sendBuf->append(buf, len);
#ifndef MSG_SEND_AUTO
            int sendLen = sendBuf->writeToFd(channel);
            if (sendLen == -1) {
                break;
            }
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
    LOG_DEBUG("HttpRequest::sendFile(), 文件发送完毕");
}

std::string toFormatTime(time_point<system_clock> tp) {
    // 将时间点转换为时间戳
    std::time_t tt = system_clock::to_time_t(tp);
    std::tm t;
     // 使用 localtime_r 线程安全地将 time_t 转换为 tm 结构
    localtime_r(&tt, &t); 

    // 打印调试信息
    // std::cout << "Debug: std::time_t = " << tt << std::endl;
    // std::cout << "Debug: tm_year = " << t.tm_year + 1900 
    //           << ", tm_mon = " << t.tm_mon + 1 
    //           << ", tm_mday = " << t.tm_mday 
    //           << ", tm_hour = " << t.tm_hour 
    //           << ", tm_min = " << t.tm_min 
    //           << ", tm_sec = " << t.tm_sec << std::endl;

    // 格式化时间为字符串
    char format_time[64];
    std::strftime(format_time, sizeof(format_time), "%Y-%m-%d %H:%M:%S", &t);

    return std::string(format_time);
}

void HttpRequest::sendDir(std::string dirName, Buffer* sendBuf, Channel* channel) {
    LOG_DEBUG("HttpRequest::sendDir(), 开始发送目录信息");
    std::ostringstream html;
    html << "<html><head><title>" << dirName << "</title>"
         << "<style>"
         << "body { font-family: Arial, sans-serif; }"
         << "table { width: 100%; border-collapse: collapse; }"
         << "th, td { padding: 8px; text-align: left; border-bottom: 1px solid #ddd; }"
         << "th { cursor: pointer; }"
         << "tr:hover { background-color: #f5f5f5; }"
         << ".icon { width: 20px; }"
         << "</style>"
         << "<script>"
         << "function sortTable(n) {"
         << "var table, rows, switching, i, x, y, shouldSwitch, dir, switchcount = 0;"
         << "table = document.getElementById('directoryTable');"
         << "switching = true; dir = 'asc';"
         << "while (switching) {"
         << "switching = false; rows = table.rows;"
         << "for (i = 1; i < (rows.length - 1); i++) {"
         << "shouldSwitch = false; x = rows[i].getElementsByTagName('TD')[n];"
         << "y = rows[i + 1].getElementsByTagName('TD')[n];"
         << "if (dir == 'asc') {"
         << "if (x.innerHTML.toLowerCase() > y.innerHTML.toLowerCase()) {"
         << "shouldSwitch = true; break; } }"
         << "else if (dir == 'desc') {"
         << "if (x.innerHTML.toLowerCase() < y.innerHTML.toLowerCase()) {"
         << "shouldSwitch = true; break; } } }"
         << "if (shouldSwitch) {"
         << "rows[i].parentNode.insertBefore(rows[i + 1], rows[i]);"
         << "switching = true; switchcount++; }"
         << "else {"
         << "if (switchcount == 0 && dir == 'asc') {"
         << "dir = 'desc'; switching = true; } } } }"
         << "</script></head><body>"
         << "<h2>Index of " << dirName << "</h2>"
         << "<table id='directoryTable'><thead><tr>"
         << "<th onclick='sortTable(0)'>Name</th>"
         << "<th onclick='sortTable(1)'>Size</th>"
         << "<th onclick='sortTable(2)'>Last Modified</th>"
         << "</tr></thead><tbody>";

    for (const auto& entry : std::filesystem::directory_iterator(dirName)) {
        auto name = entry.path().filename().string();
        auto file_time = entry.last_write_time();
        
        // 转换 file_time 到 system_clock::time_point
        auto sctp = time_point_cast<system_clock::duration>(
            file_time - std::filesystem::file_time_type::clock::now() + system_clock::now());

        std::string lastModTime = toFormatTime(sctp);

        std::string icon = entry.is_directory() ? "📁" : "📄"; // 使用 emoji 图标

        html << "<tr><td><a href=\"" << name << (entry.is_directory() ? "/" : "") << "\">"
             << icon << " " << name << (entry.is_directory() ? "/" : "") << "</a></td><td>";

        if (entry.is_directory()) {
            html << "Directory";
        } else {
            html << entry.file_size();
        }

        html << "</td><td>" << lastModTime << "</td></tr>";
    }

    html << "</tbody></table></body></html>";

    std::string htmlStr = html.str();
    sendBuf->append(htmlStr.c_str(), htmlStr.size());
    sendBuf->writeToFd(channel);
    LOG_DEBUG("HttpRequest::sendDir(), 目录信息发送完毕");
}

void HttpRequest::setMethod(const std::string& method) { method_ = method; }

void HttpRequest::setUrl(const std::string& url) { url_ = url; }

void HttpRequest::setVersion(const std::string& version) { version_ = version; }

ParseState HttpRequest::getState() const { return parseState_; }