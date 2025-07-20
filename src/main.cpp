#include <M5Stack.h>
#include "ui.h"
#include <settings.h>
#include "m5stack_adapters.h"
#include "types.h"
#include <alarm.h>
#include "../lib/libaimatix/include/IButtonManager.h"
#include "../lib/libaimatix/src/button_manager.h"

// drawMainDisplay用ダミー変数
#include <time.h>
time_t lastReleaseTime = 0;

// モード管理
Mode currentMode = MAIN_DISPLAY;
// settingsMenuの唯一の定義
SettingsMenu settingsMenu;

// ButtonManagerインスタンス生成（millisをDI）
ButtonManager buttonManager(millis);

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
  buttonManager.initialize();
}

void loop() {
  M5.update();
  buttonManager.update();
  // 物理ボタン状態をButtonManagerに反映
  buttonManager.setButtonState(BUTTON_TYPE_A, M5.BtnA.isPressed());
  buttonManager.setButtonState(BUTTON_TYPE_B, M5.BtnB.isPressed());
  buttonManager.setButtonState(BUTTON_TYPE_C, M5.BtnC.isPressed());
  // A/B/Cボタンでモード切り替え
  if (buttonManager.isShortPress(BUTTON_TYPE_A)) {
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
  if (buttonManager.isShortPress(BUTTON_TYPE_B)) {
    currentMode = SETTINGS_MENU;
    Serial.println("Mode: SETTINGS_MENU");
  }
  if (buttonManager.isShortPress(BUTTON_TYPE_C)) {
    currentMode = INFO_DISPLAY;
    Serial.println("Mode: INFO_DISPLAY");
  }
  // 長押しテスト出力
  if (buttonManager.isLongPressed(BUTTON_TYPE_A)) {
    Serial.println("[TEST] Aボタン長押し検出");
  }
  if (buttonManager.isLongPressed(BUTTON_TYPE_B)) {
    Serial.println("[TEST] Bボタン長押し検出");
  }
  if (buttonManager.isLongPressed(BUTTON_TYPE_C)) {
    Serial.println("[TEST] Cボタン長押し検出");
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