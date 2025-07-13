#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include <map>
#include "../test/mocks/mock_m5stack.h"
#include "debounce_manager.h"

// ボタン状態の構造体
struct ButtonState {
  bool isPressed;           // 現在押されているか
  bool wasPressed;          // 前回の更新で押されたか
  bool wasReleased;         // 前回の更新でリリースされたか
  unsigned long pressStartTime;  // 押下開始時刻
  unsigned long lastChangeTime;  // 最後の状態変化時刻
  int pressCount;           // 押下回数
  bool longPressHandled;    // 長押し処理済みフラグ
};

// ButtonManagerクラス（静的クラス）
class ButtonManager {
public:
  // 基本的な判定（既存のM5.BtnX.wasPressed()をラップ）
  static bool isShortPress(Button& button, unsigned long threshold = 1000);
  static bool isLongPress(Button& button, unsigned long threshold = 1000);
  static bool isReleased(Button& button);
  
  // 状態管理
  static void updateButtonStates();
  static void resetButtonStates();
  static ButtonState* getButtonState(Button& button);
  
  // デバウンス処理（DebounceManagerと連携）
  static bool canProcessButton(Button& button);
  
  // 初期化
  static void initialize();

private:
  // 静的メンバ変数
  static std::map<Button*, ButtonState> buttonStates;
  static unsigned long lastUpdateTime;
  
  // 内部処理関数
  static void updateButtonState(Button& button, unsigned long currentTime);
  static void applyHardwareDebounce(ButtonState& state);
  static ButtonState& getOrCreateButtonState(Button& button);
};

#endif // BUTTON_MANAGER_H 