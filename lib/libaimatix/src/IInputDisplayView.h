#pragma once
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
class IInputDisplayView {
public:
    virtual ~IInputDisplayView() {}
    virtual void showTitle(const char* title, int batteryLevel, bool isCharging) = 0;
    virtual void showHints(const char* btnA, const char* btnB, const char* btnC) = 0;
    virtual void showPreview(const char* preview) = 0;
    virtual void clear() = 0;
    virtual void showDigit(int index, int value, bool entered) = 0;
    virtual void showColon() = 0;
}; 