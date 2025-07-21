#pragma once
#include "main_display.h"

inline void drawCommonTitleBar(const char* modeName, int batteryLevel = 42, bool isCharging = false) {
    drawTitleBar(modeName, batteryLevel, isCharging);
}

inline void drawCommonButtonHints(const char* btnA, const char* btnB, const char* btnC) {
    drawButtonHintsGrid(btnA, btnB, btnC);
} 