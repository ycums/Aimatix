#include "main_display.h"
#include <stdio.h>
#include <string.h>
#include <vector>
#include <string>
#include <ctime>
#include "AlarmLogic.h"

void drawTitleBar(IDisplay* disp, const char* modeName, int batteryLevel, bool isCharging) {
    disp->fillRect(0, TITLE_HEIGHT - 1, SCREEN_WIDTH, 1, AMBER_COLOR); // 横線
    disp->setTextFont(FONT_AUXILIARY);
    disp->setTextColor(AMBER_COLOR, TFT_BLACK);
    disp->setTextDatum(TL_DATUM);
    disp->drawText(5, 2, modeName, FONT_AUXILIARY);
    char batteryStr[16];
    snprintf(batteryStr, sizeof(batteryStr), "%s %d%%", isCharging ? "CHG" : "BAT", batteryLevel);
    disp->drawText(SCREEN_WIDTH - 70, 2, batteryStr, FONT_AUXILIARY);
}

void drawButtonHintsGrid(IDisplay* disp, const char* btnA, const char* btnB, const char* btnC) {
    disp->fillRect(0, SCREEN_HEIGHT - HINT_HEIGHT, SCREEN_WIDTH, 1, AMBER_COLOR); // 横線
    disp->setTextFont(FONT_AUXILIARY);
    disp->setTextColor(AMBER_COLOR, TFT_BLACK);
    disp->setTextDatum(TL_DATUM);
    if (btnA) disp->drawText(20, SCREEN_HEIGHT - HINT_HEIGHT + 2, btnA, FONT_AUXILIARY);
    if (btnB) disp->drawText(SCREEN_WIDTH / 2 - 30, SCREEN_HEIGHT - HINT_HEIGHT + 2, btnB, FONT_AUXILIARY);
    if (btnC) disp->drawText(SCREEN_WIDTH - 80, SCREEN_HEIGHT - HINT_HEIGHT + 2, btnC, FONT_AUXILIARY);
}

void fillProgressBarSprite(IDisplay* disp, int x, int y, int w, int h, int percent) {
    disp->fillRect(x, y, w, h, TFT_BLACK);
    disp->fillRect(x, y, w, 1, AMBER_COLOR); // 上枠
    disp->fillRect(x, y + h - 1, w, 1, AMBER_COLOR); // 下枠
    disp->fillRect(x, y, 1, h, AMBER_COLOR); // 左枠
    disp->fillRect(x + w - 1, y, 1, h, AMBER_COLOR); // 右枠
    int fillW = (w - 2) * percent / 100;
    if (fillW > 0) {
        disp->fillRect(x + 1, y + 1, fillW, h - 2, AMBER_COLOR);
    }
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
    drawTitleBar(disp, modeName, batteryLevel, isCharging);

    // --- ボタンヒント ---
    drawButtonHintsGrid(disp, "ABS", "REL+", "MGMT");

    // --- 現在時刻（中央寄せ・Font4）---
    disp->setTextFont(FONT_IMPORTANT);
    disp->setTextDatum(MC_DATUM);
    disp->drawText(SCREEN_WIDTH/2, GRID_Y(2) + GRID_HEIGHT, currentTime, FONT_IMPORTANT);

    // --- 残り時間（中央寄せ・Font7）---
    disp->setTextDatum(TC_DATUM);
    disp->setTextFont(FONT_MAIN);
    disp->drawText(SCREEN_WIDTH/2, GRID_Y(4), remainTime, FONT_MAIN);

    // --- プログレスバー（グリッドセル(0,6)-(15,7)）---
    const int progressBarHeight = 8;
    fillProgressBarSprite(disp,
        GRID_X(0),
        GRID_Y(7),
        SCREEN_WIDTH,
        progressBarHeight,
        progressPercent
    );

    // --- アラームリスト（最大5件・Font2）---
    const int alermColStep = (14 * GRID_WIDTH / 5);
    disp->setTextFont(FONT_AUXILIARY);
    disp->setTextDatum(MC_DATUM);
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
            disp->drawText(x, y, alarmStrs[i].c_str(), FONT_AUXILIARY);
        } else {
            disp->fillRect(x - clearW/2, y - clearH/2, clearW, clearH, TFT_BLACK);
        }
    }
    disp->setTextDatum(TL_DATUM); // 以降は左上基準に戻す
}

void drawGridLines() {
    // 縦線（17本）
    for (int i = 0; i <= 16; ++i) {
        int x = i * GRID_WIDTH;
        if (disp->drawRect) disp->drawRect(x, 0, 1, SCREEN_HEIGHT);
    }
    // 横線（13本）
    for (int i = 0; i <= 12; ++i) {
        int y = TITLE_HEIGHT + i * GRID_HEIGHT;
        if (disp->drawRect) disp->drawRect(0, y, SCREEN_WIDTH, 1);
    }
} 