#include "button_manager.h"

ButtonManager::ButtonManager(unsigned long (*getTimeFunc)()) : lastUpdateTime(0), getTime(getTimeFunc) {
  buttonStates.clear();
}
ButtonManager::~ButtonManager() {}

void ButtonManager::initialize() {
  buttonStates.clear();
  lastUpdateTime = 0;
  // 必要に応じて初期化（buttonIdの登録など）
}

void ButtonManager::update() {
  unsigned long currentTime = getTime();
  for (auto& kv : buttonStates) {
    ButtonState& state = kv.second;
    // 長押し判定: 押しっぱなし中、まだ未報告なら1回だけtrueに
    if (state.isPressed && !state.longPressHandled) {
      if (currentTime - state.pressStartTime >= 1000) {
        state.longPressHandled = true;
        state.longPressReported = true;
      }
    }
  }
  lastUpdateTime = currentTime;
}

void ButtonManager::updateButtonState(ButtonType buttonId, unsigned long currentTime) {
  ButtonState& state = getOrCreateButtonState(buttonId);
  // 実際のハード依存部は本番実装/モックで差し替え
  // ここでは仮に外部から状態注入される前提
  // state.isPressed = ...;
  // 状態変化検出・更新ロジックは従来通り
}

bool ButtonManager::isPressed(ButtonType buttonId) {
  ButtonState* state = getButtonState(buttonId);
  return state ? state->isPressed : false;
}

bool ButtonManager::isLongPressed(ButtonType buttonId) {
  ButtonState* state = getButtonState(buttonId);
  if (!state) return false;
  if (!canProcessButton(buttonId)) return false;
  if (state->longPressReported) {
    state->longPressReported = false;
    return true;
  }
  return false;
}

bool ButtonManager::isShortPress(ButtonType buttonId, unsigned long threshold) {
  ButtonState* state = getButtonState(buttonId);
  if (!state) return false;
  if (!canProcessButton(buttonId)) return false;
  if (state->wasReleased) {
    unsigned long pressDuration = getTime() - state->pressStartTime;
    state->wasReleased = false;
    return pressDuration < threshold;
  }
  return false;
}

bool ButtonManager::isReleased(ButtonType buttonId) {
  ButtonState* state = getButtonState(buttonId);
  if (!state) return false;
  if (!canProcessButton(buttonId)) return false;
  if (state->wasReleased) {
    state->wasReleased = false;
    return true;
  }
  return false;
}

void ButtonManager::resetButtonStates() {
  buttonStates.clear();
  lastUpdateTime = 0;
}

ButtonState* ButtonManager::getButtonState(ButtonType buttonId) {
  auto it = buttonStates.find(buttonId);
  if (it != buttonStates.end()) {
    return &(it->second);
  }
  return nullptr;
}

ButtonState& ButtonManager::getOrCreateButtonState(ButtonType buttonId) {
  auto it = buttonStates.find(buttonId);
  if (it == buttonStates.end()) {
    ButtonState newState = {false, false, false, 0, 0, 0, false};
    buttonStates[buttonId] = newState;
    return buttonStates[buttonId];
  }
  return it->second;
}

void ButtonManager::applyHardwareDebounce(ButtonState& state) {
  unsigned long currentTime = getTime();
  if (currentTime - state.lastChangeTime < 50) {
    // デバウンス処理
  }
}

bool ButtonManager::canProcessButton(ButtonType buttonId) {
  // DebounceManagerと連携する場合はここで呼び出し
  return true;
}

void ButtonManager::setButtonState(ButtonType buttonId, bool isPressed) {
  ButtonState& state = getOrCreateButtonState(buttonId);
  bool prevPressed = state.isPressed;
  state.isPressed = isPressed;
  if (!prevPressed && isPressed) {
    state.wasPressed = true;
    state.pressStartTime = getTime();
    // 長押しフラグは押し始め・離したときのみリセット
  } else if (prevPressed && !isPressed) {
    state.wasReleased = true;
    state.longPressHandled = false;
    state.longPressReported = false;
  }
} 