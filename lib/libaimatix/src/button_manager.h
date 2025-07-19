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
  bool isPressed(ButtonType buttonId) override;
  bool isLongPressed(ButtonType buttonId) override;
  void update() override;

  // 追加API
  bool isShortPress(ButtonType buttonId, unsigned long threshold = 1000);
  bool isReleased(ButtonType buttonId);
  void resetButtonStates();
  ButtonState* getButtonState(ButtonType buttonId);
  void initialize();

private:
  std::map<ButtonType, ButtonState> buttonStates;
  unsigned long lastUpdateTime;
  unsigned long (*getTime)(); // DIされた現在時刻取得関数
  void updateButtonState(ButtonType buttonId, unsigned long currentTime);
  void applyHardwareDebounce(ButtonState& state);
  ButtonState& getOrCreateButtonState(ButtonType buttonId);
  static bool canProcessButton(ButtonType buttonId);
};

#endif // BUTTON_MANAGER_H 