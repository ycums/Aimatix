#pragma once

#include "IAlarmActiveView.h"
#include "IDisplay.h"
#include "ui_constants.h"

class AlarmActiveViewImpl : public IAlarmActiveView {
public:
    explicit AlarmActiveViewImpl(IDisplay* display) : disp_(display) {}

    // 中央コンテンツのみを黒/アンバーで塗り替える（タイトル/ナビは除外、プログレスは含む）
    void drawFlashOverlay(bool on) override {
        if (!disp_) return;
        // タイトル直下からナビ上端まで一括で塗りつぶし
        const int fillX = 0;
        const int fillY = TITLE_HEIGHT;
        const int fillW = SCREEN_WIDTH;
        const int fillH = SCREEN_HEIGHT - HINT_HEIGHT - TITLE_HEIGHT;
        const uint16_t color = on ? AMBER_COLOR : TFT_BLACK;
        if (fillH > 0) {
            disp_->fillRectBuffered(fillX, fillY, fillW, fillH, color);
        }
    }

private:
    IDisplay* disp_;
};


