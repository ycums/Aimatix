#include "transition_validator.h"

// 遷移の妥当性をチェック
bool TransitionValidator::isValidTransition(Mode currentMode, const ButtonEvent& event, const SystemState& state) {
  // 基本的な妥当性チェック
  if (!isValidButtonEvent(event)) {
    return false;
  }
  
  if (!isValidSystemState(state)) {
    return false;
  }
  
  // 警告表示中の遷移ブロック
  if (isWarningBlockingTransition(state)) {
    return false;
  }
  
  // アラーム鳴動中の遷移ブロック（任意ボタンで停止は許可）
  if (isAlarmBlockingTransition(state)) {
    // アラーム鳴動中は任意ボタンで停止のみ許可
    return (event.action == SHORT_PRESS || event.action == LONG_PRESS);
  }
  
  // 境界値チェック
  if (!isIndexInRange(state.selectedIndex, static_cast<int>(state.alarmCount))) {
    return false;
  }
  
  if (!isAlarmCountValid(state.alarmCount)) {
    return false;
  }
  
  // 入力モードでの特殊チェック
  if (currentMode == ABS_TIME_INPUT || currentMode == REL_PLUS_TIME_INPUT) {
    // 入力モードではCボタンの長押しでメイン画面に戻ることを許可
    if (event.button == BUTTON_C && event.action == LONG_PRESS) {
      return true;
    }
  }
  
  // アラーム管理画面での特殊チェック
  if (currentMode == ALARM_MANAGEMENT) {
    // アラームが存在しない場合はメイン画面に戻ることを許可
    if (state.alarmCount == 0) {
      return true;
    }
  }
  
  return true;
}

// 警告表示中の遷移をチェック
bool TransitionValidator::isWarningBlockingTransition(const SystemState& state) {
  return state.warningDisplayed;
}

// アラーム鳴動中の遷移をチェック
bool TransitionValidator::isAlarmBlockingTransition(const SystemState& state) {
  return state.alarmActive;
}

// 境界値チェック
bool TransitionValidator::isIndexInRange(int index, int maxValue) {
  return index >= 0 && index < maxValue;
}

bool TransitionValidator::isAlarmCountValid(size_t count) {
  return count <= 5; // 最大5個
}

// エラーメッセージの生成
const char* TransitionValidator::getErrorMessage(Mode currentMode, const ButtonEvent& event, const SystemState& state) {
  if (!isValidButtonEvent(event)) {
    return "無効なボタンイベント";
  }
  
  if (!isValidSystemState(state)) {
    return "無効なシステム状態";
  }
  
  if (isWarningBlockingTransition(state)) {
    return "警告表示中";
  }
  
  if (isAlarmBlockingTransition(state)) {
    return "アラーム鳴動中";
  }
  
  if (!isIndexInRange(state.selectedIndex, static_cast<int>(state.alarmCount))) {
    return "選択インデックスが範囲外";
  }
  
  if (!isAlarmCountValid(state.alarmCount)) {
    return "アラーム数が上限を超えています";
  }
  
  // 入力モードでの特殊エラーメッセージ
  if (currentMode == ABS_TIME_INPUT || currentMode == REL_PLUS_TIME_INPUT) {
    if (event.button == BUTTON_C && event.action == SHORT_PRESS) {
      return "入力値が無効です";
    }
  }
  
  // アラーム管理画面での特殊エラーメッセージ
  if (currentMode == ALARM_MANAGEMENT) {
    if (state.alarmCount == 0) {
      return "アラームが存在しません";
    }
  }
  
  return "不明なエラー";
} 