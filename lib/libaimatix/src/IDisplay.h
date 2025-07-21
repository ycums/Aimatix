#pragma once
#include <cstdint>
class IDisplay {
public:
    virtual ~IDisplay() {}
    virtual void clear() = 0;
    virtual void drawText(int x, int y, const char* text, int fontSize) = 0;
    virtual void setTextColor(uint32_t color, uint32_t bgColor) = 0;
    virtual void fillRect(int x, int y, int w, int h, uint32_t color) = 0;
    virtual void setTextDatum(int datum) = 0;
    virtual void setTextFont(int font) = 0;
    // 必要に応じて追加
}; 