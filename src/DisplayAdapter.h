#pragma once
#include "IDisplay.h"
#ifdef ARDUINO
#include <M5Stack.h>
#endif

class DisplayAdapter : public IDisplay {
public:
    void clear() override {
#ifdef ARDUINO
        M5.Lcd.fillScreen(TFT_BLACK);
#endif
    }
    void drawText(int x, int y, const char* text, int fontSize) override {
#ifdef ARDUINO
        M5.Lcd.setTextFont(fontSize);
        M5.Lcd.drawString(text, x, y);
#endif
    }
    void setTextColor(uint32_t color, uint32_t bgColor) override {
#ifdef ARDUINO
        M5.Lcd.setTextColor(color, bgColor);
#endif
    }
    void fillRect(int x, int y, int w, int h, uint32_t color) override {
#ifdef ARDUINO
        M5.Lcd.fillRect(x, y, w, h, color);
#endif
    }
    void setTextDatum(int datum) override {
#ifdef ARDUINO
        M5.Lcd.setTextDatum(datum);
#endif
    }
    void setTextFont(int font) override {
#ifdef ARDUINO
        M5.Lcd.setTextFont(font);
#endif
    }
}; 