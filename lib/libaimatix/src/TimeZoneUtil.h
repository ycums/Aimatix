#pragma once

#include <string>

namespace TimeZoneUtil {

/**
 * Build POSIX TZ string from offset minutes.
 * Example:
 *  +540  -> "LT-9"
 *  -240  -> "LT+4"
 *  +330  -> "LT-5:30"
 *  -570  -> "LT+9:30"
 * Note: POSIX TZ uses reversed sign (east is negative).
 */
std::string buildPosixTzFromOffsetMinutes(int tzOffsetMinutes);

} // namespace TimeZoneUtil


