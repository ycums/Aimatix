#include "button_manager.h"

// 静的メンバ変数の定義
std::map<Button*, ButtonState> ButtonManager::buttonStates;

// ハードウェアデバウンス時間
const unsigned long ButtonManager::HARDWARE_DEBOUNCE_TIME = 50;

// ボタン状態の更新
void ButtonManager::updateButtonStates() {
  // M5Stackの3つのボタンを管理
  Button* buttons[] = {&M5.BtnA, &M5.BtnB, &M5.BtnC};
  
  for (Button* button : buttons) {
    // ボタンが初回登録されていない場合は初期化
    if (buttonStates.find(button) == buttonStates.end()) {
      buttonStates[button] = ButtonState{false, false, false, 0, 0, 0};
    }
    
    ButtonState& state = buttonStates[button];
    
    // 現在のボタン状態を取得
    bool currentPressed = button->isPressed();
    
    // 状態が変化した場合
    if (currentPressed != state.isPressed) {
      unsigned long currentTime = millis();
      
      // ハードウェアデバウンスを適用
      if (currentTime - state.lastChangeTime >= HARDWARE_DEBOUNCE_TIME) {
        // 状態を更新
        state.isPressed = currentPressed;
        state.lastChangeTime = currentTime;
        
        // 押下開始
        if (currentPressed) {
          state.pressStartTime = currentTime;
          state.wasPressed = true;
          state.pressCount++;
        } else {
          // 離上
          state.wasReleased = true;
        }
      }
    }
  }
}

// 短押し判定
bool ButtonManager::isShortPress(Button& button, unsigned long threshold) {
  auto it = buttonStates.find(&button);
  if (it == buttonStates.end()) {
    return false;
  }
  
  ButtonState& state = it->second;
  
  // 離上された直後で、押下時間が閾値未満の場合
  if (state.wasReleased && !state.isPressed) {
    unsigned long pressDuration = millis() - state.pressStartTime;
    if (pressDuration < threshold) {
      // 判定後はフラグをリセット
      state.wasReleased = false;
      return true;
    }
  }
  
  return false;
}

// 長押し判定
bool ButtonManager::isLongPress(Button& button, unsigned long threshold) {
  auto it = buttonStates.find(&button);
  if (it == buttonStates.end()) {
    return false;
  }
  
  ButtonState& state = it->second;
  
  // 現在押下中で、押下時間が閾値を超えた場合
  if (state.isPressed) {
    unsigned long pressDuration = millis() - state.pressStartTime;
    if (pressDuration >= threshold) {
      return true;
    }
  }
  
  return false;
}

// 離上判定
bool ButtonManager::isReleased(Button& button) {
  auto it = buttonStates.find(&button);
  if (it == buttonStates.end()) {
    return false;
  }
  
  ButtonState& state = it->second;
  
  // 離上フラグが立っている場合
  if (state.wasReleased) {
    // 判定後はフラグをリセット
    state.wasReleased = false;
    return true;
  }
  
  return false;
}

// ボタン状態のリセット
void ButtonManager::resetButtonStates() {
  for (auto& pair : buttonStates) {
    ButtonState& state = pair.second;
    state.isPressed = false;
    state.wasPressed = false;
    state.wasReleased = false;
    state.pressStartTime = 0;
    state.lastChangeTime = 0;
    state.pressCount = 0;
  }
}

// ハードウェアデバウンスの適用（現在はupdateButtonStates内で実装済み）
void ButtonManager::applyHardwareDebounce(ButtonState& state) {
  // 現在はupdateButtonStates内で直接実装しているため、
  // この関数は将来の拡張用に残しておく
} 