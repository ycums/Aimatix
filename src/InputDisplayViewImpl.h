#pragma once
#include "IInputDisplayView.h"
#include "main_display.h"
#ifdef ARDUINO
#include <M5Stack.h>
#endif

class InputDisplayViewImpl : public IInputDisplayView {
public:
    void showTitle(const char* title, int batteryLevel, bool isCharging) override {
#ifdef ARDUINO
        drawTitleBar(title, batteryLevel, isCharging);
#endif
    }
    void showValue(int value) override {
#ifdef ARDUINO
        // 値部分のみ再描画
        M5.Lcd.fillRect(SCREEN_WIDTH/2-40, SCREEN_HEIGHT/2-30, 80, 60, TFT_BLACK);
        M5.Lcd.setTextFont(FONT_MAIN);
        M5.Lcd.setTextColor(AMBER_COLOR, TFT_BLACK);
        M5.Lcd.setTextDatum(MC_DATUM);
        char buf[16];
        snprintf(buf, sizeof(buf), "%02d", value);
        M5.Lcd.drawString(buf, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
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