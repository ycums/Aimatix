#include <M5Stack.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <vector>
#include <EEPROM.h>
#include "ui.h"
#include "settings.h"
#include "alarm.h"
#include "input.h"
#include "types.h"
// #include "button_manager.h"
#include "debounce_manager.h"
#include "button_manager.h"
// #include "button_adapter.h"

// Constants
#define WIFI_TIMEOUT 20000  // 20秒のタイムアウト
#define MAX_ALARMS 5

// Global variables defined in main.cpp
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp.nict.jp", 32400, 3600);  // JST (+9:00)
enum Mode currentMode = MAIN_DISPLAY; // Wi-Fi同期をスキップし直接メイン画面へ

// settingsMenuの唯一の定義（types.hでextern宣言されているもの）
SettingsMenu settingsMenu;

// Global variables defined in other files (declared as extern)
extern InputState inputState; // Declared in input.h
extern Settings settings; // Declared in settings.h
extern std::vector<time_t> alarmTimes; // Declared in alarm.h
int scheduleSelectedIndex = 0;

// Function declarations defined in main.cpp
bool connectWiFi();
bool syncTime();
void handleButtons();
void handleSettingsMenu();
void removePastAlarms();

// WiFi credentials
const char* ssid = "your-ssid"; // Placeholder
const char* password = "your-password"; // Placeholder

// スケジュール選択モード用の選択インデックス
// static int scheduleSelectedIndex = 0; // This line is removed as it's now extern

time_t lastReleaseTime = 0;

void setup() {
  M5.begin();
  M5.Power.begin();
  Serial.begin(115200); // デバッグ用シリアル通信を開始

  initUI(); // スプライト初期化を追加

  // Initialize EEPROM
  EEPROM.begin(EEPROM_SIZE);
  loadSettings();
  
  // Set initial LCD brightness
  M5.Lcd.setBrightness(settings.lcd_brightness);
  
  // Set display properties
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextColor(AMBER_COLOR, TFT_BLACK);
  
  // デバッグ用：アラームリストに初期値を5件追加
  addDebugAlarms();
  
  // ButtonManagerの初期化
  ButtonManager::initialize(); // ButtonManagerの初期化を追加
  Serial.println("ButtonManager initialized");
  
  // Wi-Fi/NTP同期は一時的にスキップ
  // if (connectWiFi()) {
  //   timeClient.begin();
  //   syncTime();
  // }
  // デバッグ用: システム時刻を仮で設定（必要なら）
  // settimeofday などで適当な時刻をセットしてもよい
}

void loop() {
  removePastAlarms();
  M5.update();  // ボタン状態を更新
  
  // ButtonManagerの状態更新（IButtonインターフェース対応）
  ButtonManager::updateButtonStates(); // ButtonManagerの状態更新を追加
  
  handleButtons(); // ボタン処理を追加

  // 画面更新は100ms間隔で行う
  static unsigned long lastScreenUpdate = 0;
  bool needsUpdate = false;
  
  // バッテリー残量とステータスバーの更新（1秒間隔）
  static unsigned long lastStatusUpdate = 0;
  if (millis() - lastStatusUpdate >= 1000) {
    needsUpdate = true;
    lastStatusUpdate = millis();
  }
  
  // ボタンが押された場合は即時更新
  if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed() || M5.BtnC.wasPressed()) {
    needsUpdate = true;
  }
  
  // 100ms経過したか、更新が必要な場合のみ画面を更新
  if (millis() - lastScreenUpdate >= 100 || needsUpdate) {
    lastScreenUpdate = millis();
    
    // 現在のモードに応じた表示と処理
    switch (currentMode) {
      case MAIN_DISPLAY:
        drawMainDisplay();
        break;
      case NTP_SYNC:
        drawNTPSync();
        break;
      case ABS_TIME_INPUT:
      case REL_PLUS_TIME_INPUT:
      case REL_MINUS_TIME_INPUT:
        drawInputMode();
        break;
      case SCHEDULE_SELECT:
        drawScheduleSelection();
        break;
      case ALARM_MANAGEMENT:
        drawAlarmManagement();
        break;
      case ALARM_ACTIVE:
        drawAlarmActive();
        break;
      case SETTINGS_MENU:
        drawSettingsMenu();
        break;
      case INFO_DISPLAY:
        drawInfoDisplay();
        break;
      case WARNING_COLOR_TEST:
        drawWarningColorTest();
        break;
    }
    
    // スプライトの内容を実際の画面に反映
    sprite.pushSprite(0, 0);
  }
  
  // アラーム時刻のチェックと鳴動処理
  if (currentMode != ALARM_ACTIVE) {
    time_t nextAlarm = getNextAlarmTime();
    if (nextAlarm > 0 && time(NULL) >= nextAlarm) {
      currentMode = ALARM_ACTIVE;
      playAlarm();
    }
  }
  
  // 警告色テスト画面からの戻り処理
  if (currentMode == WARNING_COLOR_TEST && M5.BtnC.wasPressed()) {
    currentMode = SETTINGS_MENU;
  }
  
  // 警告メッセージ表示中のボタン処理
  if (isWarningMessageDisplayed("")) {
    if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed() || M5.BtnC.wasPressed()) {
      // 任意のボタンで警告メッセージを消す
      extern void clearWarningMessage();
      clearWarningMessage();
    }
  }
  
  delay(10);  // CPU負荷軽減のための短い遅延
}

