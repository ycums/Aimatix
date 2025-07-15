#pragma once
#include "../../lib/aimatix_lib/include/IDisplay.h"
#include <string>

class MockDisplay : public IDisplay {
public:
    MockDisplay();
    ~MockDisplay() override;
    void clear() override;
    void drawText(int x, int y, const char* text, int fontSize) override;
    void setTextColor(uint32_t color) override;
    void fillRect(int x, int y, int w, int h, uint32_t color) override;
    // テスト用: 描画内容を記録
    std::string lastDrawnText;
    int lastDrawX, lastDrawY, lastFontSize;
}; 