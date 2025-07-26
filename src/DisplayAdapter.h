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
    void drawRect(int x, int y, int w, int h, uint32_t color) override {
        M5.Lcd.drawRect(x, y, w, h, color);
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
    void fillProgressBarSprite(int x, int y, int w, int h, int percent) override {
#ifdef ARDUINO
        TFT_eSprite sprite(&M5.Lcd);
        sprite.createSprite(w, h);
        sprite.fillRect(0, 0, w, h, TFT_BLACK);
        sprite.drawRect(0, 0, w, h, AMBER_COLOR);
        int fillW = (w - 2) * percent / 100;
        if (fillW > 0) {
            sprite.fillRect(1, 1, fillW, h - 2, AMBER_COLOR);
        }
        sprite.pushSprite(x, y);
        sprite.deleteSprite();
#endif
    }
    void drawLine(int x0, int y0, int x1, int y1, uint32_t color) override {
#ifdef ARDUINO
        M5.Lcd.drawLine(x0, y0, x1, y1, color);
#endif
    }
    int getTextDatum() const override {
#ifdef ARDUINO
        return M5.Lcd.getTextDatum();
#else
        return 0;
#endif
    }
}; 