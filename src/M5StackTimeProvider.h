#pragma once
#include "../lib/libaimatix/src/ITimeProvider.h"
#include <ctime>

class M5StackTimeProvider : public ITimeProvider {
public:
    time_t now() const override { return time(nullptr); }
    struct tm* localtime(time_t* t) const override { return ::localtime(t); }
}; 