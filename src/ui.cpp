#include "ui.h"
#include <M5Stack.h>
#include <time.h>

// Global variables used in UI functions (declare as extern in main.cpp or pass as parameters)
// extern Settings settings; // Assuming settings are needed for drawing
// extern std::vector<time_t> alarmTimes; // Assuming alarmTimes are needed for drawing
// extern Mode currentMode; // Assuming currentMode is needed for drawing

// Define the sprite object
TFT_eSprite sprite = TFT_eSprite(&M5.Lcd); // Create sprite object associated with M5.Lcd

// Initialize UI and display settings
void initUI() {
  // スプライトの初期化
  sprite.createSprite(320, 240);  // M5Stackの画面サイズ
  sprite.fillSprite(TFT_BLACK);   // 背景を黒で初期化
}

// ステータスバーの描画
void drawStatusBar(const char* mode) {
  // スプライトに描画
  sprite.fillRect(0, 0, 320, 20, TFT_BLACK);
  sprite.setTextSize(1);
  
  // モード名を表示
  sprite.setTextColor(AMBER_COLOR);
  sprite.drawString(mode, 5, 5, 2);
  
  // バッテリー情報を表示
  int batteryLevel = M5.Power.getBatteryLevel();
  String batteryString = String(batteryLevel) + "%";
  if (M5.Power.isCharging()) {
    batteryString = "⚡" + batteryString;
  }
  
  uint16_t color = (batteryLevel <= 20) ? FLASH_ORANGE : AMBER_COLOR;
  sprite.setTextColor(color);
  sprite.drawString(batteryString.c_str(), 280, 5, 2);
}

// ボタンヒントの描画
void drawButtonHints(const char* btnA, const char* btnB, const char* btnC) {
  sprite.fillRect(0, M5.Lcd.height() - 20, 320, 20, TFT_BLACK);
  sprite.setTextColor(AMBER_COLOR);
  sprite.setTextSize(1);
  
  if (btnA) sprite.drawString(String("[A: ") + btnA + "]", 5, M5.Lcd.height() - 15, 2);
  if (btnB) sprite.drawString(String("[B: ") + btnB + "]", 110, M5.Lcd.height() - 15, 2);
  if (btnC) sprite.drawString(String("[C: ") + btnC + "]", 215, M5.Lcd.height() - 15, 2);
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
  sprite.drawRect(x, y, width, height, AMBER_COLOR);
  int progressWidth = (width - 2) * progress;
  sprite.fillRect(x + 1, y + 1, width - 2, height - 2, DARK_GREY);
  sprite.fillRect(x + 1, y + 1, progressWidth, height - 2, AMBER_COLOR);
}

void drawInvertedText(int x, int y, const char* text, int font) {
  sprite.setTextFont(font);
  sprite.setTextDatum(MC_DATUM); // 中央揃え
  sprite.setTextColor(TFT_BLACK, AMBER_COLOR); // 反転色
  sprite.drawString(text, x, y);
  sprite.setTextColor(AMBER_COLOR, TFT_BLACK); // デフォルト色に戻す
}

// drawMainDisplay, drawNTPSync, drawInputMode, drawScheduleSelect, drawAlarmActive, drawSettingsMenu
// これらの関数はmain.cppから呼ばれるため、最低限のダミー実装を用意してリンクエラーを防ぐ。
// 必要に応じて本実装を追加してください。

// メイン表示画面の詳細描画
// - 残り時間（-HH:MM:SS）
// - 進捗バー
// - NEXT時刻
// - 鳴動時刻リスト
void drawMainDisplay() {
  // ここにメイン画面の描画コードをspriteに対して記述
  sprite.fillSprite(TFT_BLACK); // 画面クリア
  drawStatusBar("MAIN");
  drawButtonHints("ABS", "REL+", "SCHED");
  
  // 例: 現在時刻を表示
  time_t now = time(NULL);
  sprite.setTextSize(2);
  sprite.setTextColor(AMBER_COLOR);
  sprite.setTextDatum(TC_DATUM); // 中央上
  sprite.drawString(getDateString(now), 160, 30);
  sprite.setTextDatum(MC_DATUM); // 中央
  sprite.drawString(getTimeString(now), 160, 120, 7); // 大きいフォント
}

