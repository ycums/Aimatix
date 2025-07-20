#include <M5Stack.h>
#include "ui.h"
#include <settings.h>
#include "m5stack_adapters.h"
#include "types.h"
#include <alarm.h>

// drawMainDisplay用ダミー変数
#include <time.h>
time_t lastReleaseTime = 0;

// モード管理
Mode currentMode = MAIN_DISPLAY;
// settingsMenuの唯一の定義
SettingsMenu settingsMenu;

void setup() {
  M5.begin();
  M5.Power.begin();
  Serial.begin(115200);
  // 設定値ロード
  loadSettings(&eepromAdapter);
  Serial.print("lcd_brightness: ");
  Serial.println(settings.lcd_brightness);
  // LCD明度を設定値で反映
  M5.Lcd.setBrightness(settings.lcd_brightness);
  // UI初期化
  initUI();
  Serial.println("Phase2-3: ボタン遷移・画面遷移最小構成");
}

void loop() {
  M5.update();
  // A/B/Cボタンでモード切り替え
  if (M5.BtnA.wasPressed()) {
    if (currentMode == MAIN_DISPLAY) {
      // テスト用: 1分後のアラームを追加
      time_t now = time(NULL);
      time_t alarmT = now + 60;
      if (std::find(alarmTimes.begin(), alarmTimes.end(), alarmT) == alarmTimes.end() && alarmTimes.size() < 5) {
        alarmTimes.push_back(alarmT);
        sortAlarms();
        Serial.println("[TEST] 1分後のアラームを追加");
      } else {
        Serial.println("[TEST] アラーム追加失敗（重複または上限）");
      }
    }
    currentMode = MAIN_DISPLAY;
    Serial.println("Mode: MAIN_DISPLAY");
  }
  if (M5.BtnB.wasPressed()) {
    currentMode = SETTINGS_MENU;
    Serial.println("Mode: SETTINGS_MENU");
  }
  if (M5.BtnC.wasPressed()) {
    currentMode = INFO_DISPLAY;
    Serial.println("Mode: INFO_DISPLAY");
  }
  // 画面更新のみ（100ms間隔）
  static unsigned long lastScreenUpdate = 0;
  if (millis() - lastScreenUpdate >= 100) {
    lastScreenUpdate = millis();
    switch (currentMode) {
      case MAIN_DISPLAY:
        drawMainDisplay();
        break;
      case SETTINGS_MENU:
        drawSettingsMenu();
        break;
      case INFO_DISPLAY:
        drawInfoDisplay();
        break;
      default:
        drawMainDisplay();
        break;
    }
    sprite.pushSprite(0, 0);
  }
  delay(10);
}