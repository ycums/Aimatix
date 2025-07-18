#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include <map>
#include "../include/IButtonManager.h"
#include "debounce_manager.h"

// ボタン状態の構造体
struct ButtonState {
  bool isPressed;
  bool wasPressed;
  bool wasReleased;
  unsigned long pressStartTime;
  unsigned long lastChangeTime;
  int pressCount;
  bool longPressHandled;
};

// IButtonManager実装クラス
class ButtonManager : public IButtonManager {
public:
  // 現在時刻取得関数をDI
  explicit ButtonManager(unsigned long (*getTimeFunc)());
  ~ButtonManager() override;

  // IButtonManagerインターフェース
  bool isPressed(int buttonId) override;
  bool isLongPressed(int buttonId) override;
  void update() override;

  // 追加API
  bool isShortPress(int buttonId, unsigned long threshold = 1000);
  bool isReleased(int buttonId);
  void resetButtonStates();
  ButtonState* getButtonState(int buttonId);
  void initialize();

private:
  std::map<int, ButtonState> buttonStates;
  unsigned long lastUpdateTime;
  unsigned long (*getTime)(); // DIされた現在時刻取得関数
  void updateButtonState(int buttonId, unsigned long currentTime);
  void applyHardwareDebounce(ButtonState& state);
  ButtonState& getOrCreateButtonState(int buttonId);
  static bool canProcessButton(int buttonId);
};

#endif // BUTTON_MANAGER_H 