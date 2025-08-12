#pragma once
#include <cstdint>
class IDisplay {
public:
    virtual ~IDisplay() {}
    virtual void clear() = 0;
    virtual void drawText(int x, int y, const char* text, int fontSize) = 0;
    virtual void setTextColor(uint16_t color, uint16_t bgColor) = 0;
    virtual void fillRect(int x, int y, int w, int h, uint16_t color) = 0;
    // Double-buffered fill for flicker-free drawing. Default: fallback to fillRect.
    virtual void fillRectBuffered(int x, int y, int w, int h, uint16_t color) {
        fillRect(x, y, w, h, color);
    }
    // Optional transactional drawing (default: no-op).
    virtual void beginUpdate() {}
    virtual void endUpdate() {}
    virtual void drawRect(int x, int y, int w, int h, uint16_t color) = 0;
    virtual void setTextDatum(uint8_t datum) = 0;
    virtual void setTextFont(int font) = 0;
    virtual void fillProgressBarSprite(int x, int y, int w, int h, int percent) = 0;
    virtual void drawLine(int x0, int y0, int x1, int y1, uint16_t color) = 0;
    virtual int getTextDatum() const = 0;
    // 必要に応じて追加
}; 