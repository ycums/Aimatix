#include "main_display.h"
#include "TimeLogic.h"
#include <stdio.h>
#include <string.h>
#include <vector>
#include <ctime>
extern std::vector<time_t> alarmTimes;

// --- Adapter層で差し替え可能な関数ポインタ ---
static void (*drawRect_impl)(int, int, int, int) = nullptr;
static void (*drawString_impl)(const char*, int, int) = nullptr;
static void (*fillProgressBar_impl)(int, int, int, int, int) = nullptr;
static void (*setFont_impl)(int) = nullptr;
static void (*setTextDatum_impl)(int) = nullptr;

void setDrawRectImpl(void (*impl)(int, int, int, int)) { drawRect_impl = impl; }
void setDrawStringImpl(void (*impl)(const char*, int, int)) { drawString_impl = impl; }
void setFillProgressBarImpl(void (*impl)(int, int, int, int, int)) { fillProgressBar_impl = impl; }
void setFontImpl(void (*impl)(int)) { setFont_impl = impl; }
void setTextDatumImpl(void (*impl)(int)) { setTextDatum_impl = impl; }

void drawRect(int x, int y, int w, int h) {
    if (drawRect_impl) drawRect_impl(x, y, w, h);
}
void drawString(const char* str, int x, int y) {
    if (drawString_impl) drawString_impl(str, x, y);
}
void fillProgressBar(int x, int y, int w, int h, int percent) {
    if (fillProgressBar_impl) fillProgressBar_impl(x, y, w, h, percent);
}
void setFont(int font) {
    if (setFont_impl) setFont_impl(font);
}
void setTextDatum(int datum) { if (setTextDatum_impl) setTextDatum_impl(datum); }

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
    // 仮データ（固定値→動的に変更）
    const char* modeName = "MAIN";
    int batteryLevel = 42;
    bool isCharging = false;
    char currentTime[16];
    char remainTime[16];
    int progressPercent = 0;

    // --- 現在時刻取得 ---
    time_t now = time(nullptr);
    struct tm* tm_now = localtime(&now);
    snprintf(currentTime, sizeof(currentTime), "%02d:%02d:%02d", tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);

    // --- アラームリストの消化 ---
    // 過去のアラームは除去
    while (!alarmTimes.empty() && alarmTimes.front() <= now) {
        alarmTimes.erase(alarmTimes.begin());
    }

    // --- 残り時間・進捗計算 ---
    int remainSec = 0;
    int totalSec = 0;
    if (!alarmTimes.empty()) {
        remainSec = (int)(alarmTimes.front() - now);
        // 開始時点の残り時間（最初のアラーム追加時の値）を保持する必要があるが、
        // ここでは仮に+2分（120秒）とする
        totalSec = 120;
        progressPercent = (remainSec > 0 && totalSec > 0) ? (remainSec * 100 / totalSec) : 0;
        if (progressPercent < 0) progressPercent = 0;
        if (progressPercent > 100) progressPercent = 100;
        snprintf(remainTime, sizeof(remainTime), "%02d:%02d:%02d", remainSec/3600, (remainSec/60)%60, remainSec%60);
    } else {
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
    fillProgressBar(
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
    int alarmCount = alarmTimes.size();
    for (int i = 0; i < alarmCount && i < 5; ++i) {
        time_t t = alarmTimes[i];
        struct tm* tm_alarm = localtime(&t);
        char buf[16];
        snprintf(buf, sizeof(buf), "%02d:%02d:%02d", tm_alarm->tm_hour, tm_alarm->tm_min, tm_alarm->tm_sec);
        int x = GRID_X(1) + i * alermColStep + alermColStep/2;
        int y = GRID_Y(9);
        drawString(buf, x, y);
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