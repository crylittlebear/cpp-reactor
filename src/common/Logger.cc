#include "Logger.h"
#include "TimeStamp.h"

Logger::Logger() {}

Logger& Logger::instance() {
    static Logger log;
    return log;
}

void Logger::setLogLevel(int level) {
    logLevel_ = level;
}

void Logger::log(const std::string& msg) {
    std::cout << "(" <<TimeStamp::now().toString() << ")";  
        switch (logLevel_) {
    case INFO:
        std::cout << " [INFO] ";
        break;
    case FATAL:
        std::cout << " [FATAL] ";
        break;
    case ERROR:
        std::cout << " [ERROR] ";
        break;
    case DEBUG:
        std::cout << " [DEBUG] ";
        break;
    }
    std::cout << msg << std::endl;
}