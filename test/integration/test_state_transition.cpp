#include <unity.h>
#include "../src/state_transition/button_event.h"
#include "../src/state_transition/system_state.h"
#include "../src/state_transition/transition_result.h"
#include "../src/state_transition/transition_validator.h"
#include "../src/state_transition/state_transition.h"

// テスト用のモック関数
extern "C" {
  // M5Stackのモック関数
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
  // テスト前の初期化
  currentMode = MAIN_DISPLAY;
  settingsMenu.selectedItem = 0;
  settingsMenu.itemCount = 5;
  inputState = InputState();
  alarmTimes.clear();
  scheduleSelectedIndex = 0;
  mockWarningDisplayed = false;
}

void tearDown(void) {
  // テスト後のクリーンアップ
}

// ButtonEvent型のテスト
void test_button_event_creation() {
  ButtonEvent event(BUTTON_TYPE_A, SHORT_PRESS);
  TEST_ASSERT_EQUAL(BUTTON_TYPE_A, event.button);
  TEST_ASSERT_EQUAL(SHORT_PRESS, event.action);
  TEST_ASSERT_EQUAL(1000, event.timestamp);
}

void test_button_event_validity() {
  ButtonEvent validEvent(BUTTON_TYPE_A, SHORT_PRESS);
  TEST_ASSERT_TRUE(isValidButtonEvent(validEvent));
  
  ButtonEvent invalidEvent;
  invalidEvent.button = static_cast<ButtonType>(10); // 無効な値
  TEST_ASSERT_FALSE(isValidButtonEvent(invalidEvent));
}

// SystemState型のテスト
void test_system_state_creation() {
  SystemState state = getCurrentSystemState();
  TEST_ASSERT_EQUAL(MAIN_DISPLAY, state.currentMode);
  TEST_ASSERT_EQUAL(0, state.selectedIndex);
  TEST_ASSERT_EQUAL(0, state.alarmCount);
  TEST_ASSERT_FALSE(state.warningDisplayed);
  TEST_ASSERT_FALSE(state.alarmActive);
}

void test_system_state_validity() {
  SystemState validState;
  validState.currentMode = MAIN_DISPLAY;
  validState.selectedIndex = 0;
  validState.alarmCount = 0;
  TEST_ASSERT_TRUE(isValidSystemState(validState));
  
  SystemState invalidState;
  invalidState.currentMode = static_cast<Mode>(100); // 無効な値
  TEST_ASSERT_FALSE(isValidSystemState(invalidState));
}

// TransitionResult型のテスト
void test_transition_result_creation() {
  TransitionResult validResult = createValidTransition(ABS_TIME_INPUT, ACTION_RESET_INPUT);
  TEST_ASSERT_EQUAL(ABS_TIME_INPUT, validResult.nextMode);
  TEST_ASSERT_TRUE(validResult.isValid);
  TEST_ASSERT_EQUAL(ACTION_RESET_INPUT, validResult.action);
  
  TransitionResult invalidResult = createInvalidTransition("テストエラー");
  TEST_ASSERT_FALSE(invalidResult.isValid);
  TEST_ASSERT_EQUAL_STRING("テストエラー", invalidResult.errorMessage);
}

// TransitionValidatorのテスト
void test_transition_validator_basic() {
  SystemState state = getCurrentSystemState();
  ButtonEvent event(BUTTON_TYPE_A, SHORT_PRESS);
  
  TEST_ASSERT_TRUE(TransitionValidator::isValidTransition(MAIN_DISPLAY, event, state));
}

void test_transition_validator_warning_blocking() {
  SystemState state = getCurrentSystemState();
  ButtonEvent event(BUTTON_TYPE_A, SHORT_PRESS);
  
  // 警告表示中は遷移をブロック
  mockWarningDisplayed = true;
  TEST_ASSERT_FALSE(TransitionValidator::isValidTransition(MAIN_DISPLAY, event, state));
  mockWarningDisplayed = false;
}

