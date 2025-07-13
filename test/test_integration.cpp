#include <unity.h>
#include "../src/state_transition/button_event.h"
#include "../src/state_transition/system_state.h"
#include "../src/state_transition/transition_result.h"
#include "../src/state_transition/transition_validator.h"
#include "../src/state_transition/state_transition.h"

// テスト用のモック関数
extern "C" {
  unsigned long millis() { return 1000; }
  time_t time(time_t* t) { return 1000; }
}

// テスト用のグローバル変数
enum Mode currentMode = MAIN_DISPLAY;
SettingsMenu settingsMenu;
InputState inputState;
std::vector<time_t> alarmTimes;
int scheduleSelectedIndex = 0;

// 警告メッセージ表示状態のモック
bool mockWarningDisplayed = false;
bool isWarningMessageDisplayed(const char* message) {
  return mockWarningDisplayed;
}

void setUp(void) {
  currentMode = MAIN_DISPLAY;
  settingsMenu.selectedItem = 0;
  settingsMenu.itemCount = 5;
  inputState = InputState();
  alarmTimes.clear();
  scheduleSelectedIndex = 0;
  mockWarningDisplayed = false;
}

void tearDown(void) {
  // クリーンアップ
}

// 典型的な操作シーケンスのテスト
void test_typical_operation_sequence() {
  SystemState state = getCurrentSystemState();
  
  // 1. メイン画面から絶対時刻入力画面へ
  ButtonEvent aShortPress(BUTTON_TYPE_A, SHORT_PRESS);
  TransitionResult result = StateTransitionManager::handleStateTransition(MAIN_DISPLAY, aShortPress, state);
  TEST_ASSERT_EQUAL(ABS_TIME_INPUT, result.nextMode);
  TEST_ASSERT_TRUE(result.isValid);
  
  // 2. 絶対時刻入力画面でキャンセル
  ButtonEvent cLongPress(BUTTON_TYPE_C, LONG_PRESS);
  result = StateTransitionManager::handleStateTransition(ABS_TIME_INPUT, cLongPress, state);
  TEST_ASSERT_EQUAL(MAIN_DISPLAY, result.nextMode);
  TEST_ASSERT_TRUE(result.isValid);
  
  // 3. メイン画面から相対時刻加算入力画面へ
  ButtonEvent bShortPress(BUTTON_TYPE_B, SHORT_PRESS);
  result = StateTransitionManager::handleStateTransition(MAIN_DISPLAY, bShortPress, state);
  TEST_ASSERT_EQUAL(REL_PLUS_TIME_INPUT, result.nextMode);
  TEST_ASSERT_TRUE(result.isValid);
  
  // 4. 相対時刻加算入力画面でキャンセル
  result = StateTransitionManager::handleStateTransition(REL_PLUS_TIME_INPUT, cLongPress, state);
  TEST_ASSERT_EQUAL(MAIN_DISPLAY, result.nextMode);
  TEST_ASSERT_TRUE(result.isValid);
  
  // 5. メイン画面からアラーム管理画面へ
  ButtonEvent cShortPress(BUTTON_TYPE_C, SHORT_PRESS);
  result = StateTransitionManager::handleStateTransition(MAIN_DISPLAY, cShortPress, state);
  TEST_ASSERT_EQUAL(ALARM_MANAGEMENT, result.nextMode);
  TEST_ASSERT_TRUE(result.isValid);
  
  // 6. アラーム管理画面からメイン画面に戻る
  result = StateTransitionManager::handleStateTransition(ALARM_MANAGEMENT, cLongPress, state);
  TEST_ASSERT_EQUAL(MAIN_DISPLAY, result.nextMode);
  TEST_ASSERT_TRUE(result.isValid);
  
  // 7. メイン画面から設定メニューへ
  result = StateTransitionManager::handleStateTransition(MAIN_DISPLAY, cLongPress, state);
  TEST_ASSERT_EQUAL(SETTINGS_MENU, result.nextMode);
  TEST_ASSERT_TRUE(result.isValid);
}

// 設定メニューの操作シーケンステスト
void test_settings_menu_sequence() {
  SystemState state = getCurrentSystemState();
  
  // 1. メイン画面から設定メニューへ
  ButtonEvent cLongPress(BUTTON_TYPE_C, LONG_PRESS);
  TransitionResult result = StateTransitionManager::handleStateTransition(MAIN_DISPLAY, cLongPress, state);
  TEST_ASSERT_EQUAL(SETTINGS_MENU, result.nextMode);
  
  // 2. 設定メニューでWarning Color Testを選択
  state.settingsMenu.selectedItem = 2;
  ButtonEvent cShortPress(BUTTON_TYPE_C, SHORT_PRESS);
  result = StateTransitionManager::handleStateTransition(SETTINGS_MENU, cShortPress, state);
  TEST_ASSERT_EQUAL(WARNING_COLOR_TEST, result.nextMode);
  
  // 3. 警告色テスト画面から設定メニューに戻る
  result = StateTransitionManager::handleStateTransition(WARNING_COLOR_TEST, cShortPress, state);
  TEST_ASSERT_EQUAL(SETTINGS_MENU, result.nextMode);
  
  // 4. 設定メニューでInfoを選択
  state.settingsMenu.selectedItem = 4;
  result = StateTransitionManager::handleStateTransition(SETTINGS_MENU, cShortPress, state);
  TEST_ASSERT_EQUAL(INFO_DISPLAY, result.nextMode);
  
  // 5. 情報表示画面からメイン画面に戻る
  ButtonEvent anyEvent(BUTTON_TYPE_A, SHORT_PRESS);
  result = StateTransitionManager::handleStateTransition(INFO_DISPLAY, anyEvent, state);
  TEST_ASSERT_EQUAL(MAIN_DISPLAY, result.nextMode);
}

