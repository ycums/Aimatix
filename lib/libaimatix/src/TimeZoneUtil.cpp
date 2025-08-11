#include "TimeZoneUtil.h"

#include <cstdio>
#include <cstdlib>
#include <string>

namespace TimeZoneUtil {

static void appendTzOffsetString(std::string& out, int tzOffsetMinutes) {
    // POSIX TZ: West is positive, East is negative
    // Our tzOffsetMinutes follows JavaScript: east positive
    int minutes = tzOffsetMinutes;
    const bool isZero = minutes == 0;
    const bool isEast = minutes > 0; // treat 0 specially
    int absMin = minutes >= 0 ? minutes : -minutes;
    int hours = absMin / 60;
    int mins  = absMin % 60;
    // Reverse sign for POSIX: east -> '-', west -> '+'
    out.push_back(isZero ? '+' : (isEast ? '-' : '+'));
    char buf[16];
    if (mins == 0) {
        // LT±H
        std::snprintf(buf, sizeof(buf), "%d", hours);
        out.append(buf);
    } else {
        // LT±H:MM
        std::snprintf(buf, sizeof(buf), "%d:%02d", hours, mins);
        out.append(buf);
    }
}

std::string buildPosixTzFromOffsetMinutes(int tzOffsetMinutes) {
    // ESP32 newlib は 3文字以上の略号を要求するケースがあるため GMT を使用
    // 0分の場合は POSIX 準拠で "GMT0" を返す
    if (tzOffsetMinutes == 0) {
        return std::string("GMT0");
    }
    std::string tz = "GMT";
    appendTzOffsetString(tz, tzOffsetMinutes);
    return tz;
}

} // namespace TimeZoneUtil


