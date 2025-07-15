#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

#include <vector>
#include <time.h>
#include "types.h"
#include <input.h>

// システム状態構造体
struct SystemState {
  Mode currentMode;              // 現在のモード
  int selectedIndex;             // 現在の選択インデックス
  size_t alarmCount;             // アラーム数
  bool warningDisplayed;         // 警告表示中か
  bool alarmActive;              // アラーム鳴動中か
  InputState inputState;         // 入力状態
  SettingsMenu settingsMenu;     // 設定メニュー状態
  time_t currentTime;            // 現在時刻
  
  SystemState() 
    : currentMode(MAIN_DISPLAY), selectedIndex(0), alarmCount(0),
      warningDisplayed(false), alarmActive(false), currentTime(0) {}
};

// システム状態管理関数の宣言
SystemState getCurrentSystemState();
void updateSystemState(SystemState& state);
bool isValidSystemState(const SystemState& state);

#endif // SYSTEM_STATE_H 