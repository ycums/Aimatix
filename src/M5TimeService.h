#pragma once
#include "../lib/libaimatix/src/ITimeService.h"
#include <Arduino.h>
#include <sys/time.h>

// Arduino/M5Stack implementation of ITimeService
class M5TimeService : public ITimeService {
public:
    time_t now() const override { return ::time(nullptr); }
    struct tm* localtime(time_t* t) const override { return ::localtime(t); }
    bool setSystemTime(time_t t) override {
        struct timeval tv = {};
        tv.tv_sec = t;
        tv.tv_usec = 0;
        return ::settimeofday(&tv, nullptr) == 0;
    }
    uint32_t monotonicMillis() const override { return static_cast<uint32_t>(::millis()); }
};


