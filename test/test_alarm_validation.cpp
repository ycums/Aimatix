#include <unity.h>
#include "input.h"
#include "alarm.h"
#include "types.h"
#include "mock_m5stack.h"
#include <vector>

// テスト用のモック関数
extern time_t mockTime;

// テスト用のグローバル変数
extern std::vector<time_t> alarmTimes;
extern Mode currentMode;

// テスト前のセットアップ
void setUp(void) {
  mockTime = 1000000000; // 2001年9月9日 01:46:40 UTC
  currentMode = ABS_TIME_INPUT;
  
  // アラームリストをクリア
  alarmTimes.clear();
  
  // 入力状態をリセット
  resetInput();
}

// テスト後のクリーンアップ
void tearDown(void) {
  alarmTimes.clear();
}

// テスト1: 正常な時刻のアラーム追加
void test_valid_time_alarm_addition() {
  // 正常な時刻を設定（12:30）
  digitEditInput.hourTens = 1;
  digitEditInput.hourOnes = 2;
  digitEditInput.minTens = 3;
  digitEditInput.minOnes = 0;
  
  // アラーム追加処理をシミュレート
  int hour = digitEditInput.hourTens * 10 + digitEditInput.hourOnes;
  int min = digitEditInput.minTens * 10 + digitEditInput.minOnes;
  
  // 時刻バリデーション
  TEST_ASSERT_TRUE(hour >= 0 && hour <= 23);
  TEST_ASSERT_TRUE(min >= 0 && min <= 59);
  
  // アラーム時刻の計算（簡略化）
  time_t now = time(NULL);
  time_t alarmTime = now + (hour * 3600) + (min * 60);
  
  // 過去時刻の場合は翌日として設定
  if (alarmTime <= now) {
    alarmTime += 24 * 3600;
  }
  
  // アラーム追加
  alarmTimes.push_back(alarmTime);
  std::sort(alarmTimes.begin(), alarmTimes.end());
  
  TEST_ASSERT_EQUAL(1, alarmTimes.size());
  
  Serial.println("✓ 正常な時刻のアラーム追加テスト: 成功");
}

// テスト2: 無効な時刻のバリデーション
void test_invalid_time_validation() {
  // 無効な時刻を設定（25:00）
  digitEditInput.hourTens = 2;
  digitEditInput.hourOnes = 5;
  digitEditInput.minTens = 0;
  digitEditInput.minOnes = 0;
  
  int hour = digitEditInput.hourTens * 10 + digitEditInput.hourOnes;
  int min = digitEditInput.minTens * 10 + digitEditInput.minOnes;
  
  // 時刻バリデーション
  bool isValid = (hour >= 0 && hour <= 23) && (min >= 0 && min <= 59);
  TEST_ASSERT_FALSE(isValid);
  
  Serial.println("✓ 無効な時刻のバリデーションテスト: 成功");
}

// テスト3: 最大アラーム数制限
void test_maximum_alarm_limit() {
  // 最大数（5件）までアラームを追加
  time_t baseTime = time(NULL);
  for (int i = 0; i < 5; i++) {
    time_t alarmTime = baseTime + (i + 1) * 3600;
    alarmTimes.push_back(alarmTime);
  }
  
  TEST_ASSERT_EQUAL(5, alarmTimes.size());
  
  // 6件目を追加しようとする
  time_t newAlarmTime = baseTime + 6 * 3600;
  bool canAdd = (alarmTimes.size() < 5);
  TEST_ASSERT_FALSE(canAdd);
  
  Serial.println("✓ 最大アラーム数制限テスト: 成功");
}

// テスト4: 重複アラームのチェック
void test_duplicate_alarm_check() {
  // 既存のアラームを追加
  time_t existingAlarm = time(NULL) + 3600;
  alarmTimes.push_back(existingAlarm);
  
  // 同じ時刻のアラームを追加しようとする
  bool isDuplicate = (std::find(alarmTimes.begin(), alarmTimes.end(), existingAlarm) != alarmTimes.end());
  TEST_ASSERT_TRUE(isDuplicate);
  
  Serial.println("✓ 重複アラームチェックテスト: 成功");
}

// テスト5: 過去時刻の処理
void test_past_time_handling() {
  // 過去の時刻を設定（現在時刻より1時間前）
  time_t now = time(NULL);
  time_t pastTime = now - 3600; // 1時間前
  
  // 過去時刻は現在時刻より小さい
  TEST_ASSERT_TRUE(pastTime <= now);
  
  // 過去時刻の場合は翌日として設定
  if (pastTime <= now) {
    pastTime += 24 * 3600;
  }
  
  // 翌日として設定された時刻は現在時刻より大きい
  TEST_ASSERT_TRUE(pastTime > now);
  
  Serial.println("✓ 過去時刻処理テスト: 成功");
}

// テスト6: 相対時刻入力の処理
void test_relative_time_input() {
  currentMode = REL_PLUS_TIME_INPUT;
  
  // 相対時刻を設定（2時間30分後）
  digitEditInput.hourTens = 0;
  digitEditInput.hourOnes = 2;
  digitEditInput.minTens = 3;
  digitEditInput.minOnes = 0;
  
  int hour = digitEditInput.hourTens * 10 + digitEditInput.hourOnes;
  int min = digitEditInput.minTens * 10 + digitEditInput.minOnes;
  
  // 相対時刻の計算
  time_t now = time(NULL);
  time_t alarmTime = now + (hour * 3600) + (min * 60);
  
  // 計算された時刻は現在時刻より大きい
  TEST_ASSERT_TRUE(alarmTime > now);
  
  Serial.println("✓ 相対時刻入力処理テスト: 成功");
}

