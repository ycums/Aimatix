#include "SoftApTimeSyncController.h"

#include "TimeSyncLogic.h"
#include "ITimeManager.h"
#include "ITimeProvider.h"
#include "ArduinoRandomProvider.h"

#ifdef ARDUINO
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
static WebServer server(80);
extern ITimeManager* g_time_manager;   // provided in main.cpp or platform layer
extern ITimeProvider* g_time_provider; // provided in main.cpp or platform layer
#endif

#include <cstdint>

namespace {
uint64_t simpleRand64() {
    // 非暗号用の簡易生成（Step1暫定: PoC準拠64bit相当）
    uint64_t x = 1469598103934665603ULL;
    x ^= (uint64_t)millis();
    x *= 1099511628211ULL;
    x ^= (uint64_t)micros();
    x *= 1099511628211ULL;
    return x;
}

std::string toHex64(uint64_t v) {
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
    // Setup minimal routes for Step2 (sync + time/set)
    server.on("/sync", HTTP_GET, [this]() {
        // Minimal HTML that auto-posts Date.now()/getTimezoneOffset()
        String html;
        html.reserve(1024);
        html += F("<!doctype html><html><head><meta charset=\"utf-8\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\"></head><body><pre id='log'>Sync...</pre><script>");
        html += F("const TOKEN='"), html += token_.c_str(), html += F("';\n");
        html += F("const epochMs=Date.now(); const tzOffsetMin = -new Date().getTimezoneOffset();\n");
        html += F("fetch('/time/set',{method:'POST',headers:{'Content-Type':'application/json'},body: JSON.stringify({epochMs, tzOffsetMin, token:TOKEN})}).then(async r=>{document.getElementById('log').textContent=r.ok?('OK\\n'+await r.text()):('ERR '+r.status+'\\n'+await r.text());}).catch(e=>{document.getElementById('log').textContent='ERR\\n'+e;});");
        html += F("</script></body></html>");
        server.send(200, "text/html", html);
    });
    server.on("/time/set", HTTP_POST, [this]() {
        if (!server.hasArg("plain")) { server.send(400, "text/plain", "Bad Request"); return; }
        const String body = server.arg("plain");
        // very small parser (not robust) to extract numbers/strings
        auto extract = [](const String& s, const char* key, String& out)->bool{
            int k = s.indexOf(key);
            if (k < 0) return false;
            int c = s.indexOf(':', k);
            if (c < 0) return false;
            int q1 = s.indexOf('"', c);
            int q2 = s.indexOf('"', q1+1);
            if (q1 >= 0 && q2 > q1) { out = s.substring(q1+1, q2); return true; }
            // number
            int e = s.indexOf(',', c); if (e < 0) e = s.indexOf('}', c);
            if (e < 0) return false; out = s.substring(c+1, e); out.trim(); return true;
        };
        String epochStr, tzStr, tokenStr;
        if (!extract(body, "epochMs", epochStr) || !extract(body, "tzOffsetMin", tzStr) || !extract(body, "token", tokenStr)) {
            server.send(400, "text/plain", "Invalid JSON"); return;
        }
        int64_t epochMs = atoll(epochStr.c_str());
        int tzOffsetMin = atoi(tzStr.c_str());
        std::string token = tokenStr.c_str();

        bool ok = false;
        if (g_time_manager && g_time_provider) {
            // Validate token against our one-time token_
            TimeSyncLogic logic;
            // begin で時間窓開始（60s）し、期待トークンを our token_ に固定
            ArduinoRandomProvider rnd;
            logic.begin(&rnd, g_time_manager, 60000);
            logic.setExpectedToken(token_.c_str());
            if (token == token_.c_str()) {
                if (logic.handleTimeSetRequest(epochMs, tzOffsetMin, token, g_time_manager, g_time_provider)) {
                    server.send(200, "text/plain", "Time applied");
                    ok = true;
                } else {
                    server.send(400, "text/plain", logic.getErrorMessage());
                }
            } else {
                server.send(403, "text/plain", "invalid_token");
            }
        } else {
            server.send(500, "text/plain", "No time adapters");
        }
        if (ok) { server.stop(); WiFi.softAPdisconnect(true); WiFi.mode(WIFI_OFF); status_ = Status::AppliedOk; }
    });
    server.begin();
#endif
    running_ = true;
    status_ = Status::Step1;
}

void SoftApTimeSyncController::cancel() {
#ifdef ARDUINO
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);
#endif
    running_ = false;
    status_ = Status::Idle;
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

auto SoftApTimeSyncController::getStatus() const -> Status {
    return status_;
}

void SoftApTimeSyncController::getUrlPayload(std::string& outUrl) const {
#ifdef ARDUINO
    const IPAddress ip = WiFi.softAPIP();
    outUrl = TimeSyncCore::buildUrl(ip.toString().c_str(), token_);
#else
    outUrl = TimeSyncCore::buildUrl("192.168.4.1", token_);
#endif
}

const char* SoftApTimeSyncController::getErrorMessage() const {
    return lastError_.c_str();
}


