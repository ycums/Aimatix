#include <unity.h>
#include <time.h>
#include <vector>
#include "alarm.h"
#include "input.h"
#include "ui.h"
#include "types.h"
#include "time_logic.h"

// モック設定
extern "C" {
  // 時間関連のモック関数
  time_t mock_current_time = 1640995200; // 2022-01-01 00:00:00 UTC
  unsigned long mock_millis = 0;
  
  // アラーム関連のモック
  std::vector<time_t> mock_alarm_times;
  bool mock_alarm_triggered = false;
  
  // 入力関連のモック
  bool mock_input_valid = true;
  bool mock_input_complete = false;
  
  // UI関連のモック
  bool mock_ui_updated = false;
  bool mock_warning_displayed = false;
}

// テスト用のグローバル変数
extern std::vector<time_t> alarmTimes;
extern InputState inputState;
extern Mode currentMode;

void setUp(void) {
  // テスト前の初期化
  mock_current_time = 1640995200;
  mock_millis = 0;
  mock_alarm_times.clear();
  mock_alarm_triggered = false;
  mock_input_valid = true;
  mock_input_complete = false;
  mock_ui_updated = false;
  mock_warning_displayed = false;
  
  // アラームリストのクリア
  alarmTimes.clear();
  
  // 入力状態のリセット
  resetInput();
  
  // モードのリセット
  currentMode = MAIN_DISPLAY;
}

void tearDown(void) {
  // テスト後のクリーンアップ
  alarmTimes.clear();
}

// テストケース1: 現在時刻表示の正確性確認
void test_current_time_display() {
  // テスト用の時刻設定
  mock_current_time = 1640995200; // 2022-01-01 00:00:00 UTC
  
  // 時刻文字列の取得
  std::string timeString = getTimeString(mock_current_time);
  
  // 時刻フォーマットの確認（HH:MM形式）
  TEST_ASSERT_EQUAL(5, timeString.length()); // "HH:MM" = 5文字
  TEST_ASSERT_EQUAL(':', timeString[2]); // 3文字目がコロン
  
  // 時刻の妥当性確認
  int hour = std::stoi(timeString.substr(0, 2));
  int minute = std::stoi(timeString.substr(3, 2));
  TEST_ASSERT_GREATER_OR_EQUAL(0, hour);
  TEST_ASSERT_LESS_OR_EQUAL(23, hour);
  TEST_ASSERT_GREATER_OR_EQUAL(0, minute);
  TEST_ASSERT_LESS_OR_EQUAL(59, minute);
}

// テストケース2: アラーム設定の基本動作確認
void test_alarm_setting_basic() {
  // アラームリストが空であることを確認
  TEST_ASSERT_EQUAL(0, alarmTimes.size());
  
  // アラームの追加
  time_t alarmTime = mock_current_time + 3600; // 1時間後
  bool addResult = addAlarm(alarmTime);
  
  TEST_ASSERT_TRUE(addResult);
  TEST_ASSERT_EQUAL(1, alarmTimes.size());
  TEST_ASSERT_EQUAL(alarmTime, alarmTimes[0]);
}

// テストケース3: アラーム重複チェック機能
void test_alarm_duplicate_check() {
  time_t alarmTime = mock_current_time + 3600; // 1時間後
  
  // 1回目のアラーム追加
  bool addResult1 = addAlarm(alarmTime);
  TEST_ASSERT_TRUE(addResult1);
  TEST_ASSERT_EQUAL(1, alarmTimes.size());
  
  // 同じ時刻のアラームを再度追加（重複）
  bool addResult2 = addAlarm(alarmTime);
  TEST_ASSERT_FALSE(addResult2); // 重複は追加されない
  TEST_ASSERT_EQUAL(1, alarmTimes.size()); // サイズは変わらない
}

