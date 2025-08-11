#pragma once

#include <string>
#include <cstdint>

// Utility for building Wi‑Fi QR payloads used in Time Sync step1.
// Format (PoC準拠): "WIFI:T:WPA;S:<SSID>;P:<PSK>;H:false;;"
// Escape rules: in SSID/PSK, characters \\ ; , : must be escaped with a preceding backslash.
namespace TimeSyncCore {

// Build Wi‑Fi QR payload from SSID and PSK with proper escaping.
std::string buildWifiQrPayload(const std::string& ssid, const std::string& psk);

// Build URL payload for Step2 QR: e.g., "http://<ip>/sync?t=<token>"
std::string buildUrl(const std::string& ip, const std::string& token);

// Format timezone offset minutes to "+HHMM"/"-HHMM"
std::string formatOffsetHHMM(int tzOffsetMin);

// Verify one-time token equality (constant-time not required for MCU scope)
bool verifyToken(const std::string& expected, const std::string& actual);

// Check if nowMs is within [startMs, startMs+windowMs]
bool isWithinWindow(uint32_t startMs, uint32_t nowMs, uint32_t windowMs);

}


