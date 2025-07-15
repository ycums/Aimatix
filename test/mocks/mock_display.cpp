#include "mock_display.h"

MockDisplay::MockDisplay() {}
MockDisplay::~MockDisplay() {}

void MockDisplay::clear() {}

void MockDisplay::drawText(int x, int y, const char* text, int fontSize) {
    lastDrawnText = text ? text : "";
    lastDrawX = x;
    lastDrawY = y;
    lastFontSize = fontSize;
}

void MockDisplay::setTextColor(uint32_t color) {}

void MockDisplay::fillRect(int x, int y, int w, int h, uint32_t color) {} 