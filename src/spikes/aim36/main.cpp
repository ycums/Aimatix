#include <M5Unified.h>
#include <M5GFX.h>
#include <WiFi.h>
#include <WebServer.h>
#include <esp_system.h>
#include <sys/time.h>
#include <qrcode.h>
#include <memory>

namespace {
  constexpr uint32_t AP_WINDOW_MS = 60 * 1000;
  constexpr int QR_MARGIN_MODULES = 2;  // 周囲マージン（モジュール単位）
  constexpr int QR_TOP_MARGIN_PX = 10;  // 画面上端の余白（メッセージ用）

  WebServer server(80);

  String apSsid;
  String apPsk;
  String oneTimeToken;
  uint32_t apStartMs = 0;
  bool timeApplied = false;
  int64_t lastReceivedEpochMs = 0;
  int lastReceivedTzOffsetMin = 0;

  // 接続イベントでURL QRへ切替
  volatile bool stationConnected = false;

  String generateRandomHex(size_t numBytes) {
    String out;
    out.reserve(numBytes * 2);
    static const char* hex = "0123456789ABCDEF";
    for (size_t i = 0; i < numBytes; ++i) {
      uint8_t b = static_cast<uint8_t>(esp_random() & 0xFF);
      out += hex[(b >> 4) & 0x0F];
      out += hex[b & 0x0F];
    }
    return out;
  }

  void drawQrPayload(const String& payload, const char* /*title*/, int version) {
    // QR生成（指定バージョンで余裕を確保）
    version = std::max(1, std::min(10, version));
    QRCode qrc;
    const size_t bufSize = qrcode_getBufferSize(version);
    std::unique_ptr<uint8_t[]> qrbuff(new uint8_t[bufSize]);
    qrcode_initText(&qrc, qrbuff.get(), version, ECC_LOW, payload.c_str());

    const int modules = qrc.size; // 17 + 4*version

    auto& d = M5.Display;
    d.fillScreen(TFT_BLACK);

    // 画面に収まる最大スケールを計算
    const int maxW = d.width();
    const int maxH = d.height() - QR_TOP_MARGIN_PX; // 上端余白を除外
    int scale = std::min((maxW - QR_MARGIN_MODULES * 2) / modules,
                         (maxH - QR_MARGIN_MODULES * 2) / modules);
    scale = std::max(2, std::min(10, scale));
    const int margin = QR_MARGIN_MODULES * scale;
    const int qrPix = modules * scale;
    const int rectW = qrPix + margin * 2;
    const int rectH = qrPix + margin * 2;
    const int rectX = (d.width() - rectW) / 2;
    const int rectY = QR_TOP_MARGIN_PX + ( (d.height() - QR_TOP_MARGIN_PX) - rectH ) / 2;
    const int x0 = rectX + margin;
    const int y0 = rectY + margin;

    d.startWrite();
    for (int y = 0; y < modules; ++y) {
      for (int x = 0; x < modules; ++x) {
        const bool on = qrcode_getModule(&qrc, x, y);
        const int px = x0 + x * scale;
        const int py = y0 + y * scale;
        d.fillRect(px, py, scale, scale, on ? TFT_ORANGE : TFT_BLACK);
      }
    }
    d.endWrite();
  }

  String buildSyncHtml(const String& token) {
    String html;
    html.reserve(2048);
    html += F("<!doctype html><html><head><meta charset=\"utf-8\">"
              "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
              "<title>AIM-36 Time Sync</title></head><body>"
              "<pre id='log'>Sync starting...</pre>"
              "<script>");
    html += "const TOKEN='";
    html += token;
    html += F("';\n"
              "const epochMs=Date.now();\n"
              "const tzOffsetMin = -new Date().getTimezoneOffset();\n"
              "fetch('/time/set',{method:'POST',headers:{'Content-Type':'application/json'},"
              "body: JSON.stringify({epochMs, tzOffsetMin, token:TOKEN})})"
              ".then(async r=>{document.getElementById('log').textContent="
              "r.ok?('OK\\n'+await r.text()):('ERR '+r.status+'\\n'+await r.text());})"
              ".catch(e=>{document.getElementById('log').textContent='ERR\\n'+e;});"
              "</script></body></html>");
    return html;
  }

