// 入力モードのロジック実装
#include "input.h"
#include "alarm.h"
#include <algorithm>
#include "types.h"
#include <cstdint>
#include "button_manager.h"
#include "debounce_manager.h"
extern Mode currentMode;

// 入力値をバリデーションし、アラームリストに追加
bool confirmInputAndAddAlarm() {
  if (inputState.hours < 0 || inputState.hours > 23) return false;
  if (inputState.minutes < 0 || inputState.minutes > 59) return false;
  time_t now = time(NULL);
  time_t alarmT = 0;
  if (currentMode == REL_PLUS_TIME_INPUT) {
    struct tm tminfo;
#ifdef _WIN32
    localtime_s(&tminfo, &now);
#else
    localtime_r(&now, &tminfo);
#endif
    int total = inputState.hours * 60 + inputState.minutes;
    time_t base = now;
    base += total * 60;
    struct tm t;
#ifdef _WIN32
    localtime_s(&t, &base);
#else
    localtime_r(&base, &t);
#endif
    t.tm_sec = 0;
    alarmT = mktime(&t);
    if (alarmT <= now) return false;
  } else {
    struct tm tminfo;
#ifdef _WIN32
    localtime_s(&tminfo, &now);
#else
    localtime_r(&now, &tminfo);
#endif
    tminfo.tm_hour = inputState.hours;
    tminfo.tm_min = inputState.minutes;
    tminfo.tm_sec = 0;
    alarmT = mktime(&tminfo);
    if (alarmT <= now) return false;
  }
  if (std::find(alarmTimes.begin(), alarmTimes.end(), alarmT) != alarmTimes.end()) return false;
  if (alarmTimes.size() >= 5) return false;
  alarmTimes.push_back(alarmT);
  sortAlarms();
  return true;
}

InputState inputState;
RightJustifiedInputState rjInputState;
DigitEditTimeInputState digitEditInput;

void resetInput() {
  inputState = InputState();
  digitEditInput.hourTens = DigitEditTimeInputState::INIT_HOUR_TENS;
  digitEditInput.hourOnes = DigitEditTimeInputState::INIT_HOUR_ONES;
  digitEditInput.minTens = DigitEditTimeInputState::INIT_MIN_TENS;
  digitEditInput.minOnes = DigitEditTimeInputState::INIT_MIN_ONES;
  digitEditInput.cursor = 3;
}

// 桁ごと編集方式の時刻入力用ボタン処理
void handleDigitEditInput(IButtonManager* buttonManager, TimeFunction timeFunc) {
  if (!buttonManager) return;
  static uint32_t aPressStart = 0;
  static bool aLongPressFired = false;
  static uint32_t bPressStart = 0;
  static bool bLongPressFired = false;
  static uint32_t cPressStart = 0;
  static bool cLongPressFired = false;
  
  // 時間取得関数のデフォルト実装（テスト用）
  uint32_t currentTime = timeFunc ? timeFunc() : 0;

  // Aボタン処理
  if (buttonManager->isPressed(BUTTON_TYPE_A)) {
    if (!aLongPressFired) {
      aPressStart = currentTime;
      aLongPressFired = false;
    }
  }
  if (buttonManager->isLongPressed(BUTTON_TYPE_A) && !aLongPressFired) {
    int add = 5;
    switch (digitEditInput.cursor) {
      case 0:
        digitEditInput.hourTens = (digitEditInput.hourTens + add) % 3;
        if (digitEditInput.hourTens == 2 && digitEditInput.hourOnes > 3) {
          digitEditInput.hourOnes = 3;
        }
        break;
      case 1: {
        int maxHourOnes = (digitEditInput.hourTens == 2) ? 4 : 10;
        digitEditInput.hourOnes = (digitEditInput.hourOnes + add) % maxHourOnes;
        break;
      }
      case 2:
        digitEditInput.minTens = (digitEditInput.minTens + add) % 6;
        break;
      case 3:
        digitEditInput.minOnes = (digitEditInput.minOnes + add) % 10;
        break;
    }
    aLongPressFired = true;
  }
  if (!buttonManager->isPressed(BUTTON_TYPE_A) && !aLongPressFired) {
    if (currentTime - aPressStart < 500) {
      int add = 1;
      switch (digitEditInput.cursor) {
        case 0:
          digitEditInput.hourTens = (digitEditInput.hourTens + add) % 3;
          if (digitEditInput.hourTens == 2 && digitEditInput.hourOnes > 3) {
            digitEditInput.hourOnes = 3;
          }
          break;
        case 1: {
          int maxHourOnes = (digitEditInput.hourTens == 2) ? 4 : 10;
          digitEditInput.hourOnes = (digitEditInput.hourOnes + add) % maxHourOnes;
          break;
        }
        case 2:
          digitEditInput.minTens = (digitEditInput.minTens + add) % 6;
          break;
        case 3:
          digitEditInput.minOnes = (digitEditInput.minOnes + add) % 10;
          break;
      }
    }
  }

  // Bボタン処理
  if (buttonManager->isPressed(BUTTON_TYPE_B)) {
    if (!bLongPressFired) {
      bPressStart = currentTime;
      bLongPressFired = false;
    }
  }
  if (buttonManager->isLongPressed(BUTTON_TYPE_B) && !bLongPressFired) {
    digitEditInput.hourTens = DigitEditTimeInputState::INIT_HOUR_TENS;
    digitEditInput.hourOnes = DigitEditTimeInputState::INIT_HOUR_ONES;
    digitEditInput.minTens = DigitEditTimeInputState::INIT_MIN_TENS;
    digitEditInput.minOnes = DigitEditTimeInputState::INIT_MIN_ONES;
    digitEditInput.cursor = 0;
    bLongPressFired = true;
  }
  if (!buttonManager->isPressed(BUTTON_TYPE_B) && !bLongPressFired) {
    if (currentTime - bPressStart < 1000) {
      digitEditInput.cursor = (digitEditInput.cursor + 1) % 4;
    }
  }

  // Cボタン処理
  if (buttonManager->isPressed(BUTTON_TYPE_C)) {
    if (!cLongPressFired) {
      cPressStart = currentTime;
      cLongPressFired = false;
    }
  }
  if (buttonManager->isLongPressed(BUTTON_TYPE_C) && !cLongPressFired) {
    currentMode = MAIN_DISPLAY;
    cLongPressFired = true;
  }
  if (!buttonManager->isPressed(BUTTON_TYPE_C) && !cLongPressFired) {
    if (currentTime - cPressStart < 1000) {
      // セット（確定）処理等をここに記述
      // 必要に応じてアラーム追加や画面遷移処理を呼び出す
      if (confirmInputAndAddAlarm()) {
        currentMode = MAIN_DISPLAY;
        // 必要に応じて入力リセットやメッセージ表示も可
      } else {
        // 追加失敗時の警告表示等（任意）
      }
    }
  }
}