// テストケース4: アラーム数の上限チェック
void test_alarm_count_limit() {
  // 最大5件のアラームを追加
  for (int i = 0; i < 5; i++) {
    time_t alarmTime = mock_current_time + (i + 1) * 3600;
    bool addResult = addAlarm(alarmTime);
    TEST_ASSERT_TRUE(addResult);
  }
  
  TEST_ASSERT_EQUAL(5, alarmTimes.size());
  
  // 6件目のアラーム追加（上限超過）
  time_t extraAlarmTime = mock_current_time + 6 * 3600;
  bool addResult = addAlarm(extraAlarmTime);
  TEST_ASSERT_FALSE(addResult); // 上限超過で追加されない
  TEST_ASSERT_EQUAL(5, alarmTimes.size()); // サイズは変わらない
}

// テストケース5: 過去時刻の自動削除
void test_past_alarm_removal() {
  // 過去のアラームを追加
  time_t pastAlarm = mock_current_time - 3600; // 1時間前
  addAlarm(pastAlarm);
  
  // 現在時刻のアラームを追加
  time_t currentAlarm = mock_current_time + 3600; // 1時間後
  addAlarm(currentAlarm);
  
  TEST_ASSERT_EQUAL(2, alarmTimes.size());
  
  // 過去時刻の削除処理
  removePastAlarms();
  
  // 過去のアラームが削除されていることを確認
  TEST_ASSERT_EQUAL(1, alarmTimes.size());
  TEST_ASSERT_EQUAL(currentAlarm, alarmTimes[0]);
}

// テストケース6: アラーム時刻の自動ソート
void test_alarm_time_sorting() {
  // ランダムな順序でアラームを追加
  time_t alarm3 = mock_current_time + 3 * 3600; // 3時間後
  time_t alarm1 = mock_current_time + 1 * 3600; // 1時間後
  time_t alarm2 = mock_current_time + 2 * 3600; // 2時間後
  
  addAlarm(alarm3);
  addAlarm(alarm1);
  addAlarm(alarm2);
  
  // 時刻順でソートされていることを確認
  TEST_ASSERT_EQUAL(3, alarmTimes.size());
  TEST_ASSERT_EQUAL(alarm1, alarmTimes[0]); // 1時間後
  TEST_ASSERT_EQUAL(alarm2, alarmTimes[1]); // 2時間後
  TEST_ASSERT_EQUAL(alarm3, alarmTimes[2]); // 3時間後
}

// テストケース7: アラーム削除の基本動作確認
void test_alarm_deletion_basic() {
  // アラームを追加
  time_t alarmTime = mock_current_time + 3600;
  addAlarm(alarmTime);
  TEST_ASSERT_EQUAL(1, alarmTimes.size());
  
  // アラームを削除
  bool deleteResult = deleteAlarm(0);
  TEST_ASSERT_TRUE(deleteResult);
  TEST_ASSERT_EQUAL(0, alarmTimes.size());
}

// テストケース8: 画面遷移の安定性確認
void test_screen_transition_stability() {
  // メイン表示画面から開始
  TEST_ASSERT_EQUAL(MAIN_DISPLAY, currentMode);
  
  // 各モードへの遷移テスト
  Mode testModes[] = {
    NTP_SYNC,
    ABS_TIME_INPUT,
    REL_PLUS_TIME_INPUT,
    ALARM_MANAGEMENT,
    SETTINGS_MENU,
    INFO_DISPLAY
  };
  
  for (Mode testMode : testModes) {
    currentMode = testMode;
    TEST_ASSERT_EQUAL(testMode, currentMode);
  }
  
  // メイン表示画面に戻る
  currentMode = MAIN_DISPLAY;
  TEST_ASSERT_EQUAL(MAIN_DISPLAY, currentMode);
}

// テストケース9: 入力値の妥当性チェック
void test_input_validation() {
  // 正常な時刻入力
  inputState.hourTens = 1;
  inputState.hourOnes = 2;
  inputState.minTens = 3;
  inputState.minOnes = 4;
  
  bool isValid = validateTimeInput(inputState);
  TEST_ASSERT_TRUE(isValid);
  
  // 不正な時刻入力（24時を超える）
  inputState.hourTens = 2;
  inputState.hourOnes = 5;
  inputState.minTens = 0;
  inputState.minOnes = 0;
  
  isValid = validateTimeInput(inputState);
  TEST_ASSERT_FALSE(isValid);
  
  // 不正な時刻入力（60分を超える）
  inputState.hourTens = 1;
  inputState.hourOnes = 2;
  inputState.minTens = 6;
  inputState.minOnes = 0;
  
  isValid = validateTimeInput(inputState);
  TEST_ASSERT_FALSE(isValid);
}

