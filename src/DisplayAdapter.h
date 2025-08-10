#pragma once
#include "IDisplay.h"
#include <Arduino.h>
#include <M5Unified.h>
#include <M5GFX.h>
// 色定数を追加
#include "ui_constants.h"



class DisplayAdapter : public IDisplay {
public:
    void clear() override {
        M5.Display.fillScreen(TFT_BLACK);
    }
    
    void drawText(int x, int y, const char* text, int fontSize) override {
        M5.Display.setTextFont(fontSize);
        M5.Display.drawString(text, x, y);
    }
    
    void setTextColor(uint16_t color, uint16_t bgColor) override {
        M5.Display.setTextColor(color, bgColor);
    }
    
    void fillRect(int x, int y, int w, int h, uint16_t color) override {
        M5.Display.fillRect(x, y, w, h, color);
    }

    void drawRect(int x, int y, int w, int h, uint16_t color) override {
        M5.Display.drawRect(x, y, w, h, color);
    }
    
    void setTextDatum(uint8_t datum) override {
        M5.Display.setTextDatum(static_cast<textdatum_t>(datum));
    }
    
    void setTextFont(int font) override {
        M5.Display.setTextFont(font);
    }
    
    void fillProgressBarSprite(int x, int y, int w, int h, int percent) override {
        M5Canvas canvas(&M5.Display);
        canvas.createSprite(w, h);
        canvas.fillSprite(TFT_BLACK);
        canvas.drawRect(0, 0, w, h, AMBER_COLOR);  // 統一された色を使用
        
        int fillW = (w - 2) * percent / 100;
        if (fillW > 0) {
            canvas.fillRect(1, 1, fillW, h - 2, AMBER_COLOR);  // 統一された色を使用
        }
        
        canvas.pushSprite(x, y);
        canvas.deleteSprite();
    }
    
    void drawLine(int x0, int y0, int x1, int y1, uint16_t color) override {
        M5.Display.drawLine(x0, y0, x1, y1, color);
    }
    
    int getTextDatum() const override {
        return M5.Display.getTextDatum();
    }
}; 