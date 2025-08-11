#include "TimeSyncViewImpl.h"
#include "DisplayCommon.h"
#include "ui_constants.h"
#include <string>
#include <algorithm>
#ifdef ARDUINO
#include <M5Unified.h>
extern "C" {
#include <qrcode.h>
}
#endif
// QR表示は仕様通り ricmoo/QRCode + M5GFX を用いるが、実装詳細は Adapter 経由へ委譲可能性あり。

void TimeSyncViewImpl::showTitle(const char* text) {
    if (adapter_ == nullptr) return;
    // 画面初期化は入場時の1回のみ呼ばれる想定
    adapter_->clear();
    constexpr int BATTERY_LEVEL_PLACEHOLDER = 42;
    const bool IS_CHARGING_PLACEHOLDER = false;
    drawTitleBar(adapter_, text, BATTERY_LEVEL_PLACEHOLDER, IS_CHARGING_PLACEHOLDER);
}

void TimeSyncViewImpl::showHints(const char* hintA, const char* hintB, const char* hintC) {
    if (adapter_ == nullptr) return;
    drawButtonHintsGrid(adapter_, hintA, hintB, hintC);
}

void TimeSyncViewImpl::showWifiQr(const char* payload) {
    if (adapter_ == nullptr || payload == nullptr) return;
    // 仕様: ECC LOW, quiet zone=2, 上余白10px, 黒背景/アンバードット
    // 現時点では DisplayAdapter にQR APIが無いため、ここで直接描画する。
    // 実装を後で Adapter へ移譲可能。

#ifdef ARDUINO
    QRCode qrcode;
    // サイズ決定（Wi‑Fi文字列は比較的長め。version 7目安）
    const uint8_t version = 7;
    const uint8_t ecc = 0; // ECC_LOW
    const size_t qrSize = qrcode_getBufferSize(version);
    uint8_t* qrcodeData = (uint8_t*)malloc(qrSize);
    if (!qrcodeData) return;
    if (qrcode_initText(&qrcode, qrcodeData, version, ecc, payload) != 0) {
        free(qrcodeData);
        return;
    }
    const int moduleCount = qrcode.size;
    const int quietZone = 2;
    // 最大スケールを計算（画面幅・高さから算出）
    const int screenW = M5.Display.width();
    const int screenH = M5.Display.height();
    const int availableH = screenH - (TITLE_HEIGHT+ HINT_HEIGHT);
    const int topMargin = TITLE_HEIGHT;
    const int maxModules = moduleCount + quietZone * 2;
    const int scale = std::max(1, std::min(screenW / maxModules, availableH / maxModules));
    const int qrW = maxModules * scale;
    const int startX = (screenW - qrW) / 2;
    const int startY = topMargin + (availableH - qrW) / 2;

    // 背景クリア（タイトルバーは保持）
    M5.Display.fillRect(0, topMargin, screenW, availableH, TFT_BLACK);

    // 描画
    for (int y = 0; y < moduleCount; y++) {
        for (int x = 0; x < moduleCount; x++) {
            const bool dot = qrcode_getModule(&qrcode, x, y);
            const int px = startX + (x + quietZone) * scale;
            const int py = startY + (y + quietZone) * scale;
            if (dot) {
                M5.Display.fillRect(px, py, scale, scale, AMBER_COLOR);
            }
        }
    }
    free(qrcodeData);
#else
    (void)payload;
#endif
}


