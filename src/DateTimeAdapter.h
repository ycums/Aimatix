#pragma once
#include "../lib/libaimatix/src/ITimeProvider.h"
#include <ctime>
#include <sys/time.h>

// M5Stack/ESP32用の日時アダプター
class DateTimeAdapter : public ITimeProvider {
private:
    time_t baseTime = 0;
    unsigned long baseMillis = 0;
    
public:
    time_t now() const override { 
        time_t current = time(nullptr);
        
        // システム時刻が0の場合は、millis()ベースの時刻を使用
        if (current == 0) {
            unsigned long currentMillis = millis();
            time_t estimatedTime = baseTime + (currentMillis - baseMillis) / 1000;
            return estimatedTime;
        }
        
        return current;
    }
    
    struct tm* localtime(time_t* t) const override { return ::localtime(t); }
    
    bool setSystemTime(time_t time) override {
        struct timeval tv = {};
        tv.tv_sec = time;
        tv.tv_usec = 0;
        
        // ESP32でシステム時刻を設定
        bool result = settimeofday(&tv, nullptr) == 0;
        
        if (result) {
            baseTime = time;
            baseMillis = millis();
        }
        
        return result;
    }
    
    // 起動時の初期化用
    void initializeTime() {
        baseTime = 0;
        baseMillis = millis();
    }
};