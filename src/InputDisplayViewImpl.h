#pragma once
#include "IInputDisplayView.h"
#include "main_display.h"
#include "IDisplay.h"

class InputDisplayViewImpl : public IInputDisplayView {
public:
    InputDisplayViewImpl(IDisplay* disp) : disp(disp) {}
    void showTitle(const char* title, int batteryLevel, bool isCharging) override {
        drawTitleBar(disp, title, batteryLevel, isCharging);
    }
    void showValue(int value) override {
        disp->fillRect(SCREEN_WIDTH/2-40, SCREEN_HEIGHT/2-30, 80, 60, TFT_BLACK);
        disp->setTextFont(FONT_MAIN);
        disp->setTextColor(AMBER_COLOR, TFT_BLACK);
        disp->setTextDatum(MC_DATUM);
        char buf[16];
        snprintf(buf, sizeof(buf), "%02d", value);
        disp->drawText(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, buf, FONT_MAIN);
    }
    void showHints(const char* btnA, const char* btnB, const char* btnC) override {
        drawButtonHintsGrid(disp, btnA, btnB, btnC);
    }
    void clear() override {
        disp->clear();
    }
private:
    IDisplay* disp;
}; 