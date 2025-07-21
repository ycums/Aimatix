#pragma once
#include "IDisplay.h"
#include "ui_constants.h"
#include <vector>
#include <string>
#include <ctime>

void drawTitleBar(IDisplay* disp, const char* modeName, int batteryLevel, bool isCharging);
void drawButtonHintsGrid(IDisplay* disp, const char* btnA, const char* btnB, const char* btnC);
void fillProgressBarSprite(IDisplay* disp, int x, int y, int w, int h, int percent);
void drawMainDisplay(IDisplay* disp, const std::vector<time_t>& alarmTimes, const char* modeName, int batteryLevel, bool isCharging);
void drawGridLines(IDisplay* disp); 