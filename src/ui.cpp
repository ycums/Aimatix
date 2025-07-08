#include "ui.h"
#include <M5Stack.h>
#include <time.h>

// Global variables used in UI functions (declare as extern in main.cpp or pass as parameters)
// extern Settings settings; // Assuming settings are needed for drawing
// extern std::vector<time_t> alarmTimes; // Assuming alarmTimes are needed for drawing
// extern Mode currentMode; // Assuming currentMode is needed for drawing

// ステータスバーの描画
void drawStatusBar(const char* mode) {
  M5.Lcd.fillRect(0, 0, 320, 20, TFT_BLACK);
  M5.Lcd.setTextSize(1);
  
  // モード名を表示
  M5.Lcd.setTextColor(AMBER_COLOR);
  M5.Lcd.drawString(mode, 5, 5, 2);
  
  // バッテリー情報を表示
  int batteryLevel = M5.Power.getBatteryLevel();
  String batteryString = String(batteryLevel) + "%";
  if (M5.Power.isCharging()) {
    batteryString = "⚡" + batteryString;
  }
  
  uint16_t color = (batteryLevel <= 20) ? FLASH_ORANGE : AMBER_COLOR;
  M5.Lcd.setTextColor(color);
  M5.Lcd.drawString(batteryString.c_str(), 280, 5, 2);
}

// ボタンヒントの描画
void drawButtonHints(const char* btnA, const char* btnB, const char* btnC) {
  M5.Lcd.fillRect(0, M5.Lcd.height() - 20, 320, 20, TFT_BLACK);
  M5.Lcd.setTextColor(AMBER_COLOR);
  M5.Lcd.setTextSize(1);
  
  if (btnA) M5.Lcd.drawString(String("[A: ") + btnA + "]", 5, M5.Lcd.height() - 15, 2);
  if (btnB) M5.Lcd.drawString(String("[B: ") + btnB + "]", 110, M5.Lcd.height() - 15, 2);
  if (btnC) M5.Lcd.drawString(String("[C: ") + btnC + "]", 215, M5.Lcd.height() - 15, 2);
}

// 時刻文字列の取得
String getTimeString(time_t t) {
  struct tm *timeinfo = localtime(&t);
  char buffer[9];
  strftime(buffer, sizeof(buffer), "%H:%M", timeinfo);
  return String(buffer);
}

// 日付文字列の取得
String getDateString(time_t t) {
  struct tm *timeinfo = localtime(&t);
  char buffer[32];
  const char* weekdays[] = {"日", "月", "火", "水", "木", "金", "土"};
  sprintf(buffer, "%04d/%02d/%02d (%s)", 
          timeinfo->tm_year + 1900,
          timeinfo->tm_mon + 1,
          timeinfo->tm_mday,
          weekdays[timeinfo->tm_wday]);
  return String(buffer);
}

// 残り時間文字列の取得
String getRemainTimeString(time_t now, time_t target) {
  time_t diff = target - now;
  if (diff < 0) return "-00:00:00";
  
  int hours = diff / 3600;
  int minutes = (diff % 3600) / 60;
  int seconds = diff % 60;
  
  char buffer[32];
  sprintf(buffer, "-%02d:%02d:%02d", hours, minutes, seconds);
  return String(buffer);
}

void drawProgressBar(int x, int y, int width, int height, float progress) {
  M5.Lcd.drawRect(x, y, width, height, AMBER_COLOR);
  int progressWidth = (width - 2) * progress;
  M5.Lcd.fillRect(x + 1, y + 1, width - 2, height - 2, DARK_GREY);
  M5.Lcd.fillRect(x + 1, y + 1, progressWidth, height - 2, AMBER_COLOR);
}

void drawInvertedText(int x, int y, const char* text, int font) {
  M5.Lcd.setTextFont(font);
  M5.Lcd.setTextDatum(MC_DATUM); // 中央揃え
  M5.Lcd.setTextColor(TFT_BLACK, AMBER_COLOR); // 反転色
  M5.Lcd.drawString(text, x, y);
  M5.Lcd.setTextColor(AMBER_COLOR, TFT_BLACK); // デフォルト色に戻す
}

// drawMainDisplay, drawNTPSync, drawInputMode, drawScheduleSelect, drawAlarmActive, drawSettingsMenu
// これらの関数はmain.cppから呼ばれるため、最低限のダミー実装を用意してリンクエラーを防ぐ。
// 必要に応じて本実装を追加してください。

// メイン表示画面の詳細描画
// - 残り時間（-HH:MM:SS）
// - 進捗バー
// - NEXT時刻
// - 鳴動時刻リスト
// 必要なグローバル: alarmTimes, getNextAlarmTime(), settings, etc.

#include "alarm.h" // getNextAlarmTime() を利用するため
extern std::vector<time_t> alarmTimes;
extern Settings settings;

