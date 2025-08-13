#pragma once

#include <cstdint>
#include <algorithm>
#include <M5Unified.h>
#include "IBacklight.h"

/**
 * M5Unified adapter for backlight brightness.
 * Accepts 0-255 and clamps to device-acceptable range (0-255 for M5.Display).
 */
class M5BacklightAdapter : public IBacklight {
public:
	void setBrightness(uint8_t brightness) override {
		const uint8_t clamped = std::min<uint8_t>(255, std::max<uint8_t>(0, brightness));
		M5.Display.setBrightness(clamped);
	}
};


