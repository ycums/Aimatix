#pragma once
#include <vector>
#include <string>
class IMainDisplayView {
public:
    virtual ~IMainDisplayView() {}
    virtual void showTitle(const char* modeName, int batteryLevel, bool isCharging) = 0;
    virtual void showTime(const char* currentTime) = 0;
    virtual void showRemain(const char* remainTime) = 0;
    virtual void showProgress(int percent) = 0;
    virtual void showAlarmList(const std::vector<std::string>& alarmStrs) = 0;
    virtual void showHints(const char* btnA, const char* btnB, const char* btnC) = 0;
    virtual void clear() = 0;
}; 