void drawMainDisplay() {
  M5.Lcd.fillScreen(TFT_BLACK);
  drawStatusBar("MAIN");

  time_t now = time(NULL);
  time_t nextAlarm = getNextAlarmTime();

  // 日付・時刻表示
  String dateStr = getDateString(now);
  String timeStr = getTimeString(now);
  M5.Lcd.setTextFont(FONT_AUXILIARY);
  M5.Lcd.setTextColor(AMBER_COLOR, TFT_BLACK);
  M5.Lcd.drawString(dateStr, 10, 30);
  M5.Lcd.setTextFont(FONT_IMPORTANT);
  M5.Lcd.drawString(timeStr, 10, 55);

  // 残り時間表示（中央大きく）
  String remainStr = "--:--:--";
  if (nextAlarm > 0) remainStr = getRemainTimeString(now, nextAlarm);
  M5.Lcd.setTextFont(FONT_MAIN);
  M5.Lcd.setTextColor(AMBER_COLOR, TFT_BLACK);
  M5.Lcd.drawString(remainStr, 160, 100);

  // 進捗バー
  float progress = 0.0f;
  static time_t lastClear = 0; // 直前の解除時刻（本来はALARM解除時に更新）
  if (nextAlarm > 0 && lastClear > 0 && nextAlarm > lastClear) {
    float total = float(nextAlarm - lastClear);
    float done = float(now - lastClear);
    progress = (done >= 0 && total > 0) ? (done / total) : 0.0f;
    if (progress > 1.0f) progress = 1.0f;
  }
  drawProgressBar(40, 170, 240, 12, progress);

  // NEXT時刻
  if (nextAlarm > 0) {
    String nextStr = "NEXT: " + getTimeString(nextAlarm);
    M5.Lcd.setTextFont(FONT_IMPORTANT);
    M5.Lcd.setTextColor(AMBER_COLOR, TFT_BLACK);
    M5.Lcd.drawString(nextStr, 160, 190);
  }

  // 鳴動時刻リスト（最大5件、下部に表示）
  M5.Lcd.setTextFont(FONT_AUXILIARY);
  M5.Lcd.setTextColor(AMBER_COLOR, TFT_BLACK);
  int y = 220;
  int count = 0;
  for (time_t t : alarmTimes) {
    if (count >= 5) break;
    if (t <= now) continue;
    String tstr = getTimeString(t);
    M5.Lcd.drawString(tstr, 60 + count * 50, y);
    count++;
  }

  // ボタンヒント
  drawButtonHints("ABS", "REL+", "SEL");
}
void drawNTPSync() {}
// 入力モード画面の詳細描画
// - 数字入力UI（カーソル移動、ネガポジ反転、未入力は_表示）
// - 入力値のバリデーション、予測時刻表示
// - ボタンヒント
#include "input.h"
extern InputState inputState;

void drawInputMode() {
  M5.Lcd.fillScreen(TFT_BLACK);
  drawStatusBar("INPUT");


  // 入力中の数字列を作成（例: "1_ : _5"）
  char buf[8] = {'_', '_', ' ', ':', ' ', '_', '_', '\0'};
  int h = inputState.hours;
  int m = inputState.minutes;
  // 各桁を分解
  buf[0] = (h >= 10) ? ('0' + h / 10) : '_';
  buf[1] = (h >= 0) ? ('0' + h % 10) : '_';
  buf[5] = (m >= 10) ? ('0' + m / 10) : '_';
  buf[6] = (m >= 0) ? ('0' + m % 10) : '_';

  // カーソル位置の桁を反転表示
  int x = 100, y = 80;
  for (int i = 0; i < 7; ++i) {
    if (i == 2 || i == 4) continue; // スペースとコロン
    int font = FONT_MAIN;
    bool isCursor = false;
    if ((inputState.currentDigit == 0 && i == 0) ||
        (inputState.currentDigit == 1 && i == 1) ||
        (inputState.currentDigit == 2 && i == 5) ||
        (inputState.currentDigit == 3 && i == 6)) {
      isCursor = true;
    }
    if (isCursor) {
      drawInvertedText(x + i * 32, y, String(buf[i]).c_str(), font);
    } else {
      M5.Lcd.setTextFont(font);
      M5.Lcd.setTextColor(AMBER_COLOR, TFT_BLACK);
      M5.Lcd.drawString(String(buf[i]), x + i * 32, y);
    }
  }

  // 予測時刻表示（現在時刻+入力値）
  time_t now = time(NULL);
  struct tm tminfo;
  localtime_r(&now, &tminfo);
  tminfo.tm_hour = inputState.hours;
  tminfo.tm_min = inputState.minutes;
  tminfo.tm_sec = 0;
  time_t predicted = mktime(&tminfo);
  String predStr = "-> " + getDateString(predicted) + " " + getTimeString(predicted);
  M5.Lcd.setTextFont(FONT_AUXILIARY);
  M5.Lcd.setTextColor(AMBER_COLOR, TFT_BLACK);
  M5.Lcd.drawString(predStr, 60, 180);

  // ボタンヒント
  drawButtonHints("+1/5", "NEXT", "OK/CANCEL");
}
void drawScheduleSelect(int selectedIndex) {
  M5.Lcd.fillScreen(TFT_BLACK);
  drawStatusBar("SCHEDULE");

  time_t now = time(NULL);
  int listSize = alarmTimes.size() + 1; // +1 for SETTINGS
  int y0 = 50;
  int lineHeight = 32;

  for (int i = 0; i < listSize; ++i) {
    int y = y0 + i * lineHeight;
    String label;
    if (i < alarmTimes.size()) {
      // アラーム時刻
      label = getTimeString(alarmTimes[i]);
    } else {
      // SETTINGS項目
      label = "[SETTINGS]";
    }
    if (i == selectedIndex) {
      drawInvertedText(160, y, label.c_str(), FONT_IMPORTANT);
    } else {
      M5.Lcd.setTextFont(FONT_IMPORTANT);
      M5.Lcd.setTextColor(AMBER_COLOR, TFT_BLACK);
      M5.Lcd.drawString(label, 160, y);
    }
  }

  // ボタンヒント
  drawButtonHints("DEL", "UP/DOWN", "SELECT");
}
void drawSettingsMenu() {}

// drawAlarmActive も必要なら追加してください。