bool connectWiFi() {
  WiFi.begin(ssid, password);
  unsigned long startAttemptTime = millis();
  
  while (WiFi.status() != WL_CONNECTED && 
         millis() - startAttemptTime < WIFI_TIMEOUT) {
    delay(500);
    M5.Lcd.drawString("Connecting to WiFi...", 10, 120, 4);
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    M5.Lcd.drawString("WiFi Connected!", 10, 120, 4);
    delay(1000);
    return true;
  } else {
    M5.Lcd.drawString("WiFi Connection Failed!", 10, 120, 4);
    delay(2000);
    return false;
  }
}

bool syncTime() {
  timeClient.update();
  // Unix時間をローカルタイムに設定
  timeval tv = { (time_t)timeClient.getEpochTime(), 0 }; // Cast to time_t
  settimeofday(&tv, NULL);
  return true; // NTPClient::update() は成功/失敗を返さないため常にtrueを返す
}



void handleSettingsMenu() {
  if (M5.BtnA.wasPressed()) {
    settingsMenu.selectedItem = (settingsMenu.selectedItem - 1 + settingsMenu.itemCount) % settingsMenu.itemCount;
  }
  if (M5.BtnB.wasPressed()) {
    settingsMenu.selectedItem = (settingsMenu.selectedItem + 1) % settingsMenu.itemCount;
  }
  if (M5.BtnC.wasPressed()) {
    switch (settingsMenu.selectedItem) {
      case 0:  // Sound
        settings.sound_enabled = !settings.sound_enabled;
        break;
      case 1:  // Vibration
        // Vibration設定の処理（現在は未実装）
        break;
      case 2:  // LCD Brightness
        settings.lcd_brightness = (settings.lcd_brightness + 50) % 251;
        if (settings.lcd_brightness < 50) settings.lcd_brightness = 50;
        M5.Lcd.setBrightness(settings.lcd_brightness);
        break;
      case 3:  // Warning Color Test
        currentMode = WARNING_COLOR_TEST;
        break;
      case 4:  // All Clear
        // 共通の確認画面を使用
        if (showYesNoDialog("CLEAR ALL ALARMS?", NULL)) {
          alarmTimes.clear();
        }
        break;
      case 5:  // Info
        currentMode = INFO_DISPLAY;
        break;
    }
    saveSettings();
  }
}

