#include "button_manager.h"

// 静的メンバ変数の定義
std::map<Button*, ButtonState> ButtonManager::buttonStates;
unsigned long ButtonManager::lastUpdateTime = 0;

// 初期化
void ButtonManager::initialize() {
  buttonStates.clear();
  lastUpdateTime = 0;
  
  // 初期状態を設定
  getOrCreateButtonState(M5.BtnA);
  getOrCreateButtonState(M5.BtnB);
  getOrCreateButtonState(M5.BtnC);
  
  Serial.println("ButtonManager initialized");
}

// ボタン状態の更新
void ButtonManager::updateButtonStates() {
  unsigned long currentTime = millis();
  
  // 各ボタンの状態を更新
  updateButtonState(M5.BtnA, currentTime);
  updateButtonState(M5.BtnB, currentTime);
  updateButtonState(M5.BtnC, currentTime);
  
  lastUpdateTime = currentTime;
}

// 個別ボタンの状態更新
void ButtonManager::updateButtonState(Button& button, unsigned long currentTime) {
  ButtonState& state = getOrCreateButtonState(button);
  
  // 現在の状態を保存
  bool wasPressed = state.isPressed;
  state.isPressed = button.isPressed();
  
  // 状態変化の検出
  if (state.isPressed && !wasPressed) {
    // 押下開始
    state.wasPressed = true;
    state.pressStartTime = currentTime;
    state.longPressHandled = false;
    state.pressCount++;
    state.lastChangeTime = currentTime;
  } else if (!state.isPressed && wasPressed) {
    // リリース
    state.wasReleased = true;
    state.lastChangeTime = currentTime;
  } else {
    // 状態変化なし
    state.wasPressed = false;
    state.wasReleased = false;
  }
  
  // ハードウェアデバウンス適用
  applyHardwareDebounce(state);
}

// 短押し判定
bool ButtonManager::isShortPress(Button& button, unsigned long threshold) {
  ButtonState* state = getButtonState(button);
  if (!state) return false;
  
  // デバウンス処理を適用
  if (!canProcessButton(button)) {
    return false;
  }
  
  // リリースされた瞬間に短押しかどうかを判定
  if (state->wasReleased) {
    unsigned long pressDuration = millis() - state->pressStartTime;
    return pressDuration < threshold;
  }
  
  return false;
}

// 長押し判定
bool ButtonManager::isLongPress(Button& button, unsigned long threshold) {
  ButtonState* state = getButtonState(button);
  if (!state) return false;
  
  // デバウンス処理を適用
  if (!canProcessButton(button)) {
    return false;
  }
  
  // 押されている状態で長押し時間を超えているかチェック
  if (state->isPressed && !state->longPressHandled) {
    unsigned long pressDuration = millis() - state->pressStartTime;
    if (pressDuration >= threshold) {
      state->longPressHandled = true;  // 重複処理を防ぐ
      return true;
    }
  }
  
  return false;
}

// リリース判定
bool ButtonManager::isReleased(Button& button) {
  ButtonState* state = getButtonState(button);
  if (!state) return false;
  
  // デバウンス処理を適用
  if (!canProcessButton(button)) {
    return false;
  }
  
  return state->wasReleased;
}

// デバウンス処理（DebounceManagerと連携）
bool ButtonManager::canProcessButton(Button& button) {
  return DebounceManager::canProcessHardware(button);
}

// 状態リセット
void ButtonManager::resetButtonStates() {
  buttonStates.clear();
  lastUpdateTime = 0;
  Serial.println("ButtonManager states reset");
}

// ボタン状態の取得
ButtonState* ButtonManager::getButtonState(Button& button) {
  auto it = buttonStates.find(&button);
  if (it != buttonStates.end()) {
    return &(it->second);
  }
  return nullptr;
}

// ボタン状態の取得または作成
ButtonState& ButtonManager::getOrCreateButtonState(Button& button) {
  auto it = buttonStates.find(&button);
  if (it == buttonStates.end()) {
    // 新しい状態を作成
    ButtonState newState = {
      false,  // isPressed
      false,  // wasPressed
      false,  // wasReleased
      0,      // pressStartTime
      0,      // lastChangeTime
      0,      // pressCount
      false   // longPressHandled
    };
    buttonStates[&button] = newState;
    return buttonStates[&button];
  }
  return it->second;
}

// ハードウェアデバウンス適用
void ButtonManager::applyHardwareDebounce(ButtonState& state) {
  // 既存のDebounceManagerのハードウェアレベルデバウンスを適用
  // 状態変化から一定時間経過していない場合は、状態を無効化
  unsigned long currentTime = millis();
  if (currentTime - state.lastChangeTime < 50) {  // 50msデバウンス
    // 状態変化が新しい場合は、一時的に無効化
    // 実際の実装では、より高度なデバウンス処理を適用
  }
} 