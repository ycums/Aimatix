#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include <M5Stack.h>
#include <map>
#include <vector>

// ボタン状態を管理する構造体
struct ButtonState {
  bool isPressed;
  bool wasPressed;
  bool wasReleased;
  unsigned long pressStartTime;
  unsigned long lastChangeTime;
  int pressCount;
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
  
private:
  static std::map<Button*, ButtonState> buttonStates;
  static void applyHardwareDebounce(ButtonState& state);
  
  // ハードウェアデバウンス時間（50ms）
  static const unsigned long HARDWARE_DEBOUNCE_TIME = 50;
};

#endif // BUTTON_MANAGER_H 