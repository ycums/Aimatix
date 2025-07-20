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
  buttonManager.initialize();

  Serial.println("Initialised.");
}

void loop() {
  M5.update();
  buttonManager.update();
  // 物理ボタン状態をButtonManagerに反映
  buttonManager.setButtonState(BUTTON_TYPE_A, M5.BtnA.isPressed());
  buttonManager.setButtonState(BUTTON_TYPE_B, M5.BtnB.isPressed());
  buttonManager.setButtonState(BUTTON_TYPE_C, M5.BtnC.isPressed());
 
  // 短押しテスト出力
  if (buttonManager.isShortPressed(BUTTON_TYPE_A)) {
    Serial.println("[TEST] Aボタン短押し検出");
  }
  if (buttonManager.isShortPressed(BUTTON_TYPE_B)) {
    Serial.println("[TEST] Bボタン短押し検出");
  }
  if (buttonManager.isShortPressed(BUTTON_TYPE_C)) {
    Serial.println("[TEST] Cボタン短押し検出");
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
      case ABS_TIME_INPUT:
      case REL_PLUS_TIME_INPUT:
        drawInputMode();
        break;
      default:
        drawMainDisplay();
        break;
    }
    sprite.pushSprite(0, 0);
  }
  delay(10);
}