void drawNTPSync() {
  sprite.fillSprite(TFT_BLACK);
  drawStatusBar("NTP SYNC");
  drawButtonHints(NULL, NULL, "SKIP");
  sprite.setTextDatum(MC_DATUM);
  sprite.setTextColor(AMBER_COLOR);
  sprite.drawString("Syncing Time...", 160, 120, 4);
}

void drawInputMode() {
  sprite.fillSprite(TFT_BLACK);
  drawStatusBar("INPUT");
  drawButtonHints("INC", "NEXT", "SET/CANCEL");
  sprite.setTextDatum(MC_DATUM);
  sprite.setTextColor(AMBER_COLOR);
  sprite.drawString("Enter Time:", 160, 80, 4);
  
  // 入力中の時刻表示（inputStateを使用）
  char inputBuffer[6];
  sprintf(inputBuffer, "%02d:%02d", inputState.hours, inputState.minutes);
  sprite.drawString(inputBuffer, 160, 120, 7);
  
  // カーソル表示（点滅させる場合は別途ロジックが必要）
  int cursor_x = 160 - sprite.textWidth(inputBuffer, 7) / 2;
  if (inputState.currentDigit < 2) cursor_x += inputState.currentDigit * sprite.textWidth("0", 7);
  else cursor_x += (inputState.currentDigit - 2) * sprite.textWidth("0", 7) + sprite.textWidth(":", 7);
  
  sprite.fillRect(cursor_x, 120 + sprite.fontHeight(7) / 2, sprite.textWidth("0", 7), 2, AMBER_COLOR);
}

void drawScheduleSelect(int selectedIndex) {
  sprite.fillSprite(TFT_BLACK);
  drawStatusBar("SCHEDULE");
  drawButtonHints("DEL", "NEXT", "PREV/SET");
  
  sprite.setTextDatum(TL_DATUM); // 左上
  sprite.setTextColor(AMBER_COLOR);
  sprite.drawString("Alarms:", 10, 30, 2);
  
  // アラームリスト表示
  for (size_t i = 0; i < alarmTimes.size(); ++i) {
    String alarmStr = getTimeString(alarmTimes[i]);
    if (i == selectedIndex) {
      drawInvertedText(160, 60 + i * 20, alarmStr.c_str(), 2); // 選択項目を反転
    } else {
      sprite.drawString(alarmStr, 10, 60 + i * 20, 2);
    }
  }
  
  // SETTINGS項目
  if (selectedIndex == alarmTimes.size()) {
     drawInvertedText(160, 60 + alarmTimes.size() * 20, "SETTINGS", 2);
  } else {
     sprite.drawString("SETTINGS", 10, 60 + alarmTimes.size() * 20, 2);
  }
}



void drawSettingsMenu() {
  sprite.fillSprite(TFT_BLACK);
  drawStatusBar("SETTINGS");
  drawButtonHints("PREV", "NEXT", "SELECT/BACK");
  
  sprite.setTextDatum(TL_DATUM);
  sprite.setTextColor(AMBER_COLOR);
  sprite.drawString("Settings:", 10, 30, 2);
  
  const char* items[] = {"Sound", "Vibration", "LCD Brightness", "All Clear", "Info"};
  settingsMenu.itemCount = sizeof(items) / sizeof(items[0]);
  
  for (int i = 0; i < settingsMenu.itemCount; ++i) {
    String itemStr = items[i];
    if (i == settingsMenu.selectedItem) {
      drawInvertedText(160, 60 + i * 20, itemStr.c_str(), 2);
    } else {
      sprite.drawString(itemStr, 10, 60 + i * 20, 2);
    }
  }
}
