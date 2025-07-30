#pragma once
#include "ITimeProvider.h"

class MockTimeProvider : public ITimeProvider {
public:
    time_t fixedTime;
    bool lastSetSystemTimeResult;
    
    MockTimeProvider(time_t t) : fixedTime(t), lastSetSystemTimeResult(true) {}
    
    time_t now() const override { return fixedTime; }
    struct tm* localtime(time_t* t) const override { return ::localtime(t); }
    
    bool setSystemTime(time_t time) override {
        fixedTime = time;
        return lastSetSystemTimeResult;
    }
    
    // テスト用のメソッド
    void setSetSystemTimeResult(bool result) { lastSetSystemTimeResult = result; }
}; 