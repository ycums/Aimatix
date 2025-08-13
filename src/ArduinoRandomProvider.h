#pragma once

#include <cstdint>
#include "IRandomProvider.h"

class ArduinoRandomProvider : public IRandomProvider {
public:
    uint64_t getRandom64() override;
};


