#include "ui.h"
#include <M5Stack.h>
#include <time.h>
#include <WiFi.h>
#include <alarm.h>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <string>
#include <input.h>
#include <settings.h>
#include <debounce_manager.h>
#include <button_manager.h>
#include <time_logic.h>

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

void draw_title_bar(const char* modeName) {
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
  sprintf(batteryStr, "%s %d%%", isCharging ? "CHG" : "BAT", batteryLevel);
  
  // 残量と充電状態に応じて色を変更
  int batteryColor = AMBER_COLOR;
  if (isCharging) {
    batteryColor = TFT_GREEN; // 充電中は緑色
  } else if (batteryLevel <= 20) {
    batteryColor = FLASH_ORANGE; // 低バッテリーは警告色
  }
  
  sprite.setTextColor(batteryColor, TFT_BLACK);
  sprite.setTextDatum(TR_DATUM);
  sprite.drawString(batteryStr, SCREEN_WIDTH - 5, 2);
}

void draw_button_hints_grid(const char* btnA, const char* btnB, const char* btnC) {
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

void draw_common_ui(const char* modeName) {
  draw_title_bar(modeName);
  // ボタンヒントは各画面で個別に呼び出す（内容が異なるため）
}

void draw_grid_lines() {
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
std::string get_time_string(time_t t) {
  char buffer[9];
  TimeLogic::formatTimeString(t, buffer, sizeof(buffer));
  return std::string(buffer);
}

// 日付文字列の取得
std::string get_date_string(time_t t) {
  struct tm *timeinfo = localtime(&t);
  char buffer[32];
  const char* weekdays[] = {"日", "月", "火", "水", "木", "金", "土"};
  sprintf(buffer, "%04d/%02d/%02d (%s)", 
          timeinfo->tm_year + 1900,
          timeinfo->tm_mon + 1,
          timeinfo->tm_mday,
          weekdays[timeinfo->tm_wday]);
  return std::string(buffer);
}

// 残り時間文字列の取得
std::string get_remain_time_string(time_t now, time_t target) {
  time_t diff = target - now;
  if (diff < 0) return "00:00:00";
  
  int hours = diff / 3600;
  int minutes = (diff % 3600) / 60;
  int seconds = diff % 60;
  
  char buffer[32];
  sprintf(buffer, "%02d:%02d:%02d", hours, minutes, seconds);
  return std::string(buffer);
}

void draw_progress_bar(int x, int y, int width, int height, float progress) {
  sprite.drawRect(x, y, width, height, AMBER_COLOR);
  int progressWidth = (width - 2) * progress;
  sprite.fillRect(x + 1, y + 1, width - 2, height - 2, DARKGREY);
  sprite.fillRect(x + 1, y + 1, progressWidth, height - 2, AMBER_COLOR);
}

void draw_inverted_text(const char* text, int x, int y, int font) {
  sprite.setTextFont(font);
  // 現在のDATUMを保存
  uint8_t currentDatum = sprite.getTextDatum();
  sprite.setTextDatum(TL_DATUM);
  int lineHeight = sprite.fontHeight(font);
  // 塗りつぶし範囲を明示
  sprite.fillRect(0, y, 320, lineHeight, AMBER_COLOR);
  sprite.setTextColor(TFT_BLACK); // 黒文字
  sprite.drawString(text, x, y);
  sprite.setTextColor(AMBER_COLOR); // デフォルト色に戻す
  sprite.setTextDatum(currentDatum); // 元のDATUMに戻す
}

// 共通のYES/NO確認画面を表示する関数
bool show_yes_no_dialog(const char* title, const char* detail) {
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
// - 残り時間（HH:MM:SS）
// - 進捗バー
// - NEXT時刻
// - 鳴動時刻リスト
void draw_main_display(const Settings& settings) {
  removePastAlarms(); // アラーム一覧を最新化
  sprite.fillSprite(TFT_BLACK);
  draw_title_bar("MAIN");
  draw_button_hints_grid("ABS", "REL+", "MGMT");
  time_t now = TimeLogic::getCurrentTime();
  
  // --- 現在時刻表示 ---
  sprite.setTextDatum(MC_DATUM);
  sprite.setTextFont(4);
  sprite.setTextColor(AMBER_COLOR, TFT_BLACK);
  char hmStr[6];
  TimeLogic::formatTimeString(now, hmStr, sizeof(hmStr));
  sprite.drawString(hmStr, SCREEN_WIDTH/2, GRID_Y(1) + GRID_HEIGHT);
  
  // --- 次のアラームまでの残り時間 ---
  time_t nextAlarm = AlarmLogic::getNextAlarmTime(alarmTimes, now);
  
  sprite.setTextDatum(MC_DATUM);
  sprite.setTextFont(7);
  sprite.setTextColor(AMBER_COLOR, TFT_BLACK);
  if (nextAlarm) {
    sprite.drawString(get_remain_time_string(now, nextAlarm).c_str(), SCREEN_WIDTH/2, GRID_Y(3) + GRID_HEIGHT * 1.5);
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
  for (time_t t : alarmTimes) {
    if (count >= 5) break;
    int x = GRID_X(1) + count * (14 * GRID_WIDTH / 5); // X=1から開始、5等分して配置
    sprite.drawString(get_time_string(t).c_str(), x, GRID_Y(8) + GRID_HEIGHT/2);
    count++;
  }
  
  sprite.pushSprite(0, 0);
}

void draw_ntp_sync() {
  sprite.fillSprite(TFT_BLACK);
  draw_title_bar("NTP SYNC");
  draw_button_hints_grid(NULL, NULL, "SKIP");
  sprite.setTextDatum(MC_DATUM);
  sprite.setTextColor(AMBER_COLOR);
  sprite.drawString("Syncing Time...", 160, 120);
}

void draw_input_mode(const DigitEditTimeInputState& state) {
  sprite.fillSprite(TFT_BLACK);
  draw_title_bar("INPUT MODE");
  draw_button_hints_grid("INC", "NEXT", "SET");

  sprite.setTextFont(7);
  sprite.setTextColor(AMBER_COLOR, TFT_BLACK);
  sprite.setTextDatum(MC_DATUM);

  // 入力中の時刻を表示
  char timeStr[6];
  snprintf(timeStr, sizeof(timeStr), "%d%d:%d%d",
           state.hourTens, state.hourOnes, state.minTens, state.minOnes);
  sprite.drawString(timeStr, SCREEN_WIDTH/2, GRID_Y(3));

  // カーソル位置を強調表示（例：下線や反転など、ここでは下線を描画）
  // 旧: int cursorX = SCREEN_WIDTH/2 - 48 + state.cursor * 24;
  // 新: 右端未入力桁の位置を強調
  int highlightIdx = 3;
  for (int i = 3; i >= 0; --i) {
    if (!state.entered[i]) { highlightIdx = i; break; }
  }
  int cursorX = SCREEN_WIDTH/2 - 48 + highlightIdx * 24;
  int cursorY = GRID_Y(3) + 40;
  sprite.drawLine(cursorX, cursorY, cursorX + 20, cursorY, AMBER_COLOR);

  sprite.pushSprite(0, 0);
}







void draw_settings_menu(const Settings& settings) {
  sprite.fillSprite(TFT_BLACK);
  draw_title_bar("SETTINGS");
  draw_button_hints_grid("PREV", "NEXT", "SELECT");

  // 設定値を表示するメニュー項目
  char soundStr[32];
  snprintf(soundStr, sizeof(soundStr), "SOUND: %s", settings.sound_enabled ? "ON" : "OFF");
  char brightnessStr[32];
  snprintf(brightnessStr, sizeof(brightnessStr), "LCD BRIGHTNESS: %d", settings.lcd_brightness);

  const char* items[] = {
    soundStr,
    brightnessStr,
    "WARNING COLOR TEST",
    "ALL CLEAR",
    "INFO"
  };
  settingsMenu.itemCount = sizeof(items) / sizeof(items[0]);

  draw_menu_items(items, settingsMenu.itemCount, settingsMenu.selectedItem, 0, 2, 1);
  sprite.pushSprite(0, 0);
}

void draw_info_display() {
  sprite.fillSprite(TFT_BLACK);
  draw_title_bar("INFO");
  draw_button_hints_grid(NULL, NULL, "BACK");
  sprite.setTextDatum(TL_DATUM);
  sprite.setTextColor(AMBER_COLOR);
  sprite.drawString("M5Stack Timer", 40, 60);
  sprite.drawString("Version 1.0.0", 40, 100);
  sprite.drawString("(C) 2025 Your Name", 40, 120);
  sprite.drawString("MAC: 00:00:00:00:00:00", 40, 140);
}

// 後方互換性のための関数（既存コードとの互換性を保つ）
void draw_status_bar(const char* mode) {
  draw_title_bar(mode);
}

void draw_button_hints(const char* btnA, const char* btnB, const char* btnC) {
  draw_button_hints_grid(btnA, btnB, btnC);
}

void draw_menu_items(const char** items, int itemCount, int selectedItem, int startGridRow, int endGridRow, int marginTop) {
  sprite.setTextDatum(TL_DATUM);
  sprite.setTextColor(AMBER_COLOR);
  
  int startY = GRID_Y(startGridRow) + marginTop;
  int itemHeight = 25; // 行間
  int totalHeight = itemCount * itemHeight;
  int availableHeight = (endGridRow - startGridRow + 1) * GRID_HEIGHT;
  int centerY = startY + (availableHeight - totalHeight) / 2; // 中央揃え
  
  for (int i = 0; i < itemCount; ++i) {
    const char* itemStr = items[i];
    int y = centerY + i * itemHeight;
    if (i == selectedItem) {
      draw_inverted_text(itemStr, GRID_X(1), y, 2); // 選択項目を反転
    } else {
      sprite.setTextFont(2);
      sprite.drawString(itemStr, GRID_X(1), y);
    }
  }
}

void draw_warning_color_test() {
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
  
  draw_title_bar("WARNING TEST");
  draw_button_hints_grid(NULL, NULL, "BACK");
  
  sprite.pushSprite(0, 0);
}

void draw_alarm_management() {
  sprite.fillSprite(TFT_BLACK);
  draw_title_bar("ALARM MGMT");
  draw_button_hints_grid("DEL", "NEXT", "PREV");
  
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
      sprite.drawString(get_time_string(alarmTimes[i]).c_str(), x, y);
      sprite.setTextColor(AMBER_COLOR, TFT_BLACK);
    } else {
      sprite.setTextFont(4); // 非選択項目も同じフォントサイズを使用
      sprite.drawString(get_time_string(alarmTimes[i]).c_str(), x, y);
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

void draw_alarm_active() {
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
    
    time_t now = TimeLogic::getCurrentTime();
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

void show_warning_message(const char* message, unsigned long duration) {
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

bool is_warning_message_displayed(const char* message) {
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

void clear_warning_message() {
  warningActive = false;
  currentWarningMessage[0] = '\0';
  Serial.println("Warning message cleared");
}
