#ifndef BUTTON_EVENT_H
#define BUTTON_EVENT_H

#include <Arduino.h>
#include "../../lib/libaimatix/include/IButtonManager.h"
#include "../../lib/libaimatix/src/types.h"

// RELEASEアクションが必要な場合は独自定義
#ifndef RELEASE
#define RELEASE 2
#endif

// ボタンイベント構造体
struct ButtonEvent {
  ButtonType button;
  ButtonAction action;
  unsigned long timestamp;
  
  ButtonEvent(ButtonType btn, ButtonAction act) 
    : button(btn), action(act), timestamp(millis()) {}
    
  ButtonEvent() 
    : button(BUTTON_TYPE_A), action(SHORT_PRESS), timestamp(0) {}
};

// ボタンイベント変換関数の宣言
ButtonEvent createButtonEvent(ButtonType button, ButtonAction action);
bool isValidButtonEvent(const ButtonEvent& event);

#endif // BUTTON_EVENT_H 