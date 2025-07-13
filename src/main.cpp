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
#include "debounce_manager.h"
#include "button_manager.h"

// 新しい状態遷移システムのインクルード
#include "state_transition/button_event.h"
#include "state_transition/system_state.h"
#include "state_transition/transition_result.h"
#include "state_transition/transition_validator.h"
#include "state_transition/state_transition.h"

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

// グローバル変数（他のファイルでextern宣言されているもの）
int scheduleSelectedIndex = 0;
time_t lastReleaseTime = 0;

// Function declarations defined in main.cpp
bool connectWiFi();
bool syncTime();
void handleButtons();
void removePastAlarms();

// WiFi credentials
const char* ssid = "your-ssid"; // Placeholder
const char* password = "your-password"; // Placeholder

// 古い変数は削除（新しい状態遷移システムで管理）

void setup() {
  M5.begin();
  M5.Power.begin();
  Serial.begin(115200); // シリアル通信を開始

  initUI(); // スプライト初期化を追加

  // Initialize EEPROM
  EEPROM.begin(EEPROM_SIZE);
  loadSettings();
  
  // Set initial LCD brightness
  M5.Lcd.setBrightness(settings.lcd_brightness);
  
  // Set display properties
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextColor(AMBER_COLOR, TFT_BLACK);
  
  // アラームリストに初期値を追加（開発用）
  addDebugAlarms();
  
  // ButtonManagerの初期化
  ButtonManager::initialize(); // ButtonManagerの初期化を追加
  Serial.println("ButtonManager initialized");
  
  // Wi-Fi/NTP同期は一時的にスキップ
  // if (connectWiFi()) {
  //   timeClient.begin();
  //   syncTime();
  // }
  // システム時刻設定（必要に応じて）
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
        drawInputMode();
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



// handleSettingsMenu関数は削除（新しい状態遷移システムで管理）

// ボタンイベントをM5Stackボタン状態から作成する関数
ButtonEvent createButtonEventFromM5Stack() {
  const unsigned long LONG_PRESS_TIME = 1000;  // 1秒間の長押し
  
  // Aボタンの処理
  if (M5.BtnA.wasPressed()) {
    return ButtonEvent(BUTTON_TYPE_A, SHORT_PRESS);
  } else if (M5.BtnA.pressedFor(LONG_PRESS_TIME)) {
    return ButtonEvent(BUTTON_TYPE_A, LONG_PRESS);
  }
  
  // Bボタンの処理
  if (M5.BtnB.wasPressed()) {
    return ButtonEvent(BUTTON_TYPE_B, SHORT_PRESS);
  } else if (M5.BtnB.pressedFor(LONG_PRESS_TIME)) {
    return ButtonEvent(BUTTON_TYPE_B, LONG_PRESS);
  }
  
  // Cボタンの処理
  if (M5.BtnC.wasPressed()) {
    return ButtonEvent(BUTTON_TYPE_C, SHORT_PRESS);
  } else if (M5.BtnC.pressedFor(LONG_PRESS_TIME)) {
    return ButtonEvent(BUTTON_TYPE_C, LONG_PRESS);
  }
  
  // ボタンが押されていない場合は無効なイベントを返す
  return ButtonEvent();
}

// 遷移結果に基づいてアクションを実行する関数
void executeTransitionAction(const TransitionResult& result) {
  switch (result.action) {
    case ACTION_RESET_INPUT:
      resetInput();
      break;
    case ACTION_ADD_ALARM:
      // アラーム追加処理は入力処理で行われる
      break;
    case ACTION_DELETE_ALARM:
      // アラーム削除処理は既存の処理で行われる
      break;
    case ACTION_UPDATE_SETTINGS:
      saveSettings();
      break;
    case ACTION_PLAY_ALARM:
      playAlarm();
      break;
    case ACTION_STOP_ALARM:
      stopAlarm();
      break;
    case ACTION_SHOW_WARNING:
      showWarningMessage(result.errorMessage);
      break;
    case ACTION_CLEAR_WARNING:
      clearWarningMessage();
      break;
    case ACTION_NONE:
    default:
      break;
  }
}

void handleButtons() {
  // 警告メッセージ表示中はボタン処理をスキップ
  extern bool isWarningMessageDisplayed(const char* message);
  if (isWarningMessageDisplayed("")) {
    // 警告メッセージ表示中は通常のボタン処理をスキップ
    return;
  }
  
  // 新しい状態遷移システムを使用
  ButtonEvent event = createButtonEventFromM5Stack();
  
  // 無効なイベントの場合は処理をスキップ
  if (!isValidButtonEvent(event)) {
    return;
  }
  
  // 現在のシステム状態を取得
  SystemState currentState = getCurrentSystemState();
  
  // 状態遷移を処理
  TransitionResult result = StateTransitionManager::handleStateTransition(
    currentMode, event, currentState
  );
  
  // 遷移結果を処理
  if (result.isValid) {
    // 有効な遷移の場合
    if (result.nextMode != currentMode) {
      // モードが変更される場合
      currentMode = result.nextMode;
      Serial.print("Mode changed to: ");
      Serial.println(result.nextMode);
    }
    
    // アクションを実行
    executeTransitionAction(result);
  } else {
    // 無効な遷移の場合
    if (result.errorMessage != nullptr) {
      Serial.print("Invalid transition: ");
      Serial.println(result.errorMessage);
    }
  }
  
  // 入力モードの処理は新しい状態遷移システムで管理
  
  // アラーム管理画面の処理は新しい状態遷移システムで管理
  
  // 設定メニューの処理は新しい状態遷移システムで管理
  
  // ButtonManagerの状態更新
  ButtonManager::updateButtonStates();
}