#pragma once
#include <ctime>
#include <cstdint>

// Unified time service interface
// - Wall clock time (seconds) via now()/localtime()/setSystemTime()
// - Monotonic milliseconds via monotonicMillis() for sub-second operations
class ITimeService {
public:
    virtual ~ITimeService() {}

    // Wall clock (may jump forward/back by user/NTP corrections)
    virtual time_t now() const = 0;
    virtual struct tm* localtime(time_t* time) const = 0;
    virtual bool setSystemTime(time_t time) = 0;

    // Monotonic milliseconds for animations, debouncing, schedulers
    virtual uint32_t monotonicMillis() const = 0;
};


