#pragma once
class IInputDisplayView {
public:
    virtual ~IInputDisplayView() {}
    virtual void showTitle(const char* title, int batteryLevel, bool isCharging) = 0;
    virtual void showValue(int value) = 0;
    virtual void showHints(const char* btnA, const char* btnB, const char* btnC) = 0;
    virtual void clear() = 0;
}; 