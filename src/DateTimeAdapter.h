#pragma once
#include "../lib/libaimatix/src/ITimeProvider.h"
#include <ctime>
#include <sys/time.h>

// M5Stack/ESP32用の日時アダプター
class DateTimeAdapter : public ITimeProvider {
public:
    time_t now() const override { return time(nullptr); }
    struct tm* localtime(time_t* t) const override { return ::localtime(t); }
    
    bool setSystemTime(time_t time) override {
        struct timeval tv = {};
        tv.tv_sec = time;
        tv.tv_usec = 0;
        
        // ESP32でシステム時刻を設定
        return settimeofday(&tv, nullptr) == 0;
    }
};