#include "SoftApTimeSyncController.h"

#include "TimeSyncLogic.h"
#include "ITimeManager.h"
#include "ITimeProvider.h"
#include "ArduinoRandomProvider.h"
#include "TimeSyncCore.h"
#include "TimeZoneUtil.h"

#ifdef ARDUINO
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
static WebServer server(80);
extern ITimeManager* g_time_manager;   // provided in main.cpp or platform layer
extern ITimeProvider* g_time_provider; // provided in main.cpp or platform layer
#endif

#include <cstdint>

namespace { }

SoftApTimeSyncController::SoftApTimeSyncController()
    : running_(false) {}

void SoftApTimeSyncController::begin() {
#ifdef ARDUINO
    // WiFi スタックの再初期化で安定性向上（netstack init errors回避）
    WiFi.mode(WIFI_OFF);
    delay(100);
    // Start pure session logic (generates credentials)
    ArduinoRandomProvider rnd;
    logic_.begin(&rnd, g_time_manager, 60000);
    ssid_  = logic_.getCredentials().ssid;
    psk_   = logic_.getCredentials().psk;
    token_ = logic_.getCredentials().token;
    WiFi.mode(WIFI_AP);
    // PSKは最低8文字必要
    WiFi.softAP(ssid_.c_str(), psk_.c_str());

    // Register station-connected event → Step2 昇格
    WiFi.onEvent([this](WiFiEvent_t /*event*/, WiFiEventInfo_t /*info*/) {
        logic_.onStationConnected();
    }, ARDUINO_EVENT_WIFI_AP_STACONNECTED);
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
        // Use pure JSON extractors from TimeSyncCore
        std::string bodyStd(body.c_str());
        int64_t epochMs = 0;
        int tzOffsetMin = 0;
        std::string tokenStd;
        bool okEpoch = TimeSyncCore::jsonExtractInt64(bodyStd, "epochMs", epochMs);
        bool okTz = TimeSyncCore::jsonExtractInt(bodyStd, "tzOffsetMin", tzOffsetMin);
        bool okToken = TimeSyncCore::jsonExtractString(bodyStd, "token", tokenStd);
        if (!okEpoch || !okTz || !okToken) { server.send(400, "text/plain", "Invalid JSON"); return; }
        std::string token = tokenStd;

        bool ok = false;
        if (g_time_manager && g_time_provider) {
            // Validate token against our one-time token_
            logic_.setExpectedToken(token_.c_str());

            
            if (token == token_.c_str()) {
                if (logic_.handleTimeSetRequest(epochMs, tzOffsetMin, token, g_time_manager, g_time_provider)) {
                    // Apply TZ immediately so localtime reflects smartphone's locale
                    #ifdef ARDUINO
                    const std::string tz = TimeZoneUtil::buildPosixTzFromOffsetMinutes(tzOffsetMin);
                    setenv("TZ", tz.c_str(), 1);
                    tzset();
                    #endif
                    server.send(200, "text/plain", "Time applied");
                    ok = true;
                } else {
                    server.send(400, "text/plain", logic_.getErrorMessage());
                    
                }
            } else {
                server.send(403, "text/plain", "invalid_token");
                
            }
        } else {
            server.send(500, "text/plain", "No time adapters");
            
        }
        if (ok) { server.stop(); WiFi.softAPdisconnect(true); WiFi.mode(WIFI_OFF); }
    });
    server.begin();
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
#ifdef ARDUINO
    // Handle incoming HTTP requests for /sync and /time/set
    server.handleClient();

    // Fallback: station count check → Step2
    if (logic_.getStatus() == TimeSyncLogic::Status::Step1) {
        const int stations = WiFi.softAPgetStationNum();
        if (stations > 0) {
            logic_.onStationConnected();
        }
    }
#endif
}

void SoftApTimeSyncController::reissue() {
#ifdef ARDUINO
    ArduinoRandomProvider rnd;
    logic_.reissue(&rnd);
    ssid_  = logic_.getCredentials().ssid;
    psk_   = logic_.getCredentials().psk;
    token_ = logic_.getCredentials().token;
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
    switch (logic_.getStatus()) {
        case TimeSyncLogic::Status::Idle: return Status::Idle;
        case TimeSyncLogic::Status::Step1: return Status::Step1;
        case TimeSyncLogic::Status::Step2: return Status::Step2;
        case TimeSyncLogic::Status::AppliedOk: return Status::AppliedOk;
        case TimeSyncLogic::Status::Error: return Status::Error;
    }
    return Status::Idle;
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
    return logic_.getErrorMessage();
}


