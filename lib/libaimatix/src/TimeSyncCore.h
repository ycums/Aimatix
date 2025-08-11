#pragma once

#include <string>

// Utility for building Wi‑Fi QR payloads used in Time Sync step1.
// Format (PoC準拠): "WIFI:T:WPA;S:<SSID>;P:<PSK>;H:false;;"
// Escape rules: in SSID/PSK, characters \\ ; , : must be escaped with a preceding backslash.
namespace TimeSyncCore {

// Build Wi‑Fi QR payload from SSID and PSK with proper escaping.
std::string buildWifiQrPayload(const std::string& ssid, const std::string& psk);

}


