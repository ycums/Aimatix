#pragma once
#include "IMainDisplayView.h"
#include "DisplayCommon.h"
#include "IDisplay.h"
#include <vector>
#include <string>

class MainDisplayViewImpl : public IMainDisplayView {
public:
    MainDisplayViewImpl(IDisplay* disp) : disp(disp) {}
    void showTitle(const char* modeName, int batteryLevel, bool isCharging) override {
        drawTitleBar(disp, modeName, batteryLevel, isCharging);
    }
    void showTime(const char* currentTime) override {
        disp->setTextColor(AMBER_COLOR, TFT_BLACK);
        disp->setTextDatum(MC_DATUM);
        disp->drawText(SCREEN_WIDTH/2, GRID_Y(2) + GRID_HEIGHT, currentTime, FONT_MAIN);
    }
    void showRemain(const char* remainTime) override {
        disp->setTextDatum(TC_DATUM);
        disp->drawText(SCREEN_WIDTH/2, GRID_Y(4), remainTime, FONT_IMPORTANT);
    }
    void showProgress(int percent) override {
        disp->fillProgressBarSprite(GRID_X(0), GRID_Y(7), SCREEN_WIDTH, 8, percent);
    }
    void showAlarmList(const std::vector<std::string>& alarmStrs) override {
        const int alermColStep = (14 * GRID_WIDTH / 5);
        disp->setTextDatum(MC_DATUM);
        const int clearW = 48;
        const int clearH = 24;
        for (int i = 0; i < 5; ++i) {
            int x = GRID_X(1) + alermColStep /2 + i * alermColStep ;            
            int y = GRID_Y(9);
            if (i < (int)alarmStrs.size()) {
                disp->drawText(x, y, alarmStrs[i].c_str(), FONT_AUXILIARY);
            } else {
                disp->fillRect(x - clearW/2, y - clearH/2, clearW, clearH, TFT_BLACK);
            }
        }
        disp->setTextDatum(TL_DATUM);
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