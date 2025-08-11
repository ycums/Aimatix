#include "TimeSyncCore.h"

namespace {
std::string escapeForWifiField(const std::string& input) {
    std::string out;
    out.reserve(input.size() * 2);
    for (char c : input) {
        switch (c) {
            case '\\':
            case ';':
            case ',':
            case ':':
                out.push_back('\\');
                out.push_back(c);
                break;
            default:
                out.push_back(c);
        }
    }
    return out;
}
}

namespace TimeSyncCore {

std::string buildWifiQrPayload(const std::string& ssid, const std::string& psk) {
    const std::string escSsid = escapeForWifiField(ssid);
    const std::string escPsk  = escapeForWifiField(psk);
    std::string payload;
    payload.reserve(32 + escSsid.size() + escPsk.size());
    payload += "WIFI:T:WPA;S:";
    payload += escSsid;
    payload += ";P:";
    payload += escPsk;
    payload += ";H:false;;";
    return payload;
}

std::string buildUrl(const std::string& ip, const std::string& token) {
    std::string url;
    url.reserve(32 + ip.size() + token.size());
    url += "http://";
    url += ip;
    url += "/sync?t=";
    url += token;
    return url;
}

std::string formatOffsetHHMM(int tzOffsetMin) {
    const int sign = tzOffsetMin >= 0 ? 1 : -1;
    int m = tzOffsetMin * sign;
    const int hh = m / 60;
    const int mm = m % 60;
    char buf[8];
    snprintf(buf, sizeof(buf), "%c%02d%02d", sign > 0 ? '+' : '-', hh, mm);
    return std::string(buf);
}

bool verifyToken(const std::string& expected, const std::string& actual) {
    if (expected.size() != actual.size()) return false;
    // Simple equality; constant-time not critical here
    return expected == actual;
}

bool isWithinWindow(uint32_t startMs, uint32_t nowMs, uint32_t windowMs) {
    // Handle wraparound with 32-bit unsigned arithmetic
    const uint32_t elapsed = nowMs - startMs;
    return elapsed <= windowMs;
}

}


