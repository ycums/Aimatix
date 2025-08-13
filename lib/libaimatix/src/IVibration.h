#pragma once
#include <stdint.h>

/**
 * Abstract output for vibration device.
 * Duty is expressed in percent [0-100].
 */
class IVibrationOutput {
public:
	virtual ~IVibrationOutput() = default;
	virtual void setDutyPercent(uint8_t dutyPercent) = 0; // 0-100
};


