#include "TimeThreadSafe.h"

namespace TimeThreadSafe {

bool toLocalTime(const time_t in, struct tm& out) noexcept {
#if defined(_WIN32)
    return localtime_s(&out, &in) == 0;
#elif defined(__unix__) || defined(__APPLE__)
    return localtime_r(&in, &out) != nullptr;
#else
    // Fallback: assume single-threaded environment
    const struct tm* t = std::localtime(&in);
    if (t == nullptr) return false;
    out = *t;
    return true;
#endif
}

bool toGmTime(const time_t in, struct tm& out) noexcept {
#if defined(_WIN32)
    return gmtime_s(&out, &in) == 0;
#elif defined(__unix__) || defined(__APPLE__)
    return gmtime_r(&in, &out) != nullptr;
#else
    const struct tm* t = std::gmtime(&in);
    if (t == nullptr) return false;
    out = *t;
    return true;
#endif
}

}


