#include "main_display.h"
#include "IDisplay.h"
#include "ui_constants.h"
#include <stdio.h>
#include <string.h>
#include <vector>
#include <string>
#include <ctime>
#include "AlarmLogic.h"

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

void fillProgressBarSprite(IDisplay* disp, int x, int y, int w, int h, int percent) {
    disp->fillRect(x, y, w, h, TFT_BLACK);
    disp->fillRect(x, y, w, 1, AMBER_COLOR); // 上枠
    disp->fillRect(x, y + h - 1, w, 1, AMBER_COLOR); // 下枠
    disp->fillRect(x, y, 1, h, AMBER_COLOR); // 左枠
    disp->fillRect(x + w - 1, y, 1, h, AMBER_COLOR); // 右枠
    int fillW = (w - 2) * percent / 100;
    if (fillW > 0) {
        disp->fillRect(x + 1, y + 1, fillW, h - 2, AMBER_COLOR);
    }
}
// ...他UI部品も同様にIDisplay*経由で実装... 