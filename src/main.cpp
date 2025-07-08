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

// Function declarations defined in main.cpp
bool connectWiFi();
bool syncTime();
void handleButtons();
void handleSettingsMenu();

// WiFi credentials
const char* ssid = "your-ssid"; // Placeholder
const char* password = "your-password"; // Placeholder

void setup() {
  M5.begin();
  M5.Power.begin();
  
  // Initialize EEPROM
  EEPROM.begin(EEPROM_SIZE);
  loadSettings();
  
  // Set initial LCD brightness
  M5.Lcd.setBrightness(settings.lcd_brightness);
  
  // Set display properties
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextColor(AMBER_COLOR, TFT_BLACK);
  
  // Wi-Fi/NTP同期は一時的にスキップ
  // if (connectWiFi()) {
  //   timeClient.begin();
  //   syncTime();
  // }
  // デバッグ用: システム時刻を仮で設定（必要なら）
  // settimeofday などで適当な時刻をセットしてもよい
}

void loop() {
  M5.update();  // ボタン状態を更新
  
  // バッテリー残量とステータスバーの更新
  static unsigned long lastStatusUpdate = 0;
  if (millis() - lastStatusUpdate >= 1000) {
    // ステータスバーの更新処理
    lastStatusUpdate = millis();
  }
  
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
      drawScheduleSelect();
      break;
    case ALARM_ACTIVE:
      drawAlarmActive();
      break;
    case SETTINGS_MENU:
      drawSettingsMenu();
      break;
  }
  
  handleButtons();  // ボタン入力の処理
  
  // アラーム時刻のチェックと鳴動処理
  if (currentMode != ALARM_ACTIVE) {
    time_t nextAlarm = getNextAlarmTime();
    if (nextAlarm > 0 && time(NULL) >= nextAlarm) {
      currentMode = ALARM_ACTIVE;
      playAlarm();
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



void drawAlarmActive() {
  static bool flash = false;
  static unsigned long lastFlash = 0;
  
  if (millis() - lastFlash >= 500) {
    flash = !flash;
    lastFlash = millis();
    
    if (flash) {
      M5.Lcd.fillScreen(FLASH_ORANGE);
      M5.Lcd.setTextColor(TFT_BLACK);
    } else {
      M5.Lcd.fillScreen(TFT_BLACK);
      M5.Lcd.setTextColor(FLASH_ORANGE);
    }
    
    M5.Lcd.drawString("TIME UP!", 80, 100, 4);
    
    // オーバータイムの表示
    time_t now = time(NULL);
    time_t alarmTime = getNextAlarmTime();
    String overtime = getRemainTimeString(alarmTime, now);
    M5.Lcd.drawString("+" + overtime.substring(1), 60, 160, 7);
  }
  
  if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed() || M5.BtnC.wasPressed()) {
    stopAlarm();
    currentMode = MAIN_DISPLAY;
    M5.Lcd.fillScreen(TFT_BLACK);
  }
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
        settings.vibration_enabled = !settings.vibration_enabled;
        break;
      case 2:  // LCD Brightness
        settings.lcd_brightness = (settings.lcd_brightness + 50) % 251;
        if (settings.lcd_brightness < 50) settings.lcd_brightness = 50;
        M5.Lcd.setBrightness(settings.lcd_brightness);
        break;
      case 3:  // All Clear
        // 確認ダイアログの表示
        M5.Lcd.fillScreen(TFT_BLACK);
        M5.Lcd.setTextColor(FLASH_ORANGE);
        M5.Lcd.drawString("ARE YOU SURE?", 60, 100, 4);
        M5.Lcd.drawString("A: Yes  C: No", 80, 140, 4);
        while (true) {
          M5.update();
          if (M5.BtnA.wasPressed()) {
            alarmTimes.clear();
            break;
          }
          if (M5.BtnC.wasPressed()) {
            break;
          }
          delay(10);
        }
        break;
      case 4:  // Info
        M5.Lcd.fillScreen(TFT_BLACK);
        M5.Lcd.setTextColor(AMBER_COLOR);
        M5.Lcd.drawString("M5Stack Timer", 40, 60, 4);
        M5.Lcd.drawString("Version 1.0.0", 40, 100, 2);
        M5.Lcd.drawString("(C) 2025 Your Name", 40, 120, 2);
        M5.Lcd.drawString("MAC: " + WiFi.macAddress(), 40, 140, 2);
        while (!M5.BtnC.wasPressed()) {
          M5.update();
          delay(10);
        }
        break;
    }
    saveSettings();
  }
}

