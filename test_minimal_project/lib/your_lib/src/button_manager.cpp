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
  // ここで全ボタンの状態を更新（buttonIdリストをループ）
  for (auto& kv : buttonStates) {
    updateButtonState(kv.first, currentTime);
  }
  lastUpdateTime = currentTime;
}

void ButtonManager::updateButtonState(int buttonId, unsigned long currentTime) {
  ButtonState& state = getOrCreateButtonState(buttonId);
  // 実際のハード依存部は本番実装/モックで差し替え
  // ここでは仮に外部から状態注入される前提
  // state.isPressed = ...;
  // 状態変化検出・更新ロジックは従来通り
}

bool ButtonManager::isPressed(int buttonId) {
  ButtonState* state = getButtonState(buttonId);
  return state ? state->isPressed : false;
}

bool ButtonManager::isLongPressed(int buttonId) {
  ButtonState* state = getButtonState(buttonId);
  return state ? state->longPressHandled : false;
}

bool ButtonManager::isShortPress(int buttonId, unsigned long threshold) {
  ButtonState* state = getButtonState(buttonId);
  if (!state) return false;
  if (!canProcessButton(buttonId)) return false;
  if (state->wasReleased) {
    unsigned long pressDuration = getTime() - state->pressStartTime;
    return pressDuration < threshold;
  }
  return false;
}

bool ButtonManager::isReleased(int buttonId) {
  ButtonState* state = getButtonState(buttonId);
  if (!state) return false;
  if (!canProcessButton(buttonId)) return false;
  return state->wasReleased;
}

void ButtonManager::resetButtonStates() {
  buttonStates.clear();
  lastUpdateTime = 0;
}

ButtonState* ButtonManager::getButtonState(int buttonId) {
  auto it = buttonStates.find(buttonId);
  if (it != buttonStates.end()) {
    return &(it->second);
  }
  return nullptr;
}

ButtonState& ButtonManager::getOrCreateButtonState(int buttonId) {
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

bool ButtonManager::canProcessButton(int buttonId) {
  // DebounceManagerと連携する場合はここで呼び出し
  return true;
} 