void test_transition_validator_alarm_blocking() {
  SystemState state = getCurrentSystemState();
  state.alarmActive = true;
  ButtonEvent event(BUTTON_TYPE_A, SHORT_PRESS);
  
  // アラーム鳴動中は任意ボタンで停止のみ許可
  TEST_ASSERT_TRUE(TransitionValidator::isValidTransition(ALARM_ACTIVE, event, state));
}

// StateTransitionManagerのテスト
void test_main_display_transitions() {
  SystemState state = getCurrentSystemState();
  
  // A短押し: 絶対時刻入力画面へ
  ButtonEvent aShortPress(BUTTON_TYPE_A, SHORT_PRESS);
  TransitionResult result = StateTransitionManager::handleStateTransition(MAIN_DISPLAY, aShortPress, state);
  TEST_ASSERT_EQUAL(ABS_TIME_INPUT, result.nextMode);
  TEST_ASSERT_TRUE(result.isValid);
  TEST_ASSERT_EQUAL(ACTION_RESET_INPUT, result.action);
  
  // B短押し: 相対時刻加算入力画面へ
  ButtonEvent bShortPress(BUTTON_TYPE_B, SHORT_PRESS);
  result = StateTransitionManager::handleStateTransition(MAIN_DISPLAY, bShortPress, state);
  TEST_ASSERT_EQUAL(REL_PLUS_TIME_INPUT, result.nextMode);
  TEST_ASSERT_TRUE(result.isValid);
  TEST_ASSERT_EQUAL(ACTION_RESET_INPUT, result.action);
  
  // C短押し: アラーム管理画面へ
  ButtonEvent cShortPress(BUTTON_TYPE_C, SHORT_PRESS);
  result = StateTransitionManager::handleStateTransition(MAIN_DISPLAY, cShortPress, state);
  TEST_ASSERT_EQUAL(ALARM_MANAGEMENT, result.nextMode);
  TEST_ASSERT_TRUE(result.isValid);
  
  // C長押し: 設定メニューへ
  ButtonEvent cLongPress(BUTTON_TYPE_C, LONG_PRESS);
  result = StateTransitionManager::handleStateTransition(MAIN_DISPLAY, cLongPress, state);
  TEST_ASSERT_EQUAL(SETTINGS_MENU, result.nextMode);
  TEST_ASSERT_TRUE(result.isValid);
}

void test_input_mode_transitions() {
  SystemState state = getCurrentSystemState();
  
  // 絶対時刻入力モードでのC短押し: メイン画面に戻る
  ButtonEvent cShortPress(BUTTON_TYPE_C, SHORT_PRESS);
  TransitionResult result = StateTransitionManager::handleStateTransition(ABS_TIME_INPUT, cShortPress, state);
  TEST_ASSERT_EQUAL(MAIN_DISPLAY, result.nextMode);
  TEST_ASSERT_TRUE(result.isValid);
  TEST_ASSERT_EQUAL(ACTION_ADD_ALARM, result.action);
  
  // 絶対時刻入力モードでのC長押し: キャンセル
  ButtonEvent cLongPress(BUTTON_TYPE_C, LONG_PRESS);
  result = StateTransitionManager::handleStateTransition(ABS_TIME_INPUT, cLongPress, state);
  TEST_ASSERT_EQUAL(MAIN_DISPLAY, result.nextMode);
  TEST_ASSERT_TRUE(result.isValid);
  TEST_ASSERT_EQUAL(ACTION_NONE, result.action);
}

