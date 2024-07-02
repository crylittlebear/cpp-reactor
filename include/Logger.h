#pragma once

#include <string>
#include <iostream>

#ifdef MINFO
#define LOG_INFO(logMsgFormat, ...) \
    do { \
        Logger& logger = Logger::instance(); \
        logger.setLogLevel(INFO); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, logMsgFormat, ##__VA_ARGS__); \
        logger.log(buf); \
    } while (0)
#else
#define LOG_INFO(logMsgFormat, ...)
#endif

#define LOG_FATAL(logMsgFormat, ...) \
    do { \
        Logger& logger = Logger::instance(); \
        logger.setLogLevel(FATAL); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, logMsgFormat, ##__VA_ARGS__); \
        logger.log(buf); \
        exit(-1); \
    } while (0)

#define LOG_ERROR(logMsgFormat, ...) \
    do { \
        Logger& logger = Logger::instance(); \
        logger.setLogLevel(ERROR); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, logMsgFormat, ##__VA_ARGS__); \
        logger.log(buf); \
    } while (0)

#ifdef MDEBUG
    #define LOG_DEBUG(logMsgFormat, ...) \
    do { \
        Logger& logger = Logger::instance(); \
        logger.setLogLevel(DEBUG); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, logMsgFormat, ##__VA_ARGS__); \
        logger.log(buf); \
    } while (0)
#else
    #define LOG_DEBUG(logMsgFormat, ...)
#endif 
    

enum LogLevel { INFO, FATAL, ERROR, DEBUG};

class Logger {
public:
    static Logger& instance();

    void setLogLevel(int level);

    void log(const std::string& msg);
private:
    Logger();

private:
    int logLevel_;
};