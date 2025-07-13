#ifndef BUTTON_EVENT_H
#define BUTTON_EVENT_H

#include <Arduino.h>

// ボタンタイプの定義
enum ButtonType {
  BUTTON_TYPE_A,
  BUTTON_TYPE_B, 
  BUTTON_TYPE_C
};

// ボタンアクションの定義
enum ButtonAction {
  SHORT_PRESS,    // 短押し
  LONG_PRESS,     // 長押し（1秒以上）
  RELEASE         // リリース
};

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