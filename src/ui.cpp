#include "ui.h"
#include <M5Stack.h>
#include <time.h>
#include <WiFi.h>

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

void drawInvertedText(const char* text, int x, int y, int font) {
  sprite.setTextFont(font);
  sprite.setTextDatum(TL_DATUM);
  int lineHeight = sprite.fontHeight(font);
  // デバッグ用: 塗りつぶし範囲を明示
  sprite.fillRect(0, y, 320, lineHeight, AMBER_COLOR);
  sprite.setTextColor(TFT_BLACK); // 黒文字
  sprite.drawString(text, x, y);
  sprite.setTextColor(AMBER_COLOR); // デフォルト色に戻す
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
  sprite.fillSprite(TFT_BLACK);
  drawStatusBar("MAIN");
  drawButtonHints("ABS", "REL+", "SCHED");

  time_t now = time(NULL);
  // --- 上部中央: 日付（英語3文字曜日, ピリオド付き） ---
  sprite.setTextDatum(TC_DATUM);
  sprite.setTextColor(AMBER_COLOR, TFT_BLACK);
  sprite.setTextFont(3); // Font3
  struct tm* tminfo = localtime(&now);
  const char* wdays[] = {"Sun.", "Mon.", "Tue.", "Wed.", "Thu.", "Fri.", "Sat."};
  char dateStr[32];
  sprintf(dateStr, "%04d/%02d/%02d %s", tminfo->tm_year+1900, tminfo->tm_mon+1, tminfo->tm_mday, wdays[tminfo->tm_wday]);
  sprite.drawString(dateStr, 160, 10);

  // --- 現在時刻（中央やや上、HH:MMのみ） ---
  sprite.setTextDatum(MC_DATUM);
  sprite.setTextFont(4);
  char hmStr[6];
  sprintf(hmStr, "%02d:%02d", tminfo->tm_hour, tminfo->tm_min);
  sprite.drawString(hmStr, 160, 45);

  // --- 中央: 残り時間 ---
  time_t nextAlarm = 0;
  std::vector<time_t> futureAlarms;
  if (!alarmTimes.empty()) {
    std::sort(alarmTimes.begin(), alarmTimes.end());
    for (time_t t : alarmTimes) {
      if (t > now) {
        if (!nextAlarm) nextAlarm = t;
        futureAlarms.push_back(t);
      }
    }
  }
  sprite.setTextDatum(MC_DATUM);
  sprite.setTextFont(7);
  sprite.setTextColor(AMBER_COLOR, TFT_BLACK);
  if (nextAlarm) {
    sprite.drawString(getRemainTimeString(now, nextAlarm), 160, 95);
  } else {
    sprite.drawString("-00:00:00", 160, 95);
  }

  // --- プログレスバー ---
  extern time_t lastReleaseTime;
  float progress = 0.0f;
  if (nextAlarm && lastReleaseTime && nextAlarm > lastReleaseTime) {
    float total = nextAlarm - lastReleaseTime;
    float done = now - lastReleaseTime;
    progress = (done >= 0 && total > 0) ? (done / total) : 0.0f;
    if (progress < 0) progress = 0;
    if (progress > 1) progress = 1;
  }
  drawProgressBar(40, 145, 240, 10, progress);

  // --- アラームリスト（未来のみ最大5件） ---
  sprite.setTextDatum(ML_DATUM);
  sprite.setTextFont(2);
  sprite.setTextColor(AMBER_COLOR, TFT_BLACK);
  int y = 210;
  int count = 0;
  for (time_t t : futureAlarms) {
    if (count >= 5) break;
    sprite.drawString(getTimeString(t), 20 + count * 60, y);
    count++;
  }

  sprite.pushSprite(0, 0);
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
  // モード名を明示
  extern Mode currentMode;
  const char* modeTitle = "SET TIME";
  if (currentMode == ABS_TIME_INPUT) modeTitle = "SET ABS TIME";
  else if (currentMode == REL_PLUS_TIME_INPUT) modeTitle = "ADD REL TIME";
  else if (currentMode == REL_MINUS_TIME_INPUT) modeTitle = "SUB REL TIME";
  drawStatusBar(modeTitle);
  drawButtonHints("INC", "NEXT", "SET/CANCEL");
  sprite.setTextDatum(TC_DATUM);
  sprite.setTextFont(4);
  sprite.setTextColor(AMBER_COLOR, TFT_BLACK);
  sprite.drawString(modeTitle, 160, 30);

  // 入力中の数字（__:__ 形式、カーソル桁はネガポジ反転）
  char buf[8] = "__:__";
  int h1 = inputState.hours / 10, h2 = inputState.hours % 10;
  int m1 = inputState.minutes / 10, m2 = inputState.minutes % 10;
  buf[0] = (inputState.currentDigit > 0 || h1 > 0) ? '0' + h1 : '_';
  buf[1] = (inputState.currentDigit > 1 || h2 > 0) ? '0' + h2 : '_';
  buf[3] = (inputState.currentDigit > 2 || m1 > 0) ? '0' + m1 : '_';
  buf[4] = (inputState.currentDigit > 3 || m2 > 0) ? '0' + m2 : '_';
  sprite.setTextDatum(MC_DATUM);
  sprite.setTextFont(7);
  for (int i = 0, x = 160 - 60; i < 5; ++i, x += 30) {
    if (i == 2) {
      sprite.setTextColor(AMBER_COLOR, TFT_BLACK);
      sprite.drawString(":", x, 120);
      continue;
    }
    int digitIdx = (i < 2) ? i : i - 1;
    if (inputState.currentDigit == digitIdx) {
      sprite.setTextColor(TFT_BLACK, AMBER_COLOR); // ネガポジ反転
      sprite.drawString(String(buf[i]), x, 120);
      sprite.setTextColor(AMBER_COLOR, TFT_BLACK);
    } else {
      sprite.drawString(String(buf[i]), x, 120);
    }
  }

  // 入力値・モードに応じたアラーム時刻プレビュー
  sprite.setTextDatum(MC_DATUM);
  sprite.setTextFont(2);
  sprite.setTextColor(AMBER_COLOR, TFT_BLACK);
  time_t now = time(NULL);
  struct tm tminfo;
  char preview[32] = "";
  if (currentMode == ABS_TIME_INPUT) {
    localtime_r(&now, &tminfo);
    tminfo.tm_hour = inputState.hours;
    tminfo.tm_min = inputState.minutes;
    tminfo.tm_sec = 0;
    time_t alarmT = mktime(&tminfo);
    strftime(preview, sizeof(preview), "-> %Y/%m/%d %H:%M", localtime(&alarmT));
  } else if (currentMode == REL_PLUS_TIME_INPUT || currentMode == REL_MINUS_TIME_INPUT) {
    int total = inputState.hours * 60 + inputState.minutes;
    if (currentMode == REL_MINUS_TIME_INPUT) total = -total;
    time_t base = now + total * 60;
    struct tm t;
    localtime_r(&base, &t);
    t.tm_sec = 0;
    time_t alarmT = mktime(&t);
    strftime(preview, sizeof(preview), "-> %Y/%m/%d %H:%M", localtime(&alarmT));
  }
  sprite.drawString(preview, 160, 170);

  sprite.pushSprite(0, 0);
}

