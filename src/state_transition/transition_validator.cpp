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
  
  return "不明なエラー";
} 