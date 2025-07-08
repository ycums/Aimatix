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

void drawMainDisplay() {}
void drawNTPSync() {}
void drawInputMode() {}
void drawScheduleSelect() {}
void drawSettingsMenu() {}

// drawAlarmActive も必要なら追加してください。
