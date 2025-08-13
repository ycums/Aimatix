#pragma once
#include <M5Unified.h>
#include <Arduino.h>
#include "IVibration.h"

/**
 * Core2VibrationAdapter maps duty [0-100] to M5.Power.setVibration [0-255].
 */
class Core2VibrationAdapter : public IVibrationOutput {
public:
	void setDutyPercent(uint8_t dutyPercent) override {
		if (dutyPercent > 100) dutyPercent = 100;
		uint8_t hw = static_cast<uint8_t>((static_cast<uint16_t>(dutyPercent) * 255) / 100);
        static uint8_t s_prev_hw = 0xFF;
        if (hw != s_prev_hw) {
            Serial.printf("[VIBE] setDutyPercent=%u (hw=%u)\r\n", dutyPercent, hw);
            s_prev_hw = hw;
        }
		M5.Power.setVibration(hw);
	}
};


