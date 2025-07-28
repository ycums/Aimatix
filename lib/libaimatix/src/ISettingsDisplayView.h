#pragma once
#include <vector>
#include <string>
#include <cstddef>

class ISettingsDisplayView {
public:
    virtual ~ISettingsDisplayView() {}
    virtual void showTitle(const char* title, int batteryLevel, bool isCharging) = 0;
    virtual void showHints(const char* btnA, const char* btnB, const char* btnC) = 0;
    virtual void showSettingsList(const std::vector<std::string>& items, size_t selectedIndex) = 0;
    virtual void clear() = 0;
}; 