void drawScheduleSelect(int selectedIndex) {
  sprite.fillSprite(TFT_BLACK);
  drawStatusBar("SCHEDULE");
  drawButtonHints("DEL", "NEXT", "PREV/SET");
  
  sprite.setTextDatum(TL_DATUM); // 左上
  sprite.setTextColor(AMBER_COLOR);
  sprite.drawString("Alarms:", 10, 30, 2);
  
  // アラームリスト表示（Font2で統一）
  for (size_t i = 0; i < alarmTimes.size(); ++i) {
    String alarmStr = getTimeString(alarmTimes[i]);
    int y = 60 + i * 20; // Font2の行間
    if (i == selectedIndex) {
      drawInvertedText(alarmStr.c_str(), 10, y, 2); // 選択項目を反転
    } else {
      sprite.setTextFont(2);
      sprite.drawString(alarmStr, 10, y, 2);
    }
  }
  
  // SETTINGS項目（Font2で統一）
  int settingsY = 60 + alarmTimes.size() * 20;
  if (selectedIndex == alarmTimes.size()) {
     drawInvertedText("SETTINGS", 10, settingsY, 2); // 左寄せで統一
  } else {
     sprite.setTextFont(2);
     sprite.drawString("SETTINGS", 10, settingsY, 2);
  }
  
  sprite.pushSprite(0, 0);
}



void drawSettingsMenu() {
  sprite.fillSprite(TFT_BLACK);
  drawStatusBar("SETTINGS");
  drawButtonHints("PREV", "NEXT", "SELECT/BACK");
  
  sprite.setTextDatum(TL_DATUM);
  sprite.setTextColor(AMBER_COLOR);
  
  const char* items[] = {"Sound", "Vibration", "LCD Brightness", "All Clear", "Info"};
  settingsMenu.itemCount = sizeof(items) / sizeof(items[0]);
  
  for (int i = 0; i < settingsMenu.itemCount; ++i) {
    String itemStr = items[i];
    int y = 40 + i * 20; // Font2の行間
    if (i == settingsMenu.selectedItem) {
      drawInvertedText(itemStr.c_str(), 10, y, 2); // Font2で強調＋全幅反転
    } else {
      sprite.setTextFont(2);
      sprite.drawString(itemStr, 10, y, 2); // Font2で統一
    }
  }
  sprite.pushSprite(0, 0);
}

void drawInfoDisplay() {
  sprite.fillSprite(TFT_BLACK);
  drawStatusBar("INFO");
  drawButtonHints(NULL, NULL, "BACK");
  sprite.setTextDatum(TL_DATUM);
  sprite.setTextColor(AMBER_COLOR);
  sprite.drawString("M5Stack Timer", 40, 60, 4);
  sprite.drawString("Version 1.0.0", 40, 100, 2);
  sprite.drawString("(C) 2025 Your Name", 40, 120, 2);
  sprite.drawString((String)"MAC: " + WiFi.macAddress(), 40, 140, 2);
}