  void handleGetSync() {
    const String t = server.hasArg("t") ? server.arg("t") : String();
    if (t != oneTimeToken) {
      server.send(403, "text/plain", "Forbidden");
      return;
    }
    server.send(200, "text/html", buildSyncHtml(oneTimeToken));
  }

  bool extractJsonNumber(const String& body, const char* key, int64_t& out) {
    String k = String("\"") + key + String("\"");
    int ks = body.indexOf(k);
    if (ks < 0) return false;
    int colon = body.indexOf(':', ks + k.length());
    if (colon < 0) return false;
    int i = colon + 1;
    while (i < (int)body.length() && isspace((unsigned char)body[i])) ++i;
    bool neg = false;
    if (i < (int)body.length() && (body[i] == '-' || body[i] == '+')) {
      neg = (body[i] == '-');
      ++i;
    }
    int64_t val = 0;
    bool any = false;
    while (i < (int)body.length() && isdigit((unsigned char)body[i])) {
      any = true;
      val = val * 10 + (body[i] - '0');
      ++i;
    }
    if (!any) return false;
    out = neg ? -val : val;
    return true;
  }

  bool extractJsonString(const String& body, const char* key, String& out) {
    String k = String("\"") + key + String("\"");
    int ks = body.indexOf(k);
    if (ks < 0) return false;
    int colon = body.indexOf(':', ks + k.length());
    if (colon < 0) return false;
    int firstQuote = body.indexOf('"', colon + 1);
    if (firstQuote < 0) return false;
    int secondQuote = body.indexOf('"', firstQuote + 1);
    if (secondQuote < 0) return false;
    out = body.substring(firstQuote + 1, secondQuote);
    return true;
  }

  String formatOffsetHHMM(int tzOffsetMin) {
    const int sign = tzOffsetMin >= 0 ? 1 : -1;
    int m = tzOffsetMin * sign;
    const int hh = m / 60;
    const int mm = m % 60;
    char buf[8];
    snprintf(buf, sizeof(buf), "%c%02d%02d", sign > 0 ? '+' : '-', hh, mm);
    return String(buf);
  }

