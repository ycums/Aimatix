#include "ui.h"
#include <M5Stack.h>
#include <time.h>
#include <WiFi.h>
#include "alarm.h"

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
  sprite.setTextColor(DARKGREY, TFT_BLACK);
  
  // 縦線（17本）
  for (int i = 0; i <= 16; ++i) {
    int x = i * GRID_WIDTH;
    sprite.drawLine(x, 0, x, SCREEN_HEIGHT, DARKGREY);
  }
  
  // 横線（13本）
  for (int i = 0; i <= 12; ++i) {
    int y = i * GRID_HEIGHT;
    sprite.drawLine(0, y, SCREEN_WIDTH, y, DARKGREY);
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
  if (diff < 0) return "00:00:00";
  
  int hours = diff / 3600;
  int minutes = (diff % 3600) / 60;
  int seconds = diff % 60;
  
  char buffer[32];
  sprintf(buffer, "%02d:%02d:%02d", hours, minutes, seconds);
  return String(buffer);
}

void drawProgressBar(int x, int y, int width, int height, float progress) {
  sprite.drawRect(x, y, width, height, AMBER_COLOR);
  int progressWidth = (width - 2) * progress;
  sprite.fillRect(x + 1, y + 1, width - 2, height - 2, DARKGREY);
  sprite.fillRect(x + 1, y + 1, progressWidth, height - 2, AMBER_COLOR);
}

void drawInvertedText(const char* text, int x, int y, int font) {
  sprite.setTextFont(font);
  // 現在のDATUMを保存
  uint8_t currentDatum = sprite.getTextDatum();
  sprite.setTextDatum(TL_DATUM);
  int lineHeight = sprite.fontHeight(font);
  // デバッグ用: 塗りつぶし範囲を明示
  sprite.fillRect(0, y, 320, lineHeight, AMBER_COLOR);
  sprite.setTextColor(TFT_BLACK); // 黒文字
  sprite.drawString(text, x, y);
  sprite.setTextColor(AMBER_COLOR); // デフォルト色に戻す
  sprite.setTextDatum(currentDatum); // 元のDATUMに戻す
}

// 共通のYES/NO確認画面を表示する関数
bool showYesNoDialog(const char* title, const char* detail) {
  sprite.fillSprite(TFT_BLACK);
  sprite.setTextDatum(MC_DATUM);
  sprite.setTextColor(FLASH_ORANGE, TFT_BLACK);
  sprite.setTextFont(4);
  sprite.drawString(title, SCREEN_WIDTH/2, 100);
  
  if (detail != NULL) {
    sprite.drawString(detail, SCREEN_WIDTH/2, 140);
  }
  
  sprite.setTextFont(2);
  sprite.drawString("A: YES  C: NO", SCREEN_WIDTH/2, 180);
  sprite.pushSprite(0, 0);
  
  // 確認待ち
  while (true) {
    M5.update();
    if (M5.BtnA.wasPressed()) {
      return true; // YES
    }
    if (M5.BtnC.wasPressed()) {
      return false; // NO
    }
    delay(10);
  }
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
  drawButtonHintsGrid("ABS", "REL+", "MGMT");
  
  time_t now = time(NULL);
  
  // --- 現在時刻（グリッドセル(0,1)-(15,2)、Font4、垂直中央寄せ） ---
  sprite.setTextDatum(MC_DATUM);
  sprite.setTextFont(4);
  sprite.setTextColor(AMBER_COLOR, TFT_BLACK);
  struct tm* tminfo = localtime(&now);
  char hmStr[6];
  sprintf(hmStr, "%02d:%02d", tminfo->tm_hour, tminfo->tm_min);
  sprite.drawString(hmStr, SCREEN_WIDTH/2, GRID_Y(1) + GRID_HEIGHT);
  
  // --- 次のアラームまでの残り時間（グリッドセル(0,3)-(15,5)、Font7、垂直中央寄せ） ---
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
    sprite.drawString(getRemainTimeString(now, nextAlarm), SCREEN_WIDTH/2, GRID_Y(3) + GRID_HEIGHT * 1.5);
  } else {
    sprite.drawString("00:00:00", SCREEN_WIDTH/2, GRID_Y(3) + GRID_HEIGHT * 1.5);
  }
  
  // --- プログレスバー（グリッドセル(0,6)-(15,7)、高さ16px） ---
  extern time_t lastReleaseTime;
  float progress = 0.0f;
  if (nextAlarm && lastReleaseTime && nextAlarm > lastReleaseTime) {
    float total = nextAlarm - lastReleaseTime;
    float done = now - lastReleaseTime;
    progress = (done >= 0 && total > 0) ? (done / total) : 0.0f;
    if (progress < 0) progress = 0;
    if (progress > 1) progress = 1;
  }
  
  // プログレスバーの位置計算（垂直中央寄せ）
  int progressY = GRID_Y(6) + (2 * GRID_HEIGHT - 16) / 2; // 16px高さで中央寄せ
  
  // プログレスバーの境界線
  sprite.drawRect(GRID_X(0), progressY, 16 * GRID_WIDTH, 16, AMBER_COLOR);
  // プログレスバーの背景
  sprite.fillRect(GRID_X(0) + 1, progressY + 1, 16 * GRID_WIDTH - 2, 14, TFT_BLACK);
  // プログレスバーの進捗
  int progressWidth = (16 * GRID_WIDTH - 2) * progress;
  if (progressWidth > 0) {
    sprite.fillRect(GRID_X(0) + 1, progressY + 1, progressWidth, 14, AMBER_COLOR);
  }
  
  // --- アラームリスト（グリッドセル(0,8)-(15,9)、Font2、最大5件、水平配置、垂直中央寄せ） ---
  sprite.setTextDatum(ML_DATUM); // 中央左寄せに変更
  sprite.setTextFont(2);
  sprite.setTextColor(AMBER_COLOR, TFT_BLACK);
  
  int count = 0;
  for (time_t t : futureAlarms) {
    if (count >= 5) break;
    int x = GRID_X(1) + count * (14 * GRID_WIDTH / 5); // X=1から開始、5等分して配置
    sprite.drawString(getTimeString(t), x, GRID_Y(8) + GRID_HEIGHT/2);
    count++;
  }
  
  sprite.pushSprite(0, 0);
}

