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
    void showValue(const int* digits, const bool* entered) override {
        int digitW = 32;
        int colonW = 16;
        int y0 = SCREEN_HEIGHT/2 - 24; // 数字のY座標（仮）
        int underlineY = GRID_Y(6); // アンダーラインのY座標を固定
        int x0 = SCREEN_WIDTH/2 - (2*digitW + colonW/2);
        // 左2桁
        for (int i = 0; i < 2; ++i) {
            int x = x0 + i * digitW + digitW/2;
            if (entered[i]) {
                char d[2];
                d[0] = static_cast<char>('0' + (digits[i] % 10));
                d[1] = '\0';
                disp->setTextColor(AMBER_COLOR, TFT_BLACK);
                disp->drawText(x, y0, d, FONT_IMPORTANT);
            } else {
                disp->drawLine(x - digitW/2 + 4, underlineY, x + digitW/2 - 4, underlineY, AMBER_COLOR);
            }
        }
        // コロン
        
        int colonX = x0 + 2 * digitW + colonW/2;
        disp->setTextColor(AMBER_COLOR, TFT_BLACK);
        disp->drawText(colonX, y0, ":", FONT_IMPORTANT);
        // 右2桁
        for (int i = 2; i < 4; ++i) {
            int x = x0 + 2 * digitW + colonW + (i-2) * digitW + digitW/2;
            if (entered[i]) {
                char d[2];
                d[0] = static_cast<char>('0' + (digits[i] % 10));
                d[1] = '\0';
                disp->setTextColor(AMBER_COLOR, TFT_BLACK);
                disp->drawText(x, y0, d, FONT_IMPORTANT);
            } else {
                disp->drawLine(x - digitW/2 + 4, underlineY, x + digitW/2 - 4, underlineY, AMBER_COLOR);
            }
        }
    }
    void showDigit(int index, int value, bool entered) override {
        int digitW = 32;
        int colonW = 16;
        int fontH = 48;
        int y0 = GRID_Y(4) + GRID_HEIGHT/2;
        int underlineY = GRID_Y(6);
        int x0 = SCREEN_WIDTH/2 - (2*digitW + colonW/2);
        int x;
        if (index < 2) {
            x = x0 + index * digitW + digitW/2;
        } else {
            x = x0 + 2 * digitW + colonW + (index-2) * digitW + digitW/2;
        }
        if (entered) {
            char d[2];
            d[0] = static_cast<char>('0' + (value % 10));
            d[1] = '\0';
            disp->setTextColor(AMBER_COLOR, TFT_BLACK);
            disp->drawText(x, y0, d, FONT_IMPORTANT);
        } else {
            disp->fillRect(x - digitW/2, y0 - fontH/2, digitW, fontH, TFT_BLACK);
            disp->drawLine(x - digitW/2 + 4, underlineY, x + digitW/2 - 4, underlineY, AMBER_COLOR);
        }
    }
    void showHints(const char* btnA, const char* btnB, const char* btnC) override {
        drawButtonHintsGrid(disp, btnA, btnB, btnC);
        drawGridLines(disp);
    }
    void showPreview(const char* preview) override {
        // プレビュー欄（下部中央、Font2、アンバー）
        int y = GRID_Y(7);
        disp->setTextFont(FONT_AUXILIARY);
        disp->setTextColor(AMBER_COLOR, TFT_BLACK);
        disp->setTextDatum(MC_DATUM);
        disp->fillRect(SCREEN_WIDTH/2-80, y-12, 160, 24, TFT_BLACK); // クリア
        if (preview && preview[0]) {
            disp->drawText(SCREEN_WIDTH/2, y, preview, FONT_AUXILIARY);
        }
    }
    void showColon() override {
        int fontH = 48;
        int y0 = GRID_Y(4) + GRID_HEIGHT/2;
        int x0 = SCREEN_WIDTH/2;
        int prevDatum = disp->getTextDatum();
        disp->setTextDatum(MC_DATUM);
        disp->setTextColor(AMBER_COLOR, TFT_BLACK);
        disp->drawText(x0, y0, ":", FONT_IMPORTANT);
        disp->setTextDatum(prevDatum);
    }
    void clear() override {
        disp->clear();
    }
private:
    IDisplay* disp;
}; 