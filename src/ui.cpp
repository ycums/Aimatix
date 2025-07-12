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

// グリッドシステム関連の関数実装

void drawTitleBar(const char* modeName) {
  // タイトルバー背景を黒で塗りつぶし
  sprite.fillRect(0, 0, SCREEN_WIDTH, TITLE_HEIGHT, TFT_BLACK);
  
  // タイトルバー下部に罫線を描画
  sprite.drawLine(0, TITLE_HEIGHT - 1, SCREEN_WIDTH - 1, TITLE_HEIGHT - 1, AMBER_COLOR);
  
  // モード名を左側に表示
  sprite.setTextFont(2);
  sprite.setTextColor(AMBER_COLOR, TFT_BLACK);
  sprite.setTextDatum(TL_DATUM);
  sprite.drawString(modeName, 5, 2);
  
  // バッテリー情報を右側に表示
  int batteryLevel = M5.Power.getBatteryLevel();
  bool isCharging = M5.Power.isCharging();
  
  char batteryStr[10];
  sprintf(batteryStr, "%s %d%%", isCharging ? "⚡" : "🔋", batteryLevel);
  
  // 残量に応じて色を変更
  int batteryColor = AMBER_COLOR;
  if (batteryLevel <= 20 && !isCharging) {
    batteryColor = FLASH_ORANGE;
  }
  
  sprite.setTextColor(batteryColor, TFT_BLACK);
  sprite.setTextDatum(TR_DATUM);
  sprite.drawString(batteryStr, SCREEN_WIDTH - 5, 2);
}

void drawButtonHintsGrid(const char* btnA, const char* btnB, const char* btnC) {
  // ボタンヒント領域背景を黒で塗りつぶし
  sprite.fillRect(0, SCREEN_HEIGHT - HINT_HEIGHT, SCREEN_WIDTH, HINT_HEIGHT, TFT_BLACK);
  
  // ボタンヒント上部に罫線を描画
  sprite.drawLine(0, SCREEN_HEIGHT - HINT_HEIGHT, SCREEN_WIDTH - 1, SCREEN_HEIGHT - HINT_HEIGHT, AMBER_COLOR);
  
  sprite.setTextFont(2);
  sprite.setTextColor(AMBER_COLOR, TFT_BLACK);
  sprite.setTextDatum(MC_DATUM);
  
  // 物理ボタンの位置に合わせて配置（A:左、B:中央、C:右）
  if (btnA) sprite.drawString(btnA, 40, SCREEN_HEIGHT - HINT_HEIGHT/2);
  if (btnB) sprite.drawString(btnB, SCREEN_WIDTH/2, SCREEN_HEIGHT - HINT_HEIGHT/2);
  if (btnC) sprite.drawString(btnC, SCREEN_WIDTH - 40, SCREEN_HEIGHT - HINT_HEIGHT/2);
}

void drawCommonUI(const char* modeName) {
  drawTitleBar(modeName);
  // ボタンヒントは各画面で個別に呼び出す（内容が異なるため）
}

