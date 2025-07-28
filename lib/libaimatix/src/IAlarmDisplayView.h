#pragma once
#include <vector>
#include <ctime>
#include <cstddef>

class IAlarmDisplayView {
public:
    virtual ~IAlarmDisplayView() {}
    virtual void showTitle(const char* title, int batteryLevel, bool isCharging) = 0;
    virtual void showHints(const char* btnA, const char* btnB, const char* btnC) = 0;
    virtual void showAlarmList(const std::vector<time_t>& alarms, size_t selectedIndex) = 0;
    virtual void showNoAlarms() = 0;
    virtual void clear() = 0;
}; 