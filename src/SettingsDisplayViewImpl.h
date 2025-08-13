#pragma once
#include "ISettingsDisplayView.h"
#include "DisplayCommon.h"
#include "IDisplay.h"
#include <vector>
#include <string>
#include <cstddef>

class SettingsDisplayViewImpl : public ISettingsDisplayView {
public:
    SettingsDisplayViewImpl(IDisplay* disp) : disp(disp) {}
    
    void showTitle(const char* title, int batteryLevel, bool isCharging) override {
        drawTitleBar(disp, title, batteryLevel, isCharging);
    }
    
    void showHints(const char* btnA, const char* btnB, const char* btnC) override {
        drawButtonHintsGrid(disp, btnA, btnB, btnC);
    }
    
    void showSettingsList(const std::vector<std::string>& items, size_t selectedIndex) override;
    void clear() override {
        if (disp) disp->clear();
    }
    
private:
    IDisplay* disp;
}; 