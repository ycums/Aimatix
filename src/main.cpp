#include <M5Stack.h>
#include "ui.h"
#include <settings.h>
#include "m5stack_adapters.h"
#include "types.h"
#include <alarm.h>
#include "../lib/libaimatix/include/IButtonManager.h"
#include "../lib/libaimatix/src/button_manager.h"
#include <libaimatix/src/input.h>
#include <libaimatix/src/ui_logic.h>

// drawMainDisplay用ダミー変数
#include <time.h>
time_t lastReleaseTime = 0;

// モード管理
Mode currentMode = MAIN_DISPLAY;
// settingsMenuの唯一の定義
SettingsMenu settingsMenu;

// ButtonManagerインスタンス生成（millisをDI）
ButtonManager buttonManager(millis);

DigitEditTimeInputState digitEditInput; // main.cppで状態を保持

Settings appSettings;

int scheduleSelectedIndex = 0;

void setup() {
  M5.begin();
  M5.Power.begin();
  Serial.begin(115200);
  // 設定値ロード
  loadSettings(&eepromAdapter, appSettings);
  Serial.print("lcd_brightness: ");
  Serial.println(appSettings.lcd_brightness);
  // LCD明度を設定値で反映
  M5.Lcd.setBrightness(appSettings.lcd_brightness);
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
  if (buttonManager.isShortPress(BUTTON_TYPE_A)) {
    Serial.println("[TEST] Aボタン短押し検出");
  }
  if (buttonManager.isShortPress(BUTTON_TYPE_B)) {
    Serial.println("[TEST] Bボタン短押し検出");
  }
  if (buttonManager.isShortPress(BUTTON_TYPE_C)) {
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
 
  // 入力モード時はInputLogicのボタン処理を呼ぶ
  if (currentMode == ABS_TIME_INPUT || currentMode == REL_PLUS_TIME_INPUT) {
    InputEventResult inputResult = handleDigitEditInput(&buttonManager, (TimeFunction)millis, digitEditInput);
    if (inputResult == InputEventResult::Confirmed || inputResult == InputEventResult::Cancelled) {
      currentMode = MAIN_DISPLAY;
    }
  }
  // 設定メニューの選択・決定・保存・反映・画面遷移をmain.cppで一元管理
  if (currentMode == SETTINGS_MENU) {
    // Cボタン短押しで決定
    if (buttonManager.isShortPress(BUTTON_TYPE_C)) {
      switch (settingsMenu.selectedItem) {
        case 0: // SOUND
          appSettings.sound_enabled = !appSettings.sound_enabled;
          saveSettings(&eepromAdapter, appSettings);
          break;
        case 1: // LCD BRIGHTNESS
          appSettings.lcd_brightness = (appSettings.lcd_brightness + 50) % 256;
          if (appSettings.lcd_brightness == 0) appSettings.lcd_brightness = 1;
          saveSettings(&eepromAdapter, appSettings);
          M5.Lcd.setBrightness(appSettings.lcd_brightness);
          break;
        case 2: // WARNING COLOR TEST
          currentMode = WARNING_COLOR_TEST;
          break;
        case 3: // ALL CLEAR
          resetSettings(&eepromAdapter, appSettings);
          M5.Lcd.setBrightness(appSettings.lcd_brightness);
          break;
        case 4: // INFO
          currentMode = INFO_DISPLAY;
          break;
      }
    }
    // A/Bボタンで項目移動
    if (buttonManager.isShortPress(BUTTON_TYPE_A)) {
      settingsMenu.selectedItem = prevMenuIndex(settingsMenu.selectedItem, settingsMenu.itemCount);
    }
    if (buttonManager.isShortPress(BUTTON_TYPE_B)) {
      settingsMenu.selectedItem = nextMenuIndex(settingsMenu.selectedItem, settingsMenu.itemCount);
    }
  }
  // 画面更新のみ（100ms間隔）
  static unsigned long lastScreenUpdate = 0;
  if (millis() - lastScreenUpdate >= 100) {
    lastScreenUpdate = millis();
    switch (currentMode) {
      case MAIN_DISPLAY:
        drawMainDisplay(appSettings);
        break;
      case SETTINGS_MENU:
        drawSettingsMenu(appSettings);
        break;
      case INFO_DISPLAY:
        drawInfoDisplay();
        break;
      case ABS_TIME_INPUT:
      case REL_PLUS_TIME_INPUT:
        drawInputMode(digitEditInput); // 状態を渡して描画
        break;
      default:
        drawMainDisplay(appSettings);
        break;
    }
    sprite.pushSprite(0, 0);
  }
  delay(10);
}