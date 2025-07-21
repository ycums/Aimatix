#include "main_display.h"
#include "TimeLogic.h"
#include <stdio.h>
#include <string.h>
#include <vector>
#include <ctime>
#include <string>
#include "AlarmLogic.h"
extern std::vector<time_t> alarmTimes;

// --- Adapter層で差し替え可能な関数ポインタ ---
static void (*drawRect_impl)(int, int, int, int) = nullptr;
static void (*drawString_impl)(const char*, int, int) = nullptr;
static void (*fillProgressBar_impl)(int, int, int, int, int) = nullptr;
static void (*setFont_impl)(int) = nullptr;
static void (*setTextDatum_impl)(int) = nullptr;
static void (*fillRect_impl)(int, int, int, int, int) = nullptr;
static void (*fillProgressBarSprite_impl)(int, int, int, int, int) = nullptr;

void setDrawRectImpl(void (*impl)(int, int, int, int)) { drawRect_impl = impl; }
void setDrawStringImpl(void (*impl)(const char*, int, int)) { drawString_impl = impl; }
void setFillProgressBarImpl(void (*impl)(int, int, int, int, int)) { fillProgressBar_impl = impl; }
void setFontImpl(void (*impl)(int)) { setFont_impl = impl; }
void setTextDatumImpl(void (*impl)(int)) { setTextDatum_impl = impl; }
void setFillRectImpl(void (*impl)(int, int, int, int, int)) { fillRect_impl = impl; }
void setFillProgressBarSpriteImpl(void (*impl)(int, int, int, int, int)) { fillProgressBarSprite_impl = impl; }

void drawRect(int x, int y, int w, int h) {
    if (drawRect_impl) drawRect_impl(x, y, w, h);
}
void drawString(const char* str, int x, int y) {
    if (drawString_impl) drawString_impl(str, x, y);
}
void fillProgressBar(int x, int y, int w, int h, int percent) {
    if (fillProgressBar_impl) fillProgressBar_impl(x, y, w, h, percent);
}
void fillProgressBarSprite(int x, int y, int w, int h, int percent) {
    if (fillProgressBarSprite_impl) fillProgressBarSprite_impl(x, y, w, h, percent);
    else if (fillProgressBar_impl) fillProgressBar_impl(x, y, w, h, percent); // フォールバック
}
void setFont(int font) {
    if (setFont_impl) setFont_impl(font);
}
void setTextDatum(int datum) { if (setTextDatum_impl) setTextDatum_impl(datum); }
void fillRect(int x, int y, int w, int h, int color) {
    if (fillRect_impl) fillRect_impl(x, y, w, h, color);
}

// --- タイトルバー描画 ---
void drawTitleBar(const char* modeName, int batteryLevel, bool isCharging) {
    // 下ボーダーのみ（fillRectで1px線）
    if (drawRect_impl) drawRect_impl(0, TITLE_HEIGHT - 1, SCREEN_WIDTH, 1); // 横線
    // モード名（左）
    setFont(FONT_AUXILIARY);
    drawString(modeName, 5, 2);
    // バッテリー表示（右）
    char batteryStr[16];
    snprintf(batteryStr, sizeof(batteryStr), "%s %d%%", isCharging ? "CHG" : "BAT", batteryLevel);
    drawString(batteryStr, SCREEN_WIDTH - 70, 2);
}

// --- ボタンヒント描画 ---
void drawButtonHintsGrid(const char* btnA, const char* btnB, const char* btnC) {
    // 上ボーダーのみ（fillRectで1px線）
    if (drawRect_impl) drawRect_impl(0, SCREEN_HEIGHT - HINT_HEIGHT, SCREEN_WIDTH, 1); // 横線
    setFont(FONT_AUXILIARY);
    if (btnA) drawString(btnA, 20, SCREEN_HEIGHT - HINT_HEIGHT + 2);
    if (btnB) drawString(btnB, SCREEN_WIDTH / 2 - 30, SCREEN_HEIGHT - HINT_HEIGHT + 2);
    if (btnC) drawString(btnC, SCREEN_WIDTH - 80, SCREEN_HEIGHT - HINT_HEIGHT + 2);
}