  String formatLocalTimeFromEpochMs(int64_t epochMs, int tzOffsetMin) {
    time_t t = static_cast<time_t>(epochMs / 1000);
    t += static_cast<time_t>(tzOffsetMin * 60);
    struct tm tmv;
    gmtime_r(&t, &tmv);
    char buf[32];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
             tmv.tm_year + 1900, tmv.tm_mon + 1, tmv.tm_mday,
             tmv.tm_hour, tmv.tm_min, tmv.tm_sec);
    return String(buf);
  }

  bool applyTimeEpochMs(int64_t epochMs, int /*tzOffsetMin*/) {
    const time_t sec = static_cast<time_t>(epochMs / 1000);
    const suseconds_t usec = static_cast<suseconds_t>((epochMs % 1000) * 1000);
    timeval tv{sec, usec};
    return settimeofday(&tv, nullptr) == 0;
  }

  void handlePostTimeSet() {
    if (!server.hasArg("plain")) {
      server.send(400, "text/plain", "Bad Request");
      return;
    }
    const String body = server.arg("plain");

    int64_t epochMs = 0;
    int64_t tzOffsetMin64 = 0;
    String token;
    if (!extractJsonNumber(body, "epochMs", epochMs) ||
        !extractJsonNumber(body, "tzOffsetMin", tzOffsetMin64) ||
        !extractJsonString(body, "token", token)) {
      server.send(400, "text/plain", "Invalid JSON");
      return;
    }
    if (token != oneTimeToken) {
      server.send(403, "text/plain", "Forbidden");
      return;
    }
    const uint32_t nowMs = millis();
    if (nowMs - apStartMs > AP_WINDOW_MS) {
      server.send(408, "text/plain", "AP window expired");
      return;
    }

    lastReceivedEpochMs = epochMs;
    lastReceivedTzOffsetMin = static_cast<int>(tzOffsetMin64);
    const bool ok = applyTimeEpochMs(epochMs, lastReceivedTzOffsetMin);
    timeApplied = ok;
    if (ok) {
      server.send(200, "text/plain", "Time applied");
    } else {
      server.send(500, "text/plain", "Failed to apply time");
    }
  }

  void setupRoutes() {
    server.on("/sync", HTTP_GET, handleGetSync);
    server.on("/time/set", HTTP_POST, handlePostTimeSet);
  }

  void showWifiQr() {
    const String wifiQr = String("WIFI:T:WPA;S:") + apSsid + ";P:" + apPsk + ";H:false;;";
    // Wi‑Fi文字列は長くなりがちなので version=7 程度で余裕を持たせる
    drawQrPayload(wifiQr, "Step1: Join AP", 7);
    auto& d = M5.Display;
    d.setTextColor(TFT_ORANGE, TFT_BLACK);
    d.setTextSize(2);
    d.setCursor(8, 2);
    d.printf("Step1: Join AP");
  }

  void showUrlQr() {
    const IPAddress ip = WiFi.softAPIP();
    const String url = String("http://") + ip.toString() + "/sync?t=" + oneTimeToken;
    // URLは比較的短いので version=5 程度
    drawQrPayload(url, "Step2: Open URL", 5);
    auto& d = M5.Display;
    d.setTextColor(TFT_ORANGE, TFT_BLACK);
    d.setTextSize(2);
    d.setCursor(8, 2);
    d.printf("Step2: Open URL");
  }

  void startSoftAP() {
    apSsid = "AIMATIX-" + generateRandomHex(3);
    apPsk  = generateRandomHex(8);
    oneTimeToken = generateRandomHex(8);

    WiFi.mode(WIFI_AP);
    // channel=1, hidden=0, max_connection=1
    WiFi.softAP(apSsid.c_str(), apPsk.c_str(), 1, 0, 1);
    delay(100);

    apStartMs = millis();
    showWifiQr();
  }

  void stopSoftAP() {
    server.stop();
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);
  }

  void onWiFiEvent(WiFiEvent_t event) {
    if (event == ARDUINO_EVENT_WIFI_AP_STACONNECTED) {
      stationConnected = true;
    }
  }
}

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  M5.Display.setBrightness(200);

  WiFi.onEvent(onWiFiEvent);

  startSoftAP();

  setupRoutes();
  server.begin();
}

void loop() {
  M5.update();
  server.handleClient();

  const uint32_t nowMs = millis();

  if (!timeApplied && (nowMs - apStartMs > AP_WINDOW_MS)) {
    stopSoftAP();
    auto& d = M5.Display;
    d.fillScreen(TFT_BLACK);
    d.setTextColor(TFT_ORANGE, TFT_BLACK);
    d.setTextSize(2);
    d.setCursor(8, 8);
    d.printf("AP window expired.\n");
    delay(1500);
    return;
  }

  if (stationConnected) {
    stationConnected = false;
    showUrlQr();
  }

  if (timeApplied) {
    stopSoftAP();
    auto& d = M5.Display;
    d.fillScreen(TFT_BLACK);
    d.setTextColor(TFT_ORANGE, TFT_BLACK);
    d.setTextSize(2);
    d.setCursor(8, 8);
    d.printf("SYNC OK\nAP closed\n\n");
    d.setTextSize(2);
    if (lastReceivedEpochMs > 0) {
      const String local = formatLocalTimeFromEpochMs(lastReceivedEpochMs, lastReceivedTzOffsetMin);
      const String tz = formatOffsetHHMM(lastReceivedTzOffsetMin);
      d.printf("Local: %s\nTZ: %s\n", local.c_str(), tz.c_str());
    }
    delay(1500);
    return;
  }
}