void handleButtons() {
  // 警告メッセージ表示中はボタン処理をスキップ
  extern bool isWarningMessageDisplayed(const char* message);
  if (isWarningMessageDisplayed("")) {
    // 警告メッセージ表示中は通常のボタン処理をスキップ
    return;
  }
  
  // 既存のデバウンス処理（DebounceManagerを使用）
  // if (!DebounceManager::canProcessModeChange()) {
  //   return;
  // }
  
  // ボタン処理用の変数
  static unsigned long lastPress = 0;
  static bool cLongPressHandled = false;
  static bool bLongPressHandled = false;
  const unsigned long LONG_PRESS_TIME = 1000;  // 1秒間の長押し
  
  // Cボタン処理（元の実装に戻す）
  if (currentMode != ABS_TIME_INPUT && currentMode != REL_PLUS_TIME_INPUT && currentMode != REL_MINUS_TIME_INPUT) {
    if (M5.BtnC.wasPressed()) {
      lastPress = millis();
      cLongPressHandled = false;
      Serial.println("Main: C button pressed - common handler");
    }
    if (M5.BtnC.pressedFor(LONG_PRESS_TIME)) {
      if (!cLongPressHandled && currentMode != MAIN_DISPLAY) {
        Serial.println("Main: C button long press - returning to main");
        currentMode = MAIN_DISPLAY;
        cLongPressHandled = true;
        return; // 他の処理をスキップ
      }
    }
  }
  
  // メイン画面C長押し処理（元の実装に戻す）
  if (currentMode == MAIN_DISPLAY && M5.BtnC.pressedFor(LONG_PRESS_TIME)) {
    if (!cLongPressHandled) {
      currentMode = SETTINGS_MENU;
      cLongPressHandled = true;
      return; // 他の処理をスキップ
    }
  }
  
  // 既存のswitch文（そのまま）
  switch (currentMode) {
    case MAIN_DISPLAY:
      if (M5.BtnA.wasPressed()) {
        currentMode = ABS_TIME_INPUT;
        resetInput();
      }
      if (M5.BtnB.wasPressed()) {
        lastPress = millis();
        bLongPressHandled = false;
      }
      if (M5.BtnB.pressedFor(LONG_PRESS_TIME)) {
        if (!bLongPressHandled) {
          // Bボタン長押しは削除（相対時刻減算画面への遷移を無効化）
          bLongPressHandled = true;
        }
      } else if (M5.BtnB.wasReleased() && millis() - lastPress < LONG_PRESS_TIME) {
        currentMode = REL_PLUS_TIME_INPUT;
        resetInput();
      }
      if (M5.BtnC.wasPressed()) {
        lastPress = millis();
        cLongPressHandled = false;
      }
      if (M5.BtnC.wasReleased() && millis() - lastPress < LONG_PRESS_TIME) {
        currentMode = ALARM_MANAGEMENT; // C短押しでアラーム管理画面へ
      }
      break;

    case ABS_TIME_INPUT:
    case REL_PLUS_TIME_INPUT:
    case REL_MINUS_TIME_INPUT:
      handleDigitEditInput();
      drawInputMode();
      break;

    case ALARM_MANAGEMENT: {
      int listSize = alarmTimes.size();
      if (M5.BtnA.wasPressed()) {
        // PREV: 前の項目へ
        if (scheduleSelectedIndex > 0) {
          scheduleSelectedIndex--;
        }
      }
      if (M5.BtnB.wasPressed()) {
        // NEXT: 次の項目へ
        if (scheduleSelectedIndex < listSize - 1) {
          scheduleSelectedIndex++;
        }
      }
      if (M5.BtnC.wasPressed()) {
        lastPress = millis();
        cLongPressHandled = false;
      }
      if (M5.BtnC.pressedFor(LONG_PRESS_TIME)) {
        if (!cLongPressHandled) {
          // C長押し: メイン画面に戻る
          currentMode = MAIN_DISPLAY;
          cLongPressHandled = true;
          return;
        }
      } else if (M5.BtnC.wasReleased() && millis() - lastPress < LONG_PRESS_TIME) {
        // C短押し: DELETE処理
        if (scheduleSelectedIndex < alarmTimes.size()) {
          // 共通の確認画面を使用
          if (showYesNoDialog("DELETE ALARM?", getTimeString(alarmTimes[scheduleSelectedIndex]).c_str())) {
            // 削除実行
            alarmTimes.erase(alarmTimes.begin() + scheduleSelectedIndex);
            if (scheduleSelectedIndex >= alarmTimes.size() && alarmTimes.size() > 0) {
              scheduleSelectedIndex = alarmTimes.size() - 1;
            }
          }
        }
      }
      break;
    }

    case SETTINGS_MENU:
      handleSettingsMenu();
      break;

    case SCHEDULE_SELECT: {
      int listSize = alarmTimes.size() + 1; // +1 for SETTINGS
      if (M5.BtnB.wasPressed()) {
        scheduleSelectedIndex = (scheduleSelectedIndex + 1) % listSize;
      }
      if (M5.BtnC.wasPressed()) {
        scheduleSelectedIndex = (scheduleSelectedIndex - 1 + listSize) % listSize;
      }
      if (M5.BtnA.wasPressed()) {
        // アラーム削除（SETTINGS選択時は無効）
        if (scheduleSelectedIndex < alarmTimes.size()) {
          alarmTimes.erase(alarmTimes.begin() + scheduleSelectedIndex);
          if (scheduleSelectedIndex >= alarmTimes.size() && alarmTimes.size() > 0) {
            scheduleSelectedIndex = alarmTimes.size() - 1;
          }
        }
      }
      // 決定（SETTINGSならSETTINGS_MENUへ遷移）
      if (M5.BtnC.pressedFor(LONG_PRESS_TIME) || M5.BtnB.pressedFor(LONG_PRESS_TIME)) {
        if (scheduleSelectedIndex == alarmTimes.size()) {
          currentMode = SETTINGS_MENU;
        } else {
          // アラーム選択時は何もしない（将来編集モード等に拡張可）
          currentMode = MAIN_DISPLAY;
        }
      }
      break;
    }
    
    case INFO_DISPLAY:
      drawInfoDisplay();
      break;
  }
  
  // 新しく追加: ButtonManagerの状態更新（既存処理には影響なし）
  ButtonManager::updateButtonStates();
}