void handleButtons() {
  static unsigned long lastPress = 0;
  const unsigned long LONG_PRESS_TIME = 1000;  // 1秒間の長押し
  
  switch (currentMode) {
    case MAIN_DISPLAY:
      if (M5.BtnA.wasPressed()) {
        currentMode = ABS_TIME_INPUT;
        resetInput();
      }
      if (M5.BtnB.wasPressed()) {
        lastPress = millis();
      }
      if (M5.BtnB.pressedFor(LONG_PRESS_TIME)) {
        currentMode = REL_MINUS_TIME_INPUT;
        resetInput();
      } else if (M5.BtnB.wasReleased() && millis() - lastPress < LONG_PRESS_TIME) {
        currentMode = REL_PLUS_TIME_INPUT;
        resetInput();
      }
      if (M5.BtnC.wasPressed()) {
        lastPress = millis();
      }
      if (M5.BtnC.pressedFor(LONG_PRESS_TIME)) {
        currentMode = SETTINGS_MENU;
      } else if (M5.BtnC.wasReleased() && millis() - lastPress < LONG_PRESS_TIME) {
        currentMode = SCHEDULE_SELECT;
      }
      break;
      
    case ABS_TIME_INPUT:
    case REL_PLUS_TIME_INPUT:
    case REL_MINUS_TIME_INPUT:
      if (M5.BtnA.wasPressed()) {
        // 数字の変更
        if (inputState.currentDigit < 2) {
          int *value = &inputState.hours;
          int digit = inputState.currentDigit;
          int currentValue = *value / (digit == 0 ? 10 : 1) % 10;
          currentValue = (currentValue + 1) % (digit == 0 && *value >= 20 ? 2 : 10);
          *value = (*value / (digit == 0 ? 100 : 10) * (digit == 0 ? 100 : 10)) +
                   (currentValue * (digit == 0 ? 10 : 1)) +
                   (*value % (digit == 0 ? 10 : 1));
        } else {
          int *value = &inputState.minutes;
          int digit = inputState.currentDigit - 2;
          int currentValue = *value / (digit == 0 ? 10 : 1) % 10;
          currentValue = (currentValue + 1) % (digit == 0 && *value >= 50 ? 6 : 10);
          *value = (*value / (digit == 0 ? 100 : 10) * (digit == 0 ? 100 : 10)) +
                   (currentValue * (digit == 0 ? 10 : 1)) +
                   (*value % (digit == 0 ? 10 : 1));
        }
      }
      if (M5.BtnB.wasPressed()) {
        inputState.currentDigit = (inputState.currentDigit + 1) % 4;
      }
      if (M5.BtnC.wasPressed()) {
        time_t now = time(NULL);
        time_t newAlarmTime;
        
        if (currentMode == ABS_TIME_INPUT) {
          struct tm timeinfo = *localtime(&now);
          timeinfo.tm_hour = inputState.hours;
          timeinfo.tm_min = inputState.minutes;
          timeinfo.tm_sec = 0;
          newAlarmTime = mktime(&timeinfo);
          if (newAlarmTime <= now) {
            timeinfo.tm_mday++;
            newAlarmTime = mktime(&timeinfo);
          }
        } else {
          time_t delta = inputState.hours * 3600 + inputState.minutes * 60;
          if (currentMode == REL_PLUS_TIME_INPUT) {
            newAlarmTime = now + delta;
          } else {
            newAlarmTime = now - delta;
          }
        }
        
        if (alarmTimes.size() < MAX_ALARMS && newAlarmTime > now) {
          alarmTimes.push_back(newAlarmTime);
          sortAlarms();
          currentMode = MAIN_DISPLAY;
        }
      }
      if (M5.BtnC.pressedFor(LONG_PRESS_TIME)) {
        currentMode = MAIN_DISPLAY;
      }
      break;
      
    case SETTINGS_MENU:
      handleSettingsMenu();
      if (M5.BtnC.pressedFor(LONG_PRESS_TIME)) {
        currentMode = MAIN_DISPLAY;
      }
      break;
  }
}