#pragma once
#include "IDisplay.h"
#include "ui_constants.h"
#include <vector>
#include <string>

void drawTitleBar(IDisplay* disp, const char* modeName, int batteryLevel, bool isCharging);
void drawButtonHintsGrid(IDisplay* disp, const char* btnA, const char* btnB, const char* btnC);
void fillProgressBarSprite(IDisplay* disp, int x, int y, int w, int h, int percent);
// ...他UI部品も同様にIDisplay*を受け取る形で宣言 