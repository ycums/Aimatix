#include "DisplayCommon.h"
#include "IDisplay.h"
#include "ui_constants.h"
#include <stdio.h>
#include <string.h>

void drawTitleBar(IDisplay* disp, const char* modeName, int batteryLevel, bool isCharging) {
    disp->fillRect(0, TITLE_HEIGHT - 1, SCREEN_WIDTH, 1, AMBER_COLOR); // 横線
    disp->setTextFont(FONT_AUXILIARY);
    disp->setTextColor(AMBER_COLOR, TFT_BLACK);
    disp->setTextDatum(TL_DATUM);
    disp->drawText(5, 2, modeName, FONT_AUXILIARY);
    char batteryStr[16];
    snprintf(batteryStr, sizeof(batteryStr), "%s %d%%", isCharging ? "CHG" : "BAT", batteryLevel);
    disp->drawText(SCREEN_WIDTH - 70, 2, batteryStr, FONT_AUXILIARY);
}

void drawButtonHintsGrid(IDisplay* disp, const char* btnA, const char* btnB, const char* btnC) {
    disp->fillRect(0, SCREEN_HEIGHT - HINT_HEIGHT, SCREEN_WIDTH, 1, AMBER_COLOR); // 横線
    disp->setTextFont(FONT_AUXILIARY);
    disp->setTextColor(AMBER_COLOR, TFT_BLACK);
    disp->setTextDatum(TL_DATUM);
    if (btnA) disp->drawText(20, SCREEN_HEIGHT - HINT_HEIGHT + 2, btnA, FONT_AUXILIARY);
    if (btnB) disp->drawText(SCREEN_WIDTH / 2 - 30, SCREEN_HEIGHT - HINT_HEIGHT + 2, btnB, FONT_AUXILIARY);
    if (btnC) disp->drawText(SCREEN_WIDTH - 80, SCREEN_HEIGHT - HINT_HEIGHT + 2, btnC, FONT_AUXILIARY);
}

void drawGridLines(IDisplay* disp) {
    for (int i = 0; i <= 16; ++i) {
        int x = i * GRID_WIDTH;
        disp->drawRect(x, 0, 1, SCREEN_HEIGHT, AMBER_COLOR);
    }
    for (int i = 0; i <= 12; ++i) {
        int y = TITLE_HEIGHT + i * GRID_HEIGHT;
        disp->drawRect(0, y, SCREEN_WIDTH, 1, AMBER_COLOR);
    }
} 