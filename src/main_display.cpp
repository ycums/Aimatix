#include "main_display.h"
#include "TimeLogic.h"
#include <stdio.h>

// --- Adapter層で差し替え可能な関数ポインタ ---
static void (*drawRect_impl)(int, int, int, int) = nullptr;
static void (*drawString_impl)(const char*, int, int) = nullptr;
static void (*fillProgressBar_impl)(int, int, int, int, int) = nullptr;

void setDrawRectImpl(void (*impl)(int, int, int, int)) { drawRect_impl = impl; }
void setDrawStringImpl(void (*impl)(const char*, int, int)) { drawString_impl = impl; }
void setFillProgressBarImpl(void (*impl)(int, int, int, int, int)) { fillProgressBar_impl = impl; }

void drawRect(int x, int y, int w, int h) {
    if (drawRect_impl) drawRect_impl(x, y, w, h);
}
void drawString(const char* str, int x, int y) {
    if (drawString_impl) drawString_impl(str, x, y);
}
void fillProgressBar(int x, int y, int w, int h, int percent) {
    if (fillProgressBar_impl) fillProgressBar_impl(x, y, w, h, percent);
}

void drawMainDisplay() {
    drawRect(0, 0, 320, 240);
    drawString("MainDisplay", 0, 0);
    char buf[16];
    snprintf(buf, sizeof(buf), "%02d:%02d", TimeLogic::getHour(), TimeLogic::getMinute());
    drawString(buf, 40, 40);
    drawString("00:04:59", 40, 80);
    fillProgressBar(40, 120, 200, 20, 80);
    drawString("A:Start  B:Stop  C:Reset", 0, 220);
} 