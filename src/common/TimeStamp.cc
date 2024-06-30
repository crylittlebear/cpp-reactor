#include "TimeStamp.h"

TimeStamp::TimeStamp(int64_t epoch) : epoch_(epoch) {}

TimeStamp TimeStamp::now() {
    time_t tt = time(NULL);
    return TimeStamp(tt);
}

std::string TimeStamp::toString() {
    struct tm t = *(localtime(&epoch_));
    char buf[128];
    sprintf(buf, "%4d-%02d-%02d %02d:%02d:%02d", 
        t.tm_year + 1900,
        t.tm_mon + 1,
        t.tm_mday,
        (t.tm_hour + 8) %  24,
        t.tm_min,
        t.tm_sec);
    return buf;
}