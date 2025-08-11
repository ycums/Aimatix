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

}


