#pragma once
#include "IInputDisplayView.h"
#include "DisplayCommon.h"
#include "IDisplay.h"
#include "InputLogic.h"

class InputDisplayViewImpl : public IInputDisplayView {
public:
    InputDisplayViewImpl(IDisplay* disp) : disp(disp) {}
    void showTitle(const char* title, int batteryLevel, bool isCharging) override {
        drawTitleBar(disp, title, batteryLevel, isCharging);
    }
    void showValue(int value) override {
        disp->fillRect(SCREEN_WIDTH/2-60, SCREEN_HEIGHT/2-30, 120, 60, TFT_BLACK);
        disp->setTextFont(FONT_IMPORTANT);
        disp->setTextDatum(MC_DATUM);
        if (value == InputLogic::EMPTY_VALUE) {
            // __:__ を下線で描画
            int x0 = SCREEN_WIDTH/2 - 48;
            int y0 = SCREEN_HEIGHT/2 + 18;
            int digitW = 24;
            int colonW = 12;
            int underlineY = y0 + 8;
            // 左2桁
            for (int i = 0; i < 2; ++i) {
                int x = x0 + i * digitW;
                disp->drawRect(x, underlineY, digitW-4, 3, AMBER_COLOR);
            }
            // コロン
            int colonX = x0 + 2 * digitW;
            disp->drawText(colonX + colonW/2, y0, ":", FONT_IMPORTANT);
            // 右2桁
            for (int i = 0; i < 2; ++i) {
                int x = x0 + 2 * digitW + colonW + i * digitW;
                disp->drawRect(x, underlineY, digitW-4, 3, AMBER_COLOR);
            }
        } else {
            // 2桁ずつ分解して描画（例: 12:34）
            int x0 = SCREEN_WIDTH/2 - 48;
            int y0 = SCREEN_HEIGHT/2;
            int digitW = 24;
            int colonW = 12;
            char buf[5];
            snprintf(buf, sizeof(buf), "%04d", value);
            // 左2桁
            for (int i = 0; i < 2; ++i) {
                int x = x0 + i * digitW + digitW/2;
                char d[2] = {buf[i], 0};
                disp->setTextColor(AMBER_COLOR, TFT_BLACK);
                disp->drawText(x, y0, d, FONT_IMPORTANT);
            }
            // コロン
            int colonX = x0 + 2 * digitW + colonW/2;
            disp->setTextColor(AMBER_COLOR, TFT_BLACK);
            disp->drawText(colonX, y0, ":", FONT_IMPORTANT);
            // 右2桁
            for (int i = 2; i < 4; ++i) {
                int x = x0 + 2 * digitW + colonW + (i-2) * digitW + digitW/2;
                char d[2] = {buf[i], 0};
                disp->setTextColor(AMBER_COLOR, TFT_BLACK);
                disp->drawText(x, y0, d, FONT_IMPORTANT);
            }
        }
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