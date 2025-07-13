#include "system_state.h"
#include <M5Stack.h>

// 外部変数の宣言
extern enum Mode currentMode;
extern SettingsMenu settingsMenu;
extern InputState inputState;
extern std::vector<time_t> alarmTimes;
extern int scheduleSelectedIndex;

// 現在のシステム状態を取得
SystemState getCurrentSystemState() {
  SystemState state;
  
  // 現在のモード
  state.currentMode = currentMode;
  
  // 選択インデックス
  state.selectedIndex = scheduleSelectedIndex;
  
  // アラーム数
  state.alarmCount = alarmTimes.size();
  
  // 警告表示状態（外部関数を使用）
  extern bool isWarningMessageDisplayed(const char* message);
  state.warningDisplayed = isWarningMessageDisplayed("");
  
  // アラーム鳴動状態
  state.alarmActive = (currentMode == ALARM_ACTIVE);
  
  // 入力状態
  state.inputState = inputState;
  
  // 設定メニュー状態
  state.settingsMenu = settingsMenu;
  
  // 現在時刻
  state.currentTime = time(NULL);
  
  return state;
}

// システム状態を更新
void updateSystemState(SystemState& state) {
  // 現在の状態で更新
  state = getCurrentSystemState();
}

// システム状態の妥当性チェック
bool isValidSystemState(const SystemState& state) {
  // モードの妥当性チェック
  if (state.currentMode < MAIN_DISPLAY || state.currentMode > WARNING_COLOR_TEST) {
    return false;
  }
  
  // 選択インデックスの妥当性チェック
  if (state.selectedIndex < 0) {
    return false;
  }
  
  // アラーム数の妥当性チェック
  if (state.alarmCount > 5) { // 最大5個
    return false;
  }
  
  // 選択インデックスとアラーム数の整合性チェック
  if (state.selectedIndex >= static_cast<int>(state.alarmCount) && state.alarmCount > 0) {
    return false;
  }
  
  // 設定メニューの妥当性チェック
  if (state.settingsMenu.selectedItem < 0 || 
      state.settingsMenu.selectedItem >= state.settingsMenu.itemCount) {
    return false;
  }
  
  return true;
} 