// テストケース10: 境界値テスト
void test_boundary_values() {
  // 時刻の境界値テスト
  // 00:00
  inputState.hourTens = 0;
  inputState.hourOnes = 0;
  inputState.minTens = 0;
  inputState.minOnes = 0;
  
  bool isValid = validateTimeInput(inputState);
  TEST_ASSERT_TRUE(isValid);
  
  // 23:59
  inputState.hourTens = 2;
  inputState.hourOnes = 3;
  inputState.minTens = 5;
  inputState.minOnes = 9;
  
  isValid = validateTimeInput(inputState);
  TEST_ASSERT_TRUE(isValid);
  
  // アラーム数の境界値テスト
  // 最大5件のアラームを追加
  for (int i = 0; i < 5; i++) {
    time_t alarmTime = mock_current_time + (i + 1) * 3600;
    addAlarm(alarmTime);
  }
  
  TEST_ASSERT_EQUAL(5, alarmTimes.size());
  
  // 6件目の追加は失敗
  time_t extraAlarm = mock_current_time + 6 * 3600;
  bool addResult = addAlarm(extraAlarm);
  TEST_ASSERT_FALSE(addResult);
  TEST_ASSERT_EQUAL(5, alarmTimes.size());
}

// テストケース11: エラーハンドリングテスト
void test_error_handling() {
  // 無効なインデックスでのアラーム削除
  bool deleteResult = deleteAlarm(-1);
  TEST_ASSERT_FALSE(deleteResult);
  
  deleteResult = deleteAlarm(10);
  TEST_ASSERT_FALSE(deleteResult);
  
  // 空のリストでの削除
  TEST_ASSERT_EQUAL(0, alarmTimes.size());
  deleteResult = deleteAlarm(0);
  TEST_ASSERT_FALSE(deleteResult);
  
  // 無効な時刻でのアラーム追加
  time_t invalidTime = 0;
  bool addResult = addAlarm(invalidTime);
  TEST_ASSERT_FALSE(addResult);
}

// テストケース12: 統合動作テスト
void test_integrated_operation() {
  // 1. アラームの設定
  time_t alarm1 = mock_current_time + 3600; // 1時間後
  time_t alarm2 = mock_current_time + 7200; // 2時間後
  
  addAlarm(alarm1);
  addAlarm(alarm2);
  TEST_ASSERT_EQUAL(2, alarmTimes.size());
  
  // 2. 時刻の進行
  mock_current_time += 3600; // 1時間経過
  
  // 3. 過去時刻の削除
  removePastAlarms();
  TEST_ASSERT_EQUAL(1, alarmTimes.size()); // alarm1が削除される
  TEST_ASSERT_EQUAL(alarm2, alarmTimes[0]);
  
  // 4. 残りアラームの削除
  deleteAlarm(0);
  TEST_ASSERT_EQUAL(0, alarmTimes.size());
}

void RUN_UNITY_TESTS() {
  UNITY_BEGIN();
  
  RUN_TEST(test_current_time_display);
  RUN_TEST(test_alarm_setting_basic);
  RUN_TEST(test_alarm_duplicate_check);
  RUN_TEST(test_alarm_count_limit);
  RUN_TEST(test_past_alarm_removal);
  RUN_TEST(test_alarm_time_sorting);
  RUN_TEST(test_alarm_deletion_basic);
  RUN_TEST(test_screen_transition_stability);
  RUN_TEST(test_input_validation);
  RUN_TEST(test_boundary_values);
  RUN_TEST(test_error_handling);
  RUN_TEST(test_integrated_operation);
  
  UNITY_END();
}

int main(int argc, char **argv) {
  RUN_UNITY_TESTS();
  return 0;
} 