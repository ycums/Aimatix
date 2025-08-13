#pragma once

#include <cstdint>

// Random source abstraction for pure logic
class IRandomProvider {
public:
    virtual ~IRandomProvider() {}
    // Returns a 64-bit random value (non-crypto acceptable for this use case)
    virtual uint64_t getRandom64() = 0;
};


