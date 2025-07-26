#include "DisplayCommon.h"
#include "IDisplay.h"
#include "ui_constants.h"
#include <cstdio>
#include <cstring>

// 定数定義
constexpr int TITLE_OFFSET_X = 5;
constexpr int TITLE_OFFSET_Y = 2;
constexpr int BATTERY_OFFSET_X = 70;
constexpr int HINT_OFFSET_Y = 2;
constexpr int BUTTON_A_OFFSET_X = 20;
constexpr int BUTTON_B_OFFSET_X = 30;
constexpr int BUTTON_C_OFFSET_X = 80;
constexpr int GRID_LINES_X = 16;
constexpr int GRID_LINES_Y = 12;

void drawTitleBar(IDisplay* disp, const char* modeName, int batteryLevel, bool isCharging) {
    disp->fillRect(0, TITLE_HEIGHT - 1, SCREEN_WIDTH, 1, AMBER_COLOR); // 横線
    disp->setTextFont(FONT_AUXILIARY);
    disp->setTextColor(AMBER_COLOR, TFT_BLACK);
    disp->setTextDatum(TL_DATUM);
    disp->drawText(TITLE_OFFSET_X, TITLE_OFFSET_Y, modeName, FONT_AUXILIARY);
    constexpr int BATTERY_STR_SIZE = 16;
    char batteryStr[BATTERY_STR_SIZE];
    snprintf(batteryStr, sizeof(batteryStr), "%s %d%%", isCharging ? "CHG" : "BAT", batteryLevel);
    disp->drawText(SCREEN_WIDTH - BATTERY_OFFSET_X, TITLE_OFFSET_Y, batteryStr, FONT_AUXILIARY);
}

void drawButtonHintsGrid(IDisplay* disp, const char* btnA, const char* btnB, const char* btnC) {
    disp->fillRect(0, SCREEN_HEIGHT - HINT_HEIGHT, SCREEN_WIDTH, 1, AMBER_COLOR); // 横線
    disp->setTextFont(FONT_AUXILIARY);
    disp->setTextColor(AMBER_COLOR, TFT_BLACK);
    disp->setTextDatum(TL_DATUM);
    if (btnA != nullptr) {
        disp->drawText(BUTTON_A_OFFSET_X, SCREEN_HEIGHT - HINT_HEIGHT + HINT_OFFSET_Y, btnA, FONT_AUXILIARY);
    }
    if (btnB != nullptr) {
        disp->drawText(SCREEN_WIDTH / 2 - BUTTON_B_OFFSET_X, SCREEN_HEIGHT - HINT_HEIGHT + HINT_OFFSET_Y, btnB, FONT_AUXILIARY);
    }
    if (btnC != nullptr) {
        disp->drawText(SCREEN_WIDTH - BUTTON_C_OFFSET_X, SCREEN_HEIGHT - HINT_HEIGHT + HINT_OFFSET_Y, btnC, FONT_AUXILIARY);
    }
}

void drawGridLines(IDisplay* disp) {
    for (int i = 0; i <= GRID_LINES_X; ++i) {
        const int pos_x = i * GRID_WIDTH;
        disp->drawRect(pos_x, 0, 1, SCREEN_HEIGHT, AMBER_COLOR);
    }
    for (int i = 0; i <= GRID_LINES_Y; ++i) {
        const int pos_y = TITLE_HEIGHT + i * GRID_HEIGHT;
        disp->drawRect(0, pos_y, SCREEN_WIDTH, 1, AMBER_COLOR);
    }
} 