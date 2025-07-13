#include "button_event.h"

// ボタンイベント作成関数
ButtonEvent createButtonEvent(ButtonType button, ButtonAction action) {
  return ButtonEvent(button, action);
}

// ボタンイベント妥当性チェック
bool isValidButtonEvent(const ButtonEvent& event) {
  // 基本的な妥当性チェック
  if (event.button < BUTTON_A || event.button > BUTTON_C) {
    return false;
  }
  
  if (event.action < SHORT_PRESS || event.action > RELEASE) {
    return false;
  }
  
  // タイムスタンプの妥当性チェック（現在時刻から1分以内）
  unsigned long currentTime = millis();
  if (event.timestamp > currentTime || 
      currentTime - event.timestamp > 60000) {
    return false;
  }
  
  return true;
} 