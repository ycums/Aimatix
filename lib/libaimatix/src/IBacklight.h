#pragma once

#include <cstdint>

/**
 * Abstract output for display backlight brightness.
 * Brightness is expressed in 0-255 (uint8_t).
 * Device adapters should clamp to device-acceptable ranges.
 */
class IBacklight {
public:
	virtual ~IBacklight() = default;
	virtual void setBrightness(uint8_t brightness) = 0; // 0-255
};


