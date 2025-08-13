#pragma once
#include <stdint.h>
#include "IVibration.h"

class MockVibrationOut : public IVibrationOutput {
public:
    uint8_t lastDuty = 0;
    void setDutyPercent(uint8_t dutyPercent) override { lastDuty = dutyPercent; }
};


