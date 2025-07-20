#include <M5Stack.h>
#include "ui.h"
#include <settings.h>
#include "m5stack_adapters.h"
#include <IButtonManager.h>
#include <ui_state_transition.h>
#include <button_manager.h>
#include <types.h>
#include <alarm.h>
#include <input.h>
#include <ui_logic.h>
#include <command.h>
#include <vector>

// drawMainDisplay用ダミー変数
#include <time.h>
time_t lastReleaseTime = 0;

// モード管理
Mode currentMode = MAIN_DISPLAY;
// settingsMenuの唯一の定義
SettingsMenu settingsMenu;

// ButtonManagerインスタンス生成（millisをDI）
ButtonManager buttonManager(millis);

extern DigitEditTimeInputState digitEditInput;

Settings appSettings;

int scheduleSelectedIndex = 0;

std::vector<Command> commandQueue;

// Effect Handlerクラス例
namespace UiEffectHandler {
    void setBrightness(int value) {
        M5.Lcd.setBrightness(value);
    }
    void showWarning(const char* msg) {
        showWarningMessage(msg);
    }
}
namespace SettingsEffectHandler {
    void save(const Settings& s) {
        saveSettings(&eepromAdapter, s);
    }
}

void processCommands() {
    for (const auto& cmd : commandQueue) {
        switch (cmd.type) {
            case CommandType::SetBrightness:
                UiEffectHandler::setBrightness(cmd.intValue);
                break;
            case CommandType::SaveSettings:
                SettingsEffectHandler::save(appSettings);
                break;
            case CommandType::ShowWarning:
                UiEffectHandler::showWarning(cmd.message);
                break;
            case CommandType::UpdateUI:
                // 必要に応じて再描画
                break;
        }
    }
    commandQueue.clear();
}

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

// ボタンイベント定義をループ外に移動
struct ButtonEvent {
  ButtonType type;
  ButtonAction action;
  const char* testMsg;
};
const ButtonEvent eventOrder[] = {
  {BUTTON_TYPE_A, SHORT_PRESS, "[TEST] Aボタン短押し検出"},
  {BUTTON_TYPE_B, SHORT_PRESS, "[TEST] Bボタン短押し検出"},
  {BUTTON_TYPE_C, SHORT_PRESS, "[TEST] Cボタン短押し検出"},
  {BUTTON_TYPE_A, LONG_PRESS,  "[TEST] Aボタン長押し検出"},
  {BUTTON_TYPE_B, LONG_PRESS,  "[TEST] Bボタン長押し検出"},
  {BUTTON_TYPE_C, LONG_PRESS,  "[TEST] Cボタン長押し検出"},
};

void loop() {
  M5.update();
  buttonManager.update();
  // 物理ボタン状態をButtonManagerに反映
  buttonManager.setButtonState(BUTTON_TYPE_A, M5.BtnA.isPressed());
  buttonManager.setButtonState(BUTTON_TYPE_B, M5.BtnB.isPressed());
  buttonManager.setButtonState(BUTTON_TYPE_C, M5.BtnC.isPressed());
 
  // ボタンイベント検出・画面遷移・テスト出力を一元化
  bool eventHandled = false;
  for (const auto& ev : eventOrder) {
    bool pressed = (ev.action == SHORT_PRESS)
      ? buttonManager.isShortPress(ev.type)
      : buttonManager.isLongPressed(ev.type);
    if (pressed) {
      Serial.println(ev.testMsg);
      currentMode = nextMode(currentMode, ev.type, ev.action);
      eventHandled = true;
      break; // 1ループ1イベントのみ処理
    }
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
    if (buttonManager.isShortPress(BUTTON_TYPE_C)) {
      switch (settingsMenu.selectedItem) {
        case 0: // SOUND
          appSettings.sound_enabled = !appSettings.sound_enabled;
          {
            Command cmd;
            cmd.type = CommandType::SaveSettings;
            commandQueue.push_back(cmd);
          }
          break;
        case 1: // LCD BRIGHTNESS
          appSettings.lcd_brightness = (appSettings.lcd_brightness + 50) % 256;
          if (appSettings.lcd_brightness == 0) appSettings.lcd_brightness = 1;
          {
            Command cmd;
            cmd.type = CommandType::SetBrightness;
            cmd.intValue = appSettings.lcd_brightness;
            commandQueue.push_back(cmd);
          }
          {
            Command cmd;
            cmd.type = CommandType::SaveSettings;
            commandQueue.push_back(cmd);
          }
          break;
        case 2: // WARNING COLOR TEST
          currentMode = WARNING_COLOR_TEST;
          break;
        case 3: // ALL CLEAR
          resetSettings(&eepromAdapter, appSettings);
          {
            Command cmd;
            cmd.type = CommandType::SetBrightness;
            cmd.intValue = appSettings.lcd_brightness;
            commandQueue.push_back(cmd);
          }
          {
            Command cmd;
            cmd.type = CommandType::SaveSettings;
            commandQueue.push_back(cmd);
          }
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
  processCommands();
  delay(10);
}