void drawNTPSync() {
  sprite.fillSprite(TFT_BLACK);
  drawTitleBar("NTP SYNC");
  drawButtonHintsGrid(NULL, NULL, "SKIP");
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

  // --- 時刻入力表示（グリッドセル(0,3)-(15,5)、Font7、水平中央寄せ） ---
  char buf[6];
  snprintf(buf, sizeof(buf), "%d%d:%d%d",
    digitEditInput.hourTens,
    digitEditInput.hourOnes,
    digitEditInput.minTens,
    digitEditInput.minOnes);
  
  sprite.setTextDatum(MC_DATUM);
  sprite.setTextFont(7);
  
  // 時刻表示の中央位置を計算
  int centerX = SCREEN_WIDTH / 2;
  int centerY = GRID_Y(3) + GRID_HEIGHT * 1.5; // グリッドセル(0,3)-(15,5)の中央
  
  // 時刻表示の開始位置を計算（5文字分の幅を考慮）
  int totalWidth = 5 * 40; // 5文字 × 40px（概算）
  int startX = centerX - totalWidth / 2 + 20; // MC_DATUM用に20px調整
  
  int x = startX;
  for (int i = 0; i < 5; ++i) {
    if (i == 2) {
      sprite.setTextColor(AMBER_COLOR, TFT_BLACK);
      sprite.drawString(":", x, centerY);
      x += 40;
      continue;
    }
    int digitIdx = (i < 2) ? i : i - 1;
    if (digitEditInput.cursor == digitIdx) {
      sprite.setTextColor(TFT_BLACK, AMBER_COLOR); // ネガポジ反転
      sprite.drawString(String(buf[i]), x, centerY);
      sprite.setTextColor(AMBER_COLOR, TFT_BLACK);
    } else {
      sprite.drawString(String(buf[i]), x, centerY);
    }
    x += 40;
  }

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
  drawTitleBar("INFO");
  drawButtonHintsGrid(NULL, NULL, "BACK");
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

void drawAlarmManagement() {
  sprite.fillSprite(TFT_BLACK);
  drawTitleBar("ALARM MGMT");
  drawButtonHintsGrid("PREV", "NEXT", "DELETE");
  
  // --- アラームリストの表示（グリッドセル(0,1)-(15,9)） ---
  sprite.setTextFont(4); // Font4に戻す
  sprite.setTextColor(AMBER_COLOR, TFT_BLACK);
  sprite.setTextDatum(ML_DATUM); // 左寄せに変更
  
  extern int scheduleSelectedIndex; // 外部変数の宣言
  
  int yStart = GRID_Y(1);
  int lineHeight = 30; // Font4用に行間を調整
  int maxItems = 9; // グリッドセル(0,1)-(15,9)に収まる最大アイテム数
  int x = GRID_X(1); // X=1で左寄せ
  
  for (int i = 0; i < alarmTimes.size() && i < maxItems; i++) {
    int y = yStart + i * lineHeight;
    
    if (i == scheduleSelectedIndex) {
      // 選択中の項目は水平方向のすべてを反転
      int fontHeight = sprite.fontHeight(4); // Font4の実際の高さを取得
      // ML_DATUMの場合、yは文字の中心位置なので、背景は文字の上端から開始
      int backgroundY = y - fontHeight/2; // 文字の中心から上端までの距離
      sprite.fillRect(0, backgroundY, 320, fontHeight, AMBER_COLOR);
      sprite.setTextColor(TFT_BLACK, AMBER_COLOR);
      sprite.drawString(getTimeString(alarmTimes[i]), x, y);
      sprite.setTextColor(AMBER_COLOR, TFT_BLACK);
    } else {
      sprite.setTextFont(4); // 非選択項目も同じフォントサイズを使用
      sprite.drawString(getTimeString(alarmTimes[i]), x, y);
    }
  }
  
  // アラームが0件の場合のメッセージ
  if (alarmTimes.empty()) {
    sprite.setTextFont(2);
    sprite.setTextColor(AMBER_COLOR, TFT_BLACK);
    sprite.setTextDatum(MC_DATUM); // 中央寄せに戻す
    sprite.drawString("NO ALARMS", SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
  }
  
  sprite.pushSprite(0, 0);
}

void drawAlarmActive() {
  static bool flash = false;
  static unsigned long lastFlash = 0;
  static unsigned long alarmStart = 0;
  
  if (alarmStart == 0) alarmStart = millis();
  unsigned long elapsed = millis() - alarmStart;
  
  // 0.5秒ごとにON/OFF切り替え（最大5秒間）
  if (elapsed < 5000) {
    if (millis() - lastFlash >= 500) {
      flash = !flash;
      lastFlash = millis();
    }
    
    // 画面点滅（フラッシュオレンジ⇔黒）
    if (flash) {
      sprite.fillSprite(FLASH_ORANGE);
      sprite.setTextColor(TFT_BLACK, FLASH_ORANGE);
    } else {
      sprite.fillSprite(TFT_BLACK);
      sprite.setTextColor(FLASH_ORANGE, TFT_BLACK);
    }
    
    // --- オーバータイム表示（グリッドセル(0,3)-(15,5)、Font7、フラッシュオレンジ） ---
    sprite.setTextDatum(MC_DATUM);
    sprite.setTextFont(7);
    
    time_t now = time(NULL);
    time_t alarmTime = getNextAlarmTime();
    if (alarmTime > 0) {
      time_t overtime = now - alarmTime;
      if (overtime > 0) {
        int hours = overtime / 3600;
        int minutes = (overtime % 3600) / 60;
        int seconds = overtime % 60;
        
        char overtimeStr[16];
        sprintf(overtimeStr, "+%02d:%02d:%02d", hours, minutes, seconds);
        sprite.drawString(overtimeStr, SCREEN_WIDTH/2, GRID_Y(3) + GRID_HEIGHT * 1.5);
      }
    }
    
    sprite.pushSprite(0, 0);
  } else {
    // 5秒経過で自動停止
    sprite.fillSprite(TFT_BLACK);
    sprite.pushSprite(0, 0);
  }
}

// 警告メッセージ表示機能
static unsigned long warningStartTime = 0;
static char currentWarningMessage[64] = "";
static bool warningActive = false;

void showWarningMessage(const char* message, unsigned long duration) {
  strncpy(currentWarningMessage, message, sizeof(currentWarningMessage) - 1);
  currentWarningMessage[sizeof(currentWarningMessage) - 1] = '\0';
  warningStartTime = millis();
  warningActive = true;
  
  // 警告メッセージを表示
  sprite.fillSprite(TFT_BLACK);
  sprite.setTextDatum(MC_DATUM);
  sprite.setTextColor(FLASH_ORANGE, TFT_BLACK);
  sprite.setTextFont(4);
  sprite.drawString(currentWarningMessage, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
  sprite.pushSprite(0, 0);
  
  Serial.print("Warning message displayed: ");
  Serial.println(message);
}

bool isWarningMessageDisplayed(const char* message) {
  if (!warningActive) return false;
  
  // 指定されたメッセージと一致するかチェック
  if (strcmp(currentWarningMessage, message) == 0) {
    // 表示時間が経過したかチェック
    if (millis() - warningStartTime >= 3000) {
      warningActive = false;
      return false;
    }
    return true;
  }
  return false;
}

void clearWarningMessage() {
  warningActive = false;
  currentWarningMessage[0] = '\0';
  Serial.println("Warning message cleared");
}