void test_alarm_management_transitions() {
  SystemState state = getCurrentSystemState();
  
  // アラームが存在しない場合: メイン画面に戻る
  ButtonEvent anyEvent(BUTTON_TYPE_A, SHORT_PRESS);
  TransitionResult result = StateTransitionManager::handleStateTransition(ALARM_MANAGEMENT, anyEvent, state);
  TEST_ASSERT_EQUAL(MAIN_DISPLAY, result.nextMode);
  TEST_ASSERT_TRUE(result.isValid);
  
  // アラームが存在する場合: 同じ画面に留まる
  alarmTimes.push_back(1000); // アラームを追加
  state = getCurrentSystemState();
  result = StateTransitionManager::handleStateTransition(ALARM_MANAGEMENT, anyEvent, state);
  TEST_ASSERT_EQUAL(MAIN_DISPLAY, result.nextMode); // 現在は同じ画面に留まる処理
  TEST_ASSERT_TRUE(result.isValid);
}

void test_settings_menu_transitions() {
  SystemState state = getCurrentSystemState();
  
  // Warning Color Test選択時のC短押し: 警告色テスト画面へ
  state.settingsMenu.selectedItem = 2;
  ButtonEvent cShortPress(BUTTON_TYPE_C, SHORT_PRESS);
  TransitionResult result = StateTransitionManager::handleStateTransition(SETTINGS_MENU, cShortPress, state);
  TEST_ASSERT_EQUAL(WARNING_COLOR_TEST, result.nextMode);
  TEST_ASSERT_TRUE(result.isValid);
  
  // Info選択時のC短押し: 情報表示画面へ
  state.settingsMenu.selectedItem = 4;
  result = StateTransitionManager::handleStateTransition(SETTINGS_MENU, cShortPress, state);
  TEST_ASSERT_EQUAL(INFO_DISPLAY, result.nextMode);
  TEST_ASSERT_TRUE(result.isValid);
}

void test_alarm_active_transitions() {
  SystemState state = getCurrentSystemState();
  state.alarmActive = true;
  
  // 任意ボタンでアラーム停止
  ButtonEvent anyEvent(BUTTON_TYPE_A, SHORT_PRESS);
  TransitionResult result = StateTransitionManager::handleStateTransition(ALARM_ACTIVE, anyEvent, state);
  TEST_ASSERT_EQUAL(MAIN_DISPLAY, result.nextMode);
  TEST_ASSERT_TRUE(result.isValid);
  TEST_ASSERT_EQUAL(ACTION_STOP_ALARM, result.action);
}

// エッジケースのテスト
void test_edge_cases() {
  SystemState state = getCurrentSystemState();
  
  // 無効なボタンイベント
  ButtonEvent invalidEvent;
  invalidEvent.button = static_cast<ButtonType>(10);
  TransitionResult result = StateTransitionManager::handleStateTransition(MAIN_DISPLAY, invalidEvent, state);
  TEST_ASSERT_FALSE(result.isValid);
  
  // 警告表示中の遷移
  mockWarningDisplayed = true;
  ButtonEvent validEvent(BUTTON_TYPE_A, SHORT_PRESS);
  result = StateTransitionManager::handleStateTransition(MAIN_DISPLAY, validEvent, state);
  TEST_ASSERT_FALSE(result.isValid);
  mockWarningDisplayed = false;
}

int main() {
  UNITY_BEGIN();
  
  // ButtonEvent型のテスト
  RUN_TEST(test_button_event_creation);
  RUN_TEST(test_button_event_validity);
  
  // SystemState型のテスト
  RUN_TEST(test_system_state_creation);
  RUN_TEST(test_system_state_validity);
  
  // TransitionResult型のテスト
  RUN_TEST(test_transition_result_creation);
  
  // TransitionValidatorのテスト
  RUN_TEST(test_transition_validator_basic);
  RUN_TEST(test_transition_validator_warning_blocking);
  RUN_TEST(test_transition_validator_alarm_blocking);
  
  // StateTransitionManagerのテスト
  RUN_TEST(test_main_display_transitions);
  RUN_TEST(test_input_mode_transitions);
  RUN_TEST(test_alarm_management_transitions);
  RUN_TEST(test_settings_menu_transitions);
  RUN_TEST(test_alarm_active_transitions);
  
  // エッジケースのテスト
  RUN_TEST(test_edge_cases);
  
  return UNITY_END();
} 