// --- メイン画面描画 ---
void drawMainDisplay() {
    static time_t lastAlarmStart = 0;
    static int lastAlarmTotalSec = 0;
    const char* modeName = "MAIN";
    int batteryLevel = 42;
    bool isCharging = false;
    char currentTime[16];
    char remainTime[16];
    int progressPercent = 0;

    // --- 現在時刻取得 ---
    time_t now = time(nullptr);
    struct tm* tm_now = localtime(&now);
    snprintf(currentTime, sizeof(currentTime), "%02d:%02d", tm_now->tm_hour, tm_now->tm_min);

    // --- アラームリストの消化 ---
    AlarmLogic::removePastAlarms(alarmTimes, now);

    // --- 残り時間・進捗計算 ---
    int remainSec = AlarmLogic::getRemainSec(alarmTimes, now);
    // アラームが変わったら開始時刻・初期残り時間を更新
    static time_t prevNextAlarm = 0;
    time_t nextAlarm = (!alarmTimes.empty()) ? alarmTimes.front() : 0;
    if (nextAlarm != prevNextAlarm) {
        lastAlarmStart = now;
        lastAlarmTotalSec = remainSec;
        prevNextAlarm = nextAlarm;
    }
    int totalSec = lastAlarmTotalSec > 0 ? lastAlarmTotalSec : 1;
    // 残り割合（右から左へ縮む）
    progressPercent = AlarmLogic::getRemainPercent(remainSec, totalSec);
    snprintf(remainTime, sizeof(remainTime), "%02d:%02d:%02d", remainSec/3600, (remainSec/60)%60, remainSec%60);
    if (alarmTimes.empty()) {
        snprintf(remainTime, sizeof(remainTime), "00:00:00");
        progressPercent = 0;
    }

    // --- タイトルバー ---
    drawTitleBar(modeName, batteryLevel, isCharging);

    // --- ボタンヒント ---
    drawButtonHintsGrid("ABS", "REL+", "MGMT");

    // --- 現在時刻（中央寄せ・Font4）---
    setFont(FONT_IMPORTANT);
    setTextDatum(MC_DATUM);
    drawString(currentTime, SCREEN_WIDTH/2, GRID_Y(2) + GRID_HEIGHT);

    // --- 残り時間（中央寄せ・Font7）---
    setTextDatum(TC_DATUM);
    setFont(FONT_MAIN);
    drawString(remainTime, SCREEN_WIDTH/2, GRID_Y(4));

    // --- プログレスバー（グリッドセル(0,6)-(15,7)）---
    const int progressBarHeight = 8;
    fillProgressBarSprite(
        GRID_X(0),
        GRID_Y(7),
        SCREEN_WIDTH,
        progressBarHeight,
        progressPercent
    );

    // --- アラームリスト（最大5件・Font2）---
    const int alermColStep = (14 * GRID_WIDTH / 5);
    setFont(FONT_AUXILIARY);
    setTextDatum(MC_DATUM);
    std::vector<std::string> alarmStrs;
    AlarmLogic::getAlarmTimeStrings(alarmTimes, alarmStrs);
    int alarmCount = alarmStrs.size();
    // Font2の高さ・幅を仮定（必要に応じて調整）
    const int clearW = 48; // 文字列幅の目安
    const int clearH = 24; // 文字列高さの目安
    for (int i = 0; i < 5; ++i) {
        int x = GRID_X(1) + i * alermColStep + alermColStep/2;
        int y = GRID_Y(9);
        if (i < alarmCount) {
            drawString(alarmStrs[i].c_str(), x, y);
        } else {
            fillRect(x - clearW/2, y - clearH/2, clearW, clearH, TFT_BLACK);
        }
    }
    setTextDatum(TL_DATUM); // 以降は左上基準に戻す
}

void drawGridLines() {
    // 縦線（17本）
    for (int i = 0; i <= 16; ++i) {
        int x = i * GRID_WIDTH;
        if (drawRect_impl) drawRect_impl(x, 0, 1, SCREEN_HEIGHT);
    }
    // 横線（13本）
    for (int i = 0; i <= 12; ++i) {
        int y = TITLE_HEIGHT + i * GRID_HEIGHT;
        if (drawRect_impl) drawRect_impl(0, y, SCREEN_WIDTH, 1);
    }
} 