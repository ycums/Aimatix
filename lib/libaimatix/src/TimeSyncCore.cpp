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
    const int m = tzOffsetMin * sign;
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

std::string buildPosixTZ(int tzOffsetMin) {
    // POSIX TZ uses reversed sign: UTC+9 -> "-9"
    int off = tzOffsetMin;
    const int sign = (off >= 0) ? -1 : +1;
    off = off < 0 ? -off : off;
    const int hh = off / 60;
    const int mm = off % 60;
    char buf[16];
    if (mm == 0) {
        snprintf(buf, sizeof(buf), "LT%+d", sign * hh);
    } else {
        snprintf(buf, sizeof(buf), "LT%+d:%02d", sign * hh, mm);
    }
    return std::string(buf);
}

static inline bool isSpace(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

bool jsonExtractRaw(const std::string& body, const char* key, std::string& out) {
    const std::string k(key);
    const size_t keyPos = body.find(k);
    if (keyPos == std::string::npos) return false;
    const size_t colonPos = body.find(':', keyPos + k.size());
    if (colonPos == std::string::npos) return false;
    size_t p = colonPos + 1;
    while (p < body.size() && isSpace(body[p])) ++p;
    if (p >= body.size()) return false;
    if (body[p] == '"') {
        const size_t start = p + 1;
        const size_t endq = body.find('"', start);
        if (endq == std::string::npos) return false;
        out.assign(body.data() + start, endq - start);
        return true;
    } else {
        size_t end = body.find(',', p);
        if (end == std::string::npos) end = body.find('}', p);
        if (end == std::string::npos) end = body.size();
        out.assign(body.data() + p, end - p);
        // trim both ends
        size_t b = 0;
        size_t e = out.size();
        while (b < e && isSpace(out[b])) { ++b; }
        while (e > b && isSpace(out[e - 1])) { --e; }
        out = out.substr(b, e - b);
        return !out.empty();
    }
}

bool jsonExtractString(const std::string& body, const char* key, std::string& out) {
    std::string tmp;
    if (!jsonExtractRaw(body, key, tmp)) return false;
    out = tmp;
    return true;
}

bool jsonExtractInt64(const std::string& body, const char* key, int64_t& out) {
    std::string tmp;
    if (!jsonExtractRaw(body, key, tmp)) return false;
    // manual parse
    const char* p = tmp.c_str();
    while (isSpace(*p)) ++p;
    out = strtoll(p, nullptr, 10);
    return true;
}

bool jsonExtractInt(const std::string& body, const char* key, int& out) {
    int64_t v = 0;
    if (!jsonExtractInt64(body, key, v)) return false;
    out = static_cast<int>(v);
    return true;
}

}