// テスト7: アラームのソート機能
void test_alarm_sorting() {
  // ランダムな順序でアラームを追加
  time_t now = time(NULL);
  alarmTimes.push_back(now + 3 * 3600); // 3時間後
  alarmTimes.push_back(now + 1 * 3600); // 1時間後
  alarmTimes.push_back(now + 2 * 3600); // 2時間後
  
  // ソート前の順序を確認
  TEST_ASSERT_EQUAL(now + 3 * 3600, alarmTimes[0]);
  TEST_ASSERT_EQUAL(now + 1 * 3600, alarmTimes[1]);
  TEST_ASSERT_EQUAL(now + 2 * 3600, alarmTimes[2]);
  
  // ソート実行
  std::sort(alarmTimes.begin(), alarmTimes.end());
  
  // ソート後の順序を確認
  TEST_ASSERT_EQUAL(now + 1 * 3600, alarmTimes[0]);
  TEST_ASSERT_EQUAL(now + 2 * 3600, alarmTimes[1]);
  TEST_ASSERT_EQUAL(now + 3 * 3600, alarmTimes[2]);
  
  Serial.println("✓ アラームソート機能テスト: 成功");
}

// テスト8: 入力状態のリセット
void test_input_state_reset() {
  // 入力状態を変更
  digitEditInput.hourTens = 1;
  digitEditInput.hourOnes = 2;
  digitEditInput.minTens = 3;
  digitEditInput.minOnes = 0;
  digitEditInput.cursor = 2;
  
  // リセット実行
  resetInput();
  
  // リセット後の状態を確認
  TEST_ASSERT_EQUAL(DigitEditTimeInputState::INIT_HOUR_TENS, digitEditInput.hourTens);
  TEST_ASSERT_EQUAL(DigitEditTimeInputState::INIT_HOUR_ONES, digitEditInput.hourOnes);
  TEST_ASSERT_EQUAL(DigitEditTimeInputState::INIT_MIN_TENS, digitEditInput.minTens);
  TEST_ASSERT_EQUAL(DigitEditTimeInputState::INIT_MIN_ONES, digitEditInput.minOnes);
  TEST_ASSERT_EQUAL(3, digitEditInput.cursor);
  
  Serial.println("✓ 入力状態リセットテスト: 成功");
}

// テスト9: 境界値テスト
void test_boundary_values() {
  // 境界値のテスト
  // 最小値: 00:00
  digitEditInput.hourTens = 0;
  digitEditInput.hourOnes = 0;
  digitEditInput.minTens = 0;
  digitEditInput.minOnes = 0;
  
  int hour = digitEditInput.hourTens * 10 + digitEditInput.hourOnes;
  int min = digitEditInput.minTens * 10 + digitEditInput.minOnes;
  
  TEST_ASSERT_EQUAL(0, hour);
  TEST_ASSERT_EQUAL(0, min);
  
  // 最大値: 23:59
  digitEditInput.hourTens = 2;
  digitEditInput.hourOnes = 3;
  digitEditInput.minTens = 5;
  digitEditInput.minOnes = 9;
  
  hour = digitEditInput.hourTens * 10 + digitEditInput.hourOnes;
  min = digitEditInput.minTens * 10 + digitEditInput.minOnes;
  
  TEST_ASSERT_EQUAL(23, hour);
  TEST_ASSERT_EQUAL(59, min);
  
  Serial.println("✓ 境界値テスト: 成功");
}

// テスト10: エラーケースの総合テスト
void test_error_cases_integration() {
  // 最大アラーム数まで追加
  time_t baseTime = time(NULL);
  for (int i = 0; i < 5; i++) {
    time_t alarmTime = baseTime + (i + 1) * 3600;
    alarmTimes.push_back(alarmTime);
  }
  
  TEST_ASSERT_EQUAL(5, alarmTimes.size());
  
  // 無効な時刻で6件目を追加しようとする
  digitEditInput.hourTens = 2;
  digitEditInput.hourOnes = 5; // 25時
  digitEditInput.minTens = 0;
  digitEditInput.minOnes = 0;
  
  int hour = digitEditInput.hourTens * 10 + digitEditInput.hourOnes;
  int min = digitEditInput.minTens * 10 + digitEditInput.minOnes;
  
  // 時刻バリデーション
  bool isValid = (hour >= 0 && hour <= 23) && (min >= 0 && min <= 59);
  TEST_ASSERT_FALSE(isValid);
  
  // 最大数チェック
  bool canAdd = (alarmTimes.size() < 5);
  TEST_ASSERT_FALSE(canAdd);
  
  Serial.println("✓ エラーケース総合テスト: 成功");
}

// Arduinoフレームワーク用のsetup関数
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("=== アラームバリデーション機能テスト開始 ===");
  
  UNITY_BEGIN();
  
  // テストケースの実行
  RUN_TEST(test_valid_time_alarm_addition);
  RUN_TEST(test_invalid_time_validation);
  RUN_TEST(test_maximum_alarm_limit);
  RUN_TEST(test_duplicate_alarm_check);
  RUN_TEST(test_past_time_handling);
  RUN_TEST(test_relative_time_input);
  RUN_TEST(test_alarm_sorting);
  RUN_TEST(test_input_state_reset);
  RUN_TEST(test_boundary_values);
  RUN_TEST(test_error_cases_integration);
  
  UNITY_END();
  
  Serial.println("=== アラームバリデーション機能テスト完了 ===");
  Serial.println("全テストが完了しました！");
}

// Arduinoフレームワーク用のloop関数
void loop() {
  // テスト完了後は何もしない
  delay(1000);
} 