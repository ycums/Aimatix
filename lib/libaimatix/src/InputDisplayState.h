#pragma once
#include "StateManager.h"
#include "InputLogic.h"
#include "DisplayCommon.h"
#ifdef ARDUINO
#include <M5Stack.h>
#endif
#include <stdio.h>

class InputDisplayState : public IState {
public:
    InputDisplayState(InputLogic* logic = nullptr) : inputLogic(logic), lastValue(-1) {}
    void onEnter() override {
#ifdef ARDUINO
        M5.Lcd.fillScreen(TFT_BLACK);
        drawCommonTitleBar("INPUT", 42, false);
        drawCommonButtonHints("OK", "", "CANCEL");
        lastValue = -1;
#endif
    }
    void onExit() override {}
    void onDraw() override {
#ifdef ARDUINO
        int value = inputLogic ? inputLogic->getValue() : 0;
        if (value != lastValue) {
            // 値部分のみ再描画
            M5.Lcd.fillRect(SCREEN_WIDTH/2-40, SCREEN_HEIGHT/2-30, 80, 60, TFT_BLACK);
            M5.Lcd.setTextFont(FONT_MAIN);
            M5.Lcd.setTextColor(AMBER_COLOR, TFT_BLACK);
            M5.Lcd.setTextDatum(MC_DATUM);
            char buf[16];
            snprintf(buf, sizeof(buf), "%02d", value);
            M5.Lcd.drawString(buf, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
            lastValue = value;
        }
#else
        int value = inputLogic ? inputLogic->getValue() : 0;
        printf("[InputDisplay] value=%d\n", value);
#endif
    }
    void onButtonA() override {}
    void onButtonB() override {}
    void onButtonC() override {}
    void onButtonALongPress() override {}
    void onButtonCLongPress() override {}
private:
    InputLogic* inputLogic;
    int lastValue;
}; 