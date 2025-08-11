#include "SoftApTimeSyncController.h"

#ifdef ARDUINO
#include <Arduino.h>
#include <WiFi.h>
#endif

#include <stdint.h>

namespace {
static uint64_t simpleRand64() {
    // 非暗号用の簡易生成（Step1暫定: PoC準拠64bit相当）
    uint64_t x = 1469598103934665603ull;
    x ^= (uint64_t)millis();
    x *= 1099511628211ull;
    x ^= (uint64_t)micros();
    x *= 1099511628211ull;
    return x;
}

static std::string toHex64(uint64_t v) {
    static const char* hex = "0123456789abcdef";
    std::string s(16, '0');
    for (int i = 15; i >= 0; --i) {
        s[i] = hex[v & 0xF];
        v >>= 4;
    }
    return s;
}
}

SoftApTimeSyncController::SoftApTimeSyncController()
    : running_(false) {}

void SoftApTimeSyncController::generateCredentials() {
    const uint64_t r1 = simpleRand64();
    const uint64_t r2 = simpleRand64();
    const uint64_t r3 = simpleRand64();
    ssid_  = std::string("AIM-TS-") + toHex64(r1).substr(0, 8);
    psk_   = toHex64(r2).substr(0, 16);
    token_ = toHex64(r3).substr(0, 16);
}

void SoftApTimeSyncController::begin() {
    generateCredentials();
#ifdef ARDUINO
    WiFi.mode(WIFI_AP);
    // PSKは最低8文字必要
    WiFi.softAP(ssid_.c_str(), psk_.c_str());
#endif
    running_ = true;
}

void SoftApTimeSyncController::cancel() {
#ifdef ARDUINO
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);
#endif
    running_ = false;
}

void SoftApTimeSyncController::loopTick() {
    // Step1では特に行うことなし（将来、クライアント接続検出等）
}

void SoftApTimeSyncController::reissue() {
    generateCredentials();
#ifdef ARDUINO
    if (running_) {
        WiFi.softAPdisconnect(true);
        WiFi.softAP(ssid_.c_str(), psk_.c_str());
    }
#endif
}

void SoftApTimeSyncController::getCredentials(std::string& outSsid, std::string& outPsk) const {
    outSsid = ssid_;
    outPsk = psk_;
}


