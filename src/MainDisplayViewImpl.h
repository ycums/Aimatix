#pragma once
#include "IMainDisplayView.h"
#include "main_display.h"
#ifdef ARDUINO
#include <M5Stack.h>
#endif

class MainDisplayViewImpl : public IMainDisplayView {
public:
    void showTitle(const char* modeName, int batteryLevel, bool isCharging) override {
#ifdef ARDUINO
        drawTitleBar(modeName, batteryLevel, isCharging);
#endif
    }
    void showTime(const char* currentTime) override {
#ifdef ARDUINO
        // 仮実装: 時刻表示エリアのみ再描画
        M5.Lcd.fillRect(SCREEN_WIDTH/2-40, 60, 80, 40, TFT_BLACK);
        M5.Lcd.setTextFont(FONT_MAIN);
        M5.Lcd.setTextColor(AMBER_COLOR, TFT_BLACK);
        M5.Lcd.setTextDatum(MC_DATUM);
        M5.Lcd.drawString(currentTime, SCREEN_WIDTH/2, 80);
#endif
    }
    void showProgress(int percent) override {
#ifdef ARDUINO
        // 仮実装: 進捗バー描画
        fillProgressBar(40, 140, 240, 20, percent);
#endif
    }
    void showHints(const char* btnA, const char* btnB, const char* btnC) override {
#ifdef ARDUINO
        drawButtonHintsGrid(btnA, btnB, btnC);
#endif
    }
    void clear() override {
#ifdef ARDUINO
        M5.Lcd.fillScreen(TFT_BLACK);
#endif
    }
}; 