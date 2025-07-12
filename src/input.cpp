// 入力モードのロジック実装
#include "input.h"
#include "alarm.h"
#include <algorithm>
#include "types.h"
#include <M5Stack.h>
extern Mode currentMode;

// 入力値をバリデーションし、アラームリストに追加
bool confirmInputAndAddAlarm() {
  // 時刻バリデーション（0-23, 0-59）
  if (inputState.hours < 0 || inputState.hours > 23) return false;
  if (inputState.minutes < 0 || inputState.minutes > 59) return false;
  time_t now = time(NULL);
  time_t alarmT = 0;
  if (currentMode == REL_PLUS_TIME_INPUT || currentMode == REL_MINUS_TIME_INPUT) {
    // 相対時刻入力: 現在時刻±入力値
    struct tm tminfo;
    localtime_r(&now, &tminfo);
    int total = inputState.hours * 60 + inputState.minutes;
    if (currentMode == REL_MINUS_TIME_INPUT) total = -total;
    // 現在時刻+total分
    time_t base = now;
    base += total * 60;
    struct tm t;
    localtime_r(&base, &t);
    t.tm_sec = 0;
    alarmT = mktime(&t);
    if (alarmT <= now) return false;
  } else {
    // 絶対時刻入力
  struct tm tminfo;
  localtime_r(&now, &tminfo);
  tminfo.tm_hour = inputState.hours;
  tminfo.tm_min = inputState.minutes;
  tminfo.tm_sec = 0;
    alarmT = mktime(&tminfo);
  if (alarmT <= now) return false;
  }
  // 既存重複チェック
  if (std::find(alarmTimes.begin(), alarmTimes.end(), alarmT) != alarmTimes.end()) return false;
  // 最大件数制限
  if (alarmTimes.size() >= 5) return false;
  alarmTimes.push_back(alarmT);
  sortAlarms();
  return true;
}
#include "input.h"

// 入力モード用の変数
InputState inputState;
RightJustifiedInputState rjInputState;
DigitEditTimeInputState digitEditInput;

void resetInput() {
  inputState = InputState();
  // 桁ごと編集方式の初期化
  digitEditInput.hourTens = DigitEditTimeInputState::INIT_HOUR_TENS;
  digitEditInput.hourOnes = DigitEditTimeInputState::INIT_HOUR_ONES;
  digitEditInput.minTens = DigitEditTimeInputState::INIT_MIN_TENS;
  digitEditInput.minOnes = DigitEditTimeInputState::INIT_MIN_ONES;
  digitEditInput.cursor = 3;
}

// 桁ごと編集方式の時刻入力用ボタン処理
void handleDigitEditInput() {
  static uint32_t aPressStart = 0;
  static bool aLongPressFired = false;

  // Aボタン: +1（短押し）、+5（長押し）
  if (M5.BtnA.wasPressed()) {
    aPressStart = millis();
    aLongPressFired = false;
  }
  if (M5.BtnA.isPressed() && !aLongPressFired && millis() - aPressStart >= 500) {
    // 長押し0.5秒経過時点で+5
    int add = 5;
    switch (digitEditInput.cursor) {
      case 0: digitEditInput.hourTens = (digitEditInput.hourTens + add) % 3; if (digitEditInput.hourTens == 2 && digitEditInput.hourOnes > 3) digitEditInput.hourOnes = 3; break;
      case 1: digitEditInput.hourOnes = (digitEditInput.hourOnes + add) % ((digitEditInput.hourTens == 2) ? 4 : 10); break;
      case 2: digitEditInput.minTens = (digitEditInput.minTens + add) % 6; break;
      case 3: digitEditInput.minOnes = (digitEditInput.minOnes + add) % 10; break;
    }
    aLongPressFired = true;
  }
  if (M5.BtnA.wasReleased()) {
    if (!aLongPressFired && millis() - aPressStart < 500) {
      // 短押し
      int add = 1;
      switch (digitEditInput.cursor) {
        case 0: digitEditInput.hourTens = (digitEditInput.hourTens + add) % 3; if (digitEditInput.hourTens == 2 && digitEditInput.hourOnes > 3) digitEditInput.hourOnes = 3; break;
        case 1: digitEditInput.hourOnes = (digitEditInput.hourOnes + add) % ((digitEditInput.hourTens == 2) ? 4 : 10); break;
        case 2: digitEditInput.minTens = (digitEditInput.minTens + add) % 6; break;
        case 3: digitEditInput.minOnes = (digitEditInput.minOnes + add) % 10; break;
      }
    }
  }
  // Bボタン: 桁送り（短押し）、リセット（長押し）
  if (M5.BtnB.wasReleasefor(1000)) {
    // 長押し（リセット）
    digitEditInput.hourTens = DigitEditTimeInputState::INIT_HOUR_TENS;
    digitEditInput.hourOnes = DigitEditTimeInputState::INIT_HOUR_ONES;
    digitEditInput.minTens = DigitEditTimeInputState::INIT_MIN_TENS;
    digitEditInput.minOnes = DigitEditTimeInputState::INIT_MIN_ONES;
    digitEditInput.cursor = 3;
  } else if (M5.BtnB.wasPressed()) {
    // 短押し（桁送り）
    if (digitEditInput.cursor < 3) digitEditInput.cursor++;
  }
  // Cボタン: セット（確定）
  if (M5.BtnC.wasPressed()) {
    int hour = digitEditInput.hourTens * 10 + digitEditInput.hourOnes;
    int min = digitEditInput.minTens * 10 + digitEditInput.minOnes;
    // バリデーション（例: 00:00は不可、23:59まで）
    if (hour == 0 && min == 0) {
      // エラー表示など
      return;
    }
    if (hour > 23 || min > 59) {
      // エラー表示など
      return;
    }
    // ここで時刻セット処理（例: alarmTimes.push_back(...) など）
    // ...
    // 入力状態リセット
    digitEditInput.hourTens = DigitEditTimeInputState::INIT_HOUR_TENS;
    digitEditInput.hourOnes = DigitEditTimeInputState::INIT_HOUR_ONES;
    digitEditInput.minTens = DigitEditTimeInputState::INIT_MIN_TENS;
    digitEditInput.minOnes = DigitEditTimeInputState::INIT_MIN_ONES;
    digitEditInput.cursor = 3;
  }
}