void drawGridLines() {
  // グリッド線を描画（デバッグ用）
  sprite.setTextColor(DARK_GREY, TFT_BLACK);
  
  // 縦線（8本）
  for (int i = 0; i <= 8; i++) {
    int x = i * GRID_WIDTH;
    sprite.drawLine(x, TITLE_HEIGHT, x, SCREEN_HEIGHT - HINT_HEIGHT, DARK_GREY);
  }
  
  // 横線（5本）
  for (int i = 0; i <= 5; i++) {
    int y = TITLE_HEIGHT + i * GRID_HEIGHT;
    sprite.drawLine(0, y, SCREEN_WIDTH, y, DARK_GREY);
  }
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
  drawTitleBar("MAIN");
  drawButtonHintsGrid("ABS", "REL+", "SCHED");
  
  time_t now = time(NULL);
  
  // --- 上部中央: 日付（グリッドセル(0,0)-(7,0)） ---
  sprite.setTextDatum(TC_DATUM);
  sprite.setTextColor(AMBER_COLOR, TFT_BLACK);
  sprite.setTextFont(2);
  struct tm* tminfo = localtime(&now);
  const char* wdays[] = {"Sun.", "Mon.", "Tue.", "Wed.", "Thu.", "Fri.", "Sat."};
  char dateStr[32];
  sprintf(dateStr, "%04d/%02d/%02d %s", tminfo->tm_year+1900, tminfo->tm_mon+1, tminfo->tm_mday, wdays[tminfo->tm_wday]);
  sprite.drawString(dateStr, GRID_X(4), GRID_Y(0) + 10);

  // --- 現在時刻（グリッドセル(3,1)-(4,1)） ---
  sprite.setTextDatum(MC_DATUM);
  sprite.setTextFont(4);
  char hmStr[6];
  sprintf(hmStr, "%02d:%02d", tminfo->tm_hour, tminfo->tm_min);
  sprite.drawString(hmStr, GRID_X(4), GRID_Y(1) + 20);

  // --- 中央: 残り時間（グリッドセル(2,2)-(5,2)） ---
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
    sprite.drawString(getRemainTimeString(now, nextAlarm), GRID_X(4), GRID_Y(2) + 24);
  } else {
    sprite.drawString("-00:00:00", GRID_X(4), GRID_Y(2) + 24);
  }

  // --- プログレスバー（グリッドセル(0,3)-(7,3)） ---
  extern time_t lastReleaseTime;
  float progress = 0.0f;
  if (nextAlarm && lastReleaseTime && nextAlarm > lastReleaseTime) {
    float total = nextAlarm - lastReleaseTime;
    float done = now - lastReleaseTime;
    progress = (done >= 0 && total > 0) ? (done / total) : 0.0f;
    if (progress < 0) progress = 0;
    if (progress > 1) progress = 1;
  }
  drawProgressBar(GRID_X(0), GRID_Y(3) + 15, GRID_WIDTH * 8, 10, progress);

  // --- アラームリスト（グリッドセル(0,4)-(7,4)） ---
  sprite.setTextDatum(MC_DATUM);
  sprite.setTextFont(2);
  sprite.setTextColor(AMBER_COLOR, TFT_BLACK);
  int count = 0;
  for (time_t t : futureAlarms) {
    if (count >= 5) break;
    int x = GRID_X(1) + count * (GRID_WIDTH * 1.5);
    sprite.drawString(getTimeString(t), x, GRID_Y(4) + 10);
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
  drawTitleBar(modeTitle);
  drawButtonHintsGrid("+1/+5", "NEXT/RESET", "SET");

  // 桁ごと編集方式の値を表示（水平方向のみ中央寄せ）
  char buf[6];
  snprintf(buf, sizeof(buf), "%d%d:%d%d",
    digitEditInput.hourTens,
    digitEditInput.hourOnes,
    digitEditInput.minTens,
    digitEditInput.minOnes);
  sprite.setTextDatum(MC_DATUM);
  sprite.setTextFont(7);
  
  // 水平方向の中央位置を計算
  int centerX = GRID_X(4); // 画面中央
  int y = GRID_Y(1) + 24;  // 以前の垂直位置
  
  // 時刻表示の開始位置を計算（5文字分の幅を考慮）
  int totalWidth = 5 * 40; // 5文字 × 40px
  int startX = centerX - totalWidth / 2;
  
  int x = startX;
  for (int i = 0; i < 5; ++i) {
    if (i == 2) {
      sprite.setTextColor(AMBER_COLOR, TFT_BLACK);
      sprite.drawString(":", x, y);
      x += 40;
      continue;
    }
    int digitIdx = (i < 2) ? i : i - 1;
    if (digitEditInput.cursor == digitIdx) {
      sprite.setTextColor(TFT_BLACK, AMBER_COLOR); // ネガポジ反転
      sprite.drawString(String(buf[i]), x, y);
      sprite.setTextColor(AMBER_COLOR, TFT_BLACK);
    } else {
      sprite.drawString(String(buf[i]), x, y);
    }
    x += 40;
  }

  // 入力値のプレビュー（グリッドセル(1,3)-(6,3)）
  sprite.setTextDatum(MC_DATUM);
  sprite.setTextFont(2);
  sprite.setTextColor(AMBER_COLOR, TFT_BLACK);
  char preview[16];
  snprintf(preview, sizeof(preview), "%02d:%02d",
    digitEditInput.hourTens * 10 + digitEditInput.hourOnes,
    digitEditInput.minTens * 10 + digitEditInput.minOnes);
  sprite.drawString(preview, GRID_X(4), GRID_Y(3) + 10);

  sprite.pushSprite(0, 0);
}



