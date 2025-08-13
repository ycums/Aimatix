#pragma once

#include <ctime>

namespace TimeThreadSafe {

// Thread-safe localtime conversion wrapper.
// Returns true on success and writes result into out.
bool toLocalTime(const time_t in, struct tm& out) noexcept;

// Thread-safe gmtime conversion wrapper.
// Returns true on success and writes result into out.
bool toGmTime(const time_t in, struct tm& out) noexcept;

}