// アラーム管理の操作シーケンステスト
void test_alarm_management_sequence() {
  SystemState state = getCurrentSystemState();
  
  // 1. アラームが存在しない場合の動作
  ButtonEvent anyEvent(BUTTON_TYPE_A, SHORT_PRESS);
  TransitionResult result = StateTransitionManager::handleStateTransition(ALARM_MANAGEMENT, anyEvent, state);
  TEST_ASSERT_EQUAL(MAIN_DISPLAY, result.nextMode);
  
  // 2. アラームを追加
  alarmTimes.push_back(1000);
  state = getCurrentSystemState();
  
  // 3. アラーム管理画面での操作
  result = StateTransitionManager::handleStateTransition(ALARM_MANAGEMENT, anyEvent, state);
  TEST_ASSERT_EQUAL(MAIN_DISPLAY, result.nextMode); // 現在は同じ画面に留まる処理
}

// エラー状態での操作シーケンステスト
void test_error_state_sequence() {
  SystemState state = getCurrentSystemState();
  
  // 1. 警告表示中の操作
  mockWarningDisplayed = true;
  ButtonEvent anyEvent(BUTTON_TYPE_A, SHORT_PRESS);
  TransitionResult result = StateTransitionManager::handleStateTransition(MAIN_DISPLAY, anyEvent, state);
  TEST_ASSERT_FALSE(result.isValid);
  mockWarningDisplayed = false;
  
  // 2. アラーム鳴動中の操作
  state.alarmActive = true;
  result = StateTransitionManager::handleStateTransition(ALARM_ACTIVE, anyEvent, state);
  TEST_ASSERT_EQUAL(MAIN_DISPLAY, result.nextMode);
  TEST_ASSERT_EQUAL(ACTION_STOP_ALARM, result.action);
}

// 境界値テスト
void test_boundary_conditions() {
  SystemState state = getCurrentSystemState();
  
  // 1. 最大アラーム数のテスト
  for (int i = 0; i < 5; i++) {
    alarmTimes.push_back(1000 + i);
  }
  state = getCurrentSystemState();
  TEST_ASSERT_EQUAL(5, state.alarmCount);
  
  // 2. 選択インデックスの境界値テスト
  state.selectedIndex = 4; // 最大値
  TEST_ASSERT_TRUE(isValidSystemState(state));
  
  state.selectedIndex = 5; // 範囲外
  TEST_ASSERT_FALSE(isValidSystemState(state));
}

// 長押し操作のテスト
void test_long_press_operations() {
  SystemState state = getCurrentSystemState();
  
  // 1. メイン画面でのC長押し
  ButtonEvent cLongPress(BUTTON_TYPE_C, LONG_PRESS);
  TransitionResult result = StateTransitionManager::handleStateTransition(MAIN_DISPLAY, cLongPress, state);
  TEST_ASSERT_EQUAL(SETTINGS_MENU, result.nextMode);
  
  // 2. 入力モードでのC長押し（キャンセル）
  result = StateTransitionManager::handleStateTransition(ABS_TIME_INPUT, cLongPress, state);
  TEST_ASSERT_EQUAL(MAIN_DISPLAY, result.nextMode);
  
  // 3. アラーム管理でのC長押し（メイン画面に戻る）
  result = StateTransitionManager::handleStateTransition(ALARM_MANAGEMENT, cLongPress, state);
  TEST_ASSERT_EQUAL(MAIN_DISPLAY, result.nextMode);
}

// 状態の一貫性テスト
void test_state_consistency() {
  SystemState state = getCurrentSystemState();
  
  // 1. 初期状態の一貫性
  TEST_ASSERT_EQUAL(MAIN_DISPLAY, state.currentMode);
  TEST_ASSERT_EQUAL(0, state.selectedIndex);
  TEST_ASSERT_EQUAL(0, state.alarmCount);
  TEST_ASSERT_FALSE(state.warningDisplayed);
  TEST_ASSERT_FALSE(state.alarmActive);
  
  // 2. 状態変更後の一貫性
  alarmTimes.push_back(1000);
  state = getCurrentSystemState();
  TEST_ASSERT_EQUAL(1, state.alarmCount);
  
  // 3. 設定メニューの状態一貫性
  state.settingsMenu.selectedItem = 2;
  TEST_ASSERT_TRUE(isValidSystemState(state));
  
  state.settingsMenu.selectedItem = 10; // 範囲外
  TEST_ASSERT_FALSE(isValidSystemState(state));
}

int main() {
  UNITY_BEGIN();
  
  // 統合テスト
  RUN_TEST(test_typical_operation_sequence);
  RUN_TEST(test_settings_menu_sequence);
  RUN_TEST(test_alarm_management_sequence);
  RUN_TEST(test_error_state_sequence);
  RUN_TEST(test_boundary_conditions);
  RUN_TEST(test_long_press_operations);
  RUN_TEST(test_state_consistency);
  
  return UNITY_END();
} 