void drawScheduleSelection() {
  sprite.fillSprite(TFT_BLACK);
  drawTitleBar("SCHEDULE");
  drawButtonHintsGrid("PREV", "NEXT", "SELECT/BACK");
  
  // アラームリストを文字列配列に変換
  String alarmStrings[alarmTimes.size()];
  for (int i = 0; i < alarmTimes.size(); ++i) {
    alarmStrings[i] = getTimeString(alarmTimes[i]);
  }
  
  // 文字列ポインタの配列を作成
  const char* items[alarmTimes.size()];
  for (int i = 0; i < alarmTimes.size(); ++i) {
    items[i] = alarmStrings[i].c_str();
  }
  
  // 共通メニュー描画関数を使用（グリッドセル(1,0)-(6,2)、1pxマージン）
  extern int scheduleSelectedIndex;
  drawMenuItems(items, alarmTimes.size(), scheduleSelectedIndex, 0, 2, 1);
  
  sprite.pushSprite(0, 0);
}



void drawSettingsMenu() {
  sprite.fillSprite(TFT_BLACK);
  drawTitleBar("SETTINGS");
  drawButtonHintsGrid("PREV", "NEXT", "SELECT/BACK");
  
  // types.hで定義された項目を使用
  const char* items[] = {
    "SOUND: ",
    "LCD BRIGHTNESS",
    "WARNING COLOR TEST",
    "ALL CLEAR",
    "INFO"
  };
  settingsMenu.itemCount = sizeof(items) / sizeof(items[0]);
  
  // 共通メニュー描画関数を使用（グリッドセル(1,0)-(6,2)、1pxマージン）
  drawMenuItems(items, settingsMenu.itemCount, settingsMenu.selectedItem, 0, 2, 1);
  
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

// 後方互換性のための関数（既存コードとの互換性を保つ）
void drawStatusBar(const char* mode) {
  drawTitleBar(mode);
}

void drawButtonHints(const char* btnA, const char* btnB, const char* btnC) {
  drawButtonHintsGrid(btnA, btnB, btnC);
}

void drawMenuItems(const char** items, int itemCount, int selectedItem, int startGridRow, int endGridRow, int marginTop) {
  sprite.setTextDatum(TL_DATUM);
  sprite.setTextColor(AMBER_COLOR);
  
  int startY = GRID_Y(startGridRow) + marginTop;
  int itemHeight = 25; // 行間
  int totalHeight = itemCount * itemHeight;
  int availableHeight = (endGridRow - startGridRow + 1) * GRID_HEIGHT;
  int centerY = startY + (availableHeight - totalHeight) / 2; // 中央揃え
  
  for (int i = 0; i < itemCount; ++i) {
    String itemStr = items[i];
    int y = centerY + i * itemHeight;
    if (i == selectedItem) {
      drawInvertedText(itemStr.c_str(), GRID_X(1), y, 2); // 選択項目を反転
    } else {
      sprite.setTextFont(2);
      sprite.drawString(itemStr, GRID_X(1), y, 2);
    }
  }
}

void drawWarningColorTest() {
  static bool flash = false;
  static unsigned long lastFlash = 0;
  
  // 0.5秒ごとに点滅
  if (millis() - lastFlash >= 500) {
    flash = !flash;
    lastFlash = millis();
  }
  
  sprite.fillSprite(flash ? FLASH_ORANGE : TFT_BLACK); // 確定した警告色
  
  sprite.setTextDatum(MC_DATUM);
  sprite.setTextFont(4);
  
  if (flash) {
    sprite.setTextColor(TFT_BLACK, FLASH_ORANGE);
    sprite.drawString("WARNING COLOR TEST", GRID_X(4), GRID_Y(1) + 20);
    sprite.drawString("0xF000", GRID_X(4), GRID_Y(2) + 20);
    sprite.drawString("R=31, G=0, B=0", GRID_X(4), GRID_Y(3) + 20);
  } else {
    sprite.setTextColor(FLASH_ORANGE, TFT_BLACK);
    sprite.drawString("WARNING COLOR TEST", GRID_X(4), GRID_Y(1) + 20);
    sprite.drawString("0xF000", GRID_X(4), GRID_Y(2) + 20);
    sprite.drawString("R=31, G=0, B=0", GRID_X(4), GRID_Y(3) + 20);
  }
  
  drawTitleBar("WARNING TEST");
  drawButtonHintsGrid(NULL, NULL, "BACK");
  
  sprite.pushSprite(0, 0);
}
