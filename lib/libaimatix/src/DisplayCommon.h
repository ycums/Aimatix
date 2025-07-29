#pragma once
#include "IDisplay.h"
#include "ui_constants.h"

void drawTitleBar(IDisplay* disp, const char* modeName, int batteryLevel, bool isCharging);
void drawButtonHintsGrid(IDisplay* disp, const char* btnA, const char* btnB, const char* btnC);
void drawGridLines(IDisplay* disp); 