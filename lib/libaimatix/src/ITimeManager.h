#pragma once
#include <ctime>

// 時間管理インターフェース
class ITimeManager {
public:
    virtual ~ITimeManager() {}
    virtual unsigned long getCurrentMillis() const = 0;
    virtual time_t getCurrentTime() const = 0;
}; 