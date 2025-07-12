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
  static uint32_t bPressStart = 0;
  static bool bLongPressFired = false;
  static uint32_t cPressStart = 0;
  static bool cLongPressFired = false;

  // Aボタン: +1（短押し）、+5（長押し）
  if (M5.BtnA.wasPressed()) {
    aPressStart = millis();
    aLongPressFired = false;
  }
  if (M5.BtnA.isPressed() && !aLongPressFired && millis() - aPressStart >= 500) {
    // 長押し0.5秒経過時点で+5（1回のみ）
    int add = 5;
    switch (digitEditInput.cursor) {
      case 0: // 時十の位
        digitEditInput.hourTens = (digitEditInput.hourTens + add) % 3;
        if (digitEditInput.hourTens == 2 && digitEditInput.hourOnes > 3) {
          digitEditInput.hourOnes = 3; // 制限
        }
        break;
      case 1: { // 時一の位
        int maxHourOnes = (digitEditInput.hourTens == 2) ? 4 : 10;
        digitEditInput.hourOnes = (digitEditInput.hourOnes + add) % maxHourOnes;
        break;
      }
      case 2: // 分十の位
        digitEditInput.minTens = (digitEditInput.minTens + add) % 6;
        break;
      case 3: // 分一の位
        digitEditInput.minOnes = (digitEditInput.minOnes + add) % 10;
        break;
    }
    aLongPressFired = true;
  }
  if (M5.BtnA.wasReleased()) {
    if (!aLongPressFired && millis() - aPressStart < 500) {
      // 短押し: +1
      int add = 1;
      switch (digitEditInput.cursor) {
        case 0: // 時十の位
          digitEditInput.hourTens = (digitEditInput.hourTens + add) % 3;
          if (digitEditInput.hourTens == 2 && digitEditInput.hourOnes > 3) {
            digitEditInput.hourOnes = 3; // 制限
          }
          break;
        case 1: { // 時一の位
          int maxHourOnes = (digitEditInput.hourTens == 2) ? 4 : 10;
          digitEditInput.hourOnes = (digitEditInput.hourOnes + add) % maxHourOnes;
          break;
        }
        case 2: // 分十の位
          digitEditInput.minTens = (digitEditInput.minTens + add) % 6;
          break;
        case 3: // 分一の位
          digitEditInput.minOnes = (digitEditInput.minOnes + add) % 10;
          break;
      }
    }
  }

  // Bボタン: 桁送り（短押し）、リセット（長押し）
  if (M5.BtnB.wasPressed()) {
    bPressStart = millis();
    bLongPressFired = false;
  }
  if (M5.BtnB.isPressed() && !bLongPressFired && millis() - bPressStart >= 1000) {
    // 長押し1秒経過時点でリセット
    digitEditInput.hourTens = DigitEditTimeInputState::INIT_HOUR_TENS;
    digitEditInput.hourOnes = DigitEditTimeInputState::INIT_HOUR_ONES;
    digitEditInput.minTens = DigitEditTimeInputState::INIT_MIN_TENS;
    digitEditInput.minOnes = DigitEditTimeInputState::INIT_MIN_ONES;
    digitEditInput.cursor = 0; // カーソル位置もリセット
    bLongPressFired = true;
  }
  if (M5.BtnB.wasReleased()) {
    if (!bLongPressFired && millis() - bPressStart < 1000) {
      // 短押し: 桁送り（左から右）
      digitEditInput.cursor = (digitEditInput.cursor + 1) % 4;
    }
  }

  // Cボタン: セット（短押し）、戻る（長押し）
  if (M5.BtnC.wasPressed()) {
    cPressStart = millis();
    cLongPressFired = false;
    Serial.println("C button pressed - start time recorded");
  }
  if (M5.BtnC.isPressed() && !cLongPressFired && millis() - cPressStart >= 1000) {
    // 長押し1秒経過時点でメイン画面に戻る
    Serial.println("C button long press detected - returning to main");
    currentMode = MAIN_DISPLAY;
    cLongPressFired = true;
  }
  if (M5.BtnC.wasReleased()) {
    Serial.print("C button released - press duration: ");
    Serial.print(millis() - cPressStart);
    Serial.println("ms");
    if (!cLongPressFired && millis() - cPressStart < 1000) {
      // 短押し: セット（確定）
      Serial.println("C button short press - confirming alarm");
      int hour = digitEditInput.hourTens * 10 + digitEditInput.hourOnes;
      int min = digitEditInput.minTens * 10 + digitEditInput.minOnes;
      
      // 時刻バリデーション
      if (hour > 23 || min > 59) {
        // エラー: 無効な時刻
        Serial.println("Invalid time - validation failed");
        return;
      }
      
      // アラーム時刻の計算
      time_t now = time(NULL);
      time_t alarmTime = 0;
      
      if (currentMode == ABS_TIME_INPUT) {
        // 絶対時刻入力
        struct tm tminfo;
        localtime_r(&now, &tminfo);
        tminfo.tm_hour = hour;
        tminfo.tm_min = min;
        tminfo.tm_sec = 0;
        alarmTime = mktime(&tminfo);
        
        // 過去時刻の場合は翌日として設定
        if (alarmTime <= now) {
          alarmTime += 24 * 3600; // 24時間追加
        }
      } else if (currentMode == REL_PLUS_TIME_INPUT) {
        // 相対時刻追加
        alarmTime = now + (hour * 3600) + (min * 60);
      }
      
      // 重複チェック
      if (std::find(alarmTimes.begin(), alarmTimes.end(), alarmTime) != alarmTimes.end()) {
        // エラー: 重複
        Serial.println("Duplicate alarm - not added");
        return;
      }
      
      // 最大数チェック
      if (alarmTimes.size() >= 5) {
        // エラー: 最大数超過
        Serial.println("Too many alarms - not added");
        return;
      }
      
      // アラーム追加
      alarmTimes.push_back(alarmTime);
      std::sort(alarmTimes.begin(), alarmTimes.end());
      
      // 入力状態リセット
      resetInput();
      
      // メイン画面に戻る
      Serial.println("Alarm added successfully - returning to main");
      currentMode = MAIN_DISPLAY;
    }
  }
}

