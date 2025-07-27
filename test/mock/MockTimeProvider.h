#pragma once
#include "ITimeProvider.h"

class MockTimeProvider : public ITimeProvider {
public:
    time_t fixedTime;
    MockTimeProvider(time_t t) : fixedTime(t) {}
    time_t now() const override { return fixedTime; }
    struct tm* localtime(time_t* t) const override { return ::localtime(t); }
}; 