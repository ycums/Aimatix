#pragma once
#include "ITimeProvider.h"

class MockTimeProvider : public ITimeProvider {
public:
    time_t fixedTime;
    bool lastSetSystemTimeResult;
    struct tm* localtimeResult;
    
    MockTimeProvider() : fixedTime(1000), lastSetSystemTimeResult(true), localtimeResult(nullptr) {}
    MockTimeProvider(time_t t) : fixedTime(t), lastSetSystemTimeResult(true), localtimeResult(nullptr) {}
    
    time_t now() const override { return fixedTime; }
    struct tm* localtime(time_t* t) const override { 
        if (localtimeResult != nullptr) {
            return localtimeResult;
        }
        return ::localtime(t); 
    }
    
    bool setSystemTime(time_t time) override {
        fixedTime = time;
        return lastSetSystemTimeResult;
    }
    
    // テスト用のメソッド
    void setSetSystemTimeResult(bool result) { lastSetSystemTimeResult = result; }
    void setLocaltimeResult(struct tm* result) { localtimeResult = result; }
}; 