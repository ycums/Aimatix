#pragma once
#include "IDisplay.h"
#include <Arduino.h>
#include <M5Unified.h>
#include <M5GFX.h>
#include <memory>
#include <mutex>
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

    void fillRectBuffered(int x, int y, int w, int h, uint16_t color) override {
        if (w <= 0 || h <= 0) {
            return;
        }
        constexpr int kSwitchArea = 0; // 実測上、HW直塗りが常に優位（Core2）
        const int area = w * h;
        if (area >= kSwitchArea) {
            // 単色の大面積はハードウェア直塗りが最速
            beginUpdate();
            M5.Display.writeFillRect(x, y, w, h, color);
            endUpdate();
            // 転送完了待ちで描画の競合を抑止
            M5.Display.waitDisplay();
            return;
        }
        ensureOverlaySprite(w, h);
        overlaySprite_->fillSprite(color);
        beginUpdate();
        overlaySprite_->pushSprite(x, y);
        endUpdate();
        // スプライト転送完了待ち
        M5.Display.waitDisplay();
    }

    void beginUpdate() override {
        // 再入可能 + マルチタスク排他
        std::lock_guard<std::recursive_mutex> lock(updateMutex_);
        if (updateDepth_++ == 0) {
            M5.Display.startWrite();
        }
    }

    void endUpdate() override {
        std::lock_guard<std::recursive_mutex> lock(updateMutex_);
        if (updateDepth_ == 0) return;
        if (--updateDepth_ == 0) {
            M5.Display.endWrite();
        }
    }

public:
#ifdef ENABLE_FILL_BENCH
    void runFillBench() {
        const int W = SCREEN_WIDTH;
        const int heights[] = {40, 80, 120, 160, 200};
        for (int h : heights) {
            benchFill(W, h);
        }
    }
#endif

private:
    void ensureOverlaySprite(int w, int h) {
        if (!overlaySprite_) {
            overlaySprite_ = std::unique_ptr<M5Canvas>(new M5Canvas(&M5.Display));
            overlayW_ = 0;
            overlayH_ = 0;
        }
        if (w != overlayW_ || h != overlayH_) {
            if (overlayW_ > 0 && overlayH_ > 0) {
                overlaySprite_->deleteSprite();
            }
            overlaySprite_->createSprite(w, h);
            overlayW_ = w;
            overlayH_ = h;
        }
    }

    std::unique_ptr<M5Canvas> overlaySprite_;
    int overlayW_ = 0;
    int overlayH_ = 0;
    int updateDepth_ = 0;
    std::recursive_mutex updateMutex_;
    
#ifdef ENABLE_FILL_BENCH
    void benchFill(int w, int h) {
        const int N = 10;
        uint32_t t0 = micros();
        for (int i = 0; i < N; ++i) {
            beginUpdate();
            M5.Display.writeFillRect(0, 0, w, h, TFT_BLACK);
            endUpdate();
            M5.Display.waitDisplay();
        }
        uint32_t hw_us = (micros() - t0) / N;
        t0 = micros();
        for (int i = 0; i < N; ++i) {
            M5Canvas c(&M5.Display);
            c.createSprite(w, h);
            c.fillSprite(TFT_BLACK);
            beginUpdate();
            c.pushSprite(0, 0);
            endUpdate();
            M5.Display.waitDisplay();
            c.deleteSprite();
        }
        uint32_t spr_new_us = (micros() - t0) / N;
        ensureOverlaySprite(w, h);
        t0 = micros();
        for (int i = 0; i < N; ++i) {
            overlaySprite_->fillSprite(TFT_BLACK);
            beginUpdate();
            overlaySprite_->pushSprite(0, 0);
            endUpdate();
            M5.Display.waitDisplay();
        }
        uint32_t spr_reuse_us = (micros() - t0) / N;
        Serial.printf("bench %dx%d: HW=%luus, SPR(new)=%luus, SPR(reuse)=%luus\n",
                      w, h, (unsigned long)hw_us, (unsigned long)spr_new_us, (unsigned long)spr_reuse_us);
    }
#endif

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