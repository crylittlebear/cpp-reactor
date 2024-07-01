#pragma once

#include <string>
#include <stdint.h>
#include <time.h>

class TimeStamp {
public:
    TimeStamp(int64_t epoch);

    static TimeStamp now();
    std::string toString();
private:
    int64_t epoch_;
};