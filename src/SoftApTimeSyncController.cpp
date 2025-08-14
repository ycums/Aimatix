#include "SoftApTimeSyncController.h"

#include "TimeSyncLogic.h"
#include "ITimeService.h"
#include "M5TimeService.h"
#include "ArduinoRandomProvider.h"
#include "TimeSyncCore.h"
#include "TimeZoneUtil.h"

#ifdef ARDUINO
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <esp_timer.h>
static WebServer server(80);
static DNSServer dnsServer; // Captive: wildcard DNS → AP IP
extern ITimeService* g_time_service;   // provided in main.cpp
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
    logic_.begin(&rnd, g_time_service, 60000);
    ssid_  = logic_.getCredentials().ssid;
    psk_   = logic_.getCredentials().psk;
    token_ = logic_.getCredentials().token;
    WiFi.mode(WIFI_AP);
    // PSKは最低8文字必要
    WiFi.softAP(ssid_.c_str(), psk_.c_str());

    // Captive DNS: 任意ホスト名をAP IPへ解決
    {
        const IPAddress apIp = WiFi.softAPIP();
        dnsServer.setTTL(1);
        dnsServer.start(53, "*", apIp);
    }

    // Register station-connected event → Step2 昇格
    WiFi.onEvent([this](WiFiEvent_t /*event*/, WiFiEventInfo_t /*info*/) {
        logic_.onStationConnected();
    }, ARDUINO_EVENT_WIFI_AP_STACONNECTED);
    // Setup minimal routes for Step2 (sync + time/set) and captive redirects
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

    // iOS接続性チェック経路（CNAトリガ目的で302→/sync）。Hostは任意（DNSで自IPへ）。
    server.on("/hotspot-detect.html", HTTP_GET, [this]() {
        String loc = "/sync?t=";
        loc += token_.c_str();
        server.sendHeader("Location", loc, true);
        server.send(302, "text/plain", "");
    });
    // 汎用success系・Windows/NCSI等も302へ（最小セット）
    server.on("/success.txt", HTTP_GET, [this]() {
        String loc = "/sync?t=";
        loc += token_.c_str();
        server.sendHeader("Location", loc, true);
        server.send(302, "text/plain", "");
    });
    server.on("/success.html", HTTP_GET, [this]() {
        String loc = "/sync?t=";
        loc += token_.c_str();
        server.sendHeader("Location", loc, true);
        server.send(302, "text/plain", "");
    });
    server.on("/ncsi.txt", HTTP_GET, [this]() {
        String loc = "/sync?t=";
        loc += token_.c_str();
        server.sendHeader("Location", loc, true);
        server.send(302, "text/plain", "");
    });
    // Android系の /generate_204 は204で返すとCNAは開かないが、iOS優先のため302で誘導
    server.on("/generate_204", HTTP_GET, [this]() {
        String loc = "/sync?t=";
        loc += token_.c_str();
        server.sendHeader("Location", loc, true);
        server.send(302, "text/plain", "");
    });

    // ルート/未知パスは一律 /sync へリダイレクト
    server.on("/", HTTP_GET, [this]() {
        String loc = "/sync?t=";
        loc += token_.c_str();
        server.sendHeader("Location", loc, true);
        server.send(302, "text/plain", "");
    });
    server.onNotFound([this]() {
        String loc = "/sync?t=";
        loc += token_.c_str();
        server.sendHeader("Location", loc, true);
        server.send(302, "text/plain", "");
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
        if (g_time_service) {
            // Validate token against our one-time token_
            logic_.setExpectedToken(token_.c_str());

            
            if (token == token_.c_str()) {
                if (logic_.handleTimeSetRequest(epochMs, tzOffsetMin, token, g_time_service)) {
                    // Apply TZ immediately so localtime reflects smartphone's locale
                    #ifdef ARDUINO
                    const std::string tz = TimeZoneUtil::buildPosixTzFromOffsetMinutes(tzOffsetMin);
                    setenv("TZ", tz.c_str(), 1);
                    tzset();
                    #endif
                    server.send(200, "text/plain", "Time applied");
                    ok = true;
                } else {
                    const char* code = logic_.getErrorMessage();
                    int status = 400;
                    const char* msg = "BAD REQUEST";
                    if (strcmp(code, "window_expired") == 0) { status = 401; msg = "AP WINDOW EXPIRED"; }
                    else if (strcmp(code, "invalid_token") == 0) { status = 403; msg = "TOKEN MISMATCH"; }
                    else if (strcmp(code, "rate_limited") == 0) { status = 403; msg = "RATE LIMITED"; }
                    else if (strcmp(code, "time_out_of_range") == 0) { status = 422; msg = "TIME OUT OF RANGE"; }
                    else if (strcmp(code, "tz_offset_out_of_range") == 0) { status = 422; msg = "TZ OFFSET OUT OF RANGE"; }
                    else if (strcmp(code, "apply_failed") == 0) { status = 500; msg = "APPLY FAILED"; }
                    server.send(status, "text/plain", msg);
                    
                }
            } else {
                server.send(403, "text/plain", "TOKEN MISMATCH");
                
            }
        } else {
            server.send(500, "text/plain", "No time adapters");
            
        }
        if (ok) {
            if (windowTimer_ != nullptr) {
                esp_timer_handle_t h = reinterpret_cast<esp_timer_handle_t>(windowTimer_);
                esp_timer_stop(h);
                esp_timer_delete(h);
                windowTimer_ = nullptr;
            }
            dnsServer.stop();
            server.stop();
            WiFi.softAPdisconnect(true);
            WiFi.mode(WIFI_OFF);
            delay(50);
            running_ = false;
        }
    });
    server.begin();
#ifdef ARDUINO
    // Arm AP stop at window end (success path stops earlier)
    {
        const uint32_t nowMs = millis();
        const uint32_t remain = logic_.getWindowRemainingMs(nowMs);
        esp_timer_create_args_t args{};
        args.callback = [](void* /*arg*/){ dnsServer.stop(); server.stop(); WiFi.softAPdisconnect(true); WiFi.mode(WIFI_OFF); };
        args.dispatch_method = ESP_TIMER_TASK;
        args.name = "ts_window";
        esp_timer_handle_t h = nullptr;
        if (esp_timer_create(&args, &h) == ESP_OK) {
            esp_timer_start_once(h, static_cast<uint64_t>(remain) * 1000ULL);
            // Keep handle to cancel on success/cancel
            windowTimer_ = reinterpret_cast<void*>(h);
        }
    }
#endif
#endif
    running_ = true;
}

void SoftApTimeSyncController::cancel() {
#ifdef ARDUINO
    if (windowTimer_ != nullptr) {
        esp_timer_handle_t h = reinterpret_cast<esp_timer_handle_t>(windowTimer_);
        esp_timer_stop(h);
        esp_timer_delete(h);
        windowTimer_ = nullptr;
    }
    dnsServer.stop();
    server.stop();
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);
    delay(50);
#endif
    running_ = false;
}

void SoftApTimeSyncController::loopTick() {
#ifdef ARDUINO
    // Handle incoming HTTP requests for /sync and /time/set
    server.handleClient();
    // Process DNS queries for captive portal
    dnsServer.processNextRequest();

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


