#include <unity.h>
#include <cstdio>
#include <cstring>
#include <ctime>
#include "time_logic.h"

// TimeLogicクラスの純粋ロジックテスト
// M5Stack依存を排除し、標準C++のみでテスト

// 時刻バリデーションテスト
void test_time_validation() {
  // 正常な時刻
  TEST_ASSERT_TRUE(TimeLogic::isValidTime(0, 0));
  TEST_ASSERT_TRUE(TimeLogic::isValidTime(12, 30));
  TEST_ASSERT_TRUE(TimeLogic::isValidTime(23, 59));
  
  // 無効な時刻
  TEST_ASSERT_FALSE(TimeLogic::isValidTime(-1, 0));
  TEST_ASSERT_FALSE(TimeLogic::isValidTime(24, 0));
  TEST_ASSERT_FALSE(TimeLogic::isValidTime(12, -1));
  TEST_ASSERT_FALSE(TimeLogic::isValidTime(12, 60));
  
  printf("✓ 時刻バリデーションテスト: 成功\n");
}

// 絶対時刻計算テスト
void test_absolute_time_calculation() {
  time_t now = time(NULL);
  struct tm* tm_info = localtime(&now);
  
  // 現在の日付で12:30の時刻を計算
  time_t calculated = TimeLogic::calculateAbsoluteTime(12, 30);
  struct tm* calc_tm = localtime(&calculated);
  
  TEST_ASSERT_EQUAL(12, calc_tm->tm_hour);
  TEST_ASSERT_EQUAL(30, calc_tm->tm_min);
  TEST_ASSERT_EQUAL(0, calc_tm->tm_sec);
  
  printf("✓ 絶対時刻計算テスト: 成功\n");
}

// 相対時刻計算テスト
void test_relative_time_calculation() {
  time_t now = time(NULL);
  struct tm* tm_info = localtime(&now);
  
  // 現在時刻から1時間30分後を計算
  time_t calculated = TimeLogic::calculateRelativeTime(tm_info->tm_hour, tm_info->tm_min, 1, 30, true);
  struct tm* calc_tm = localtime(&calculated);
  
  // 期待値: 現在時刻 + 1時間30分
  time_t expected = now + (1 * 3600 + 30 * 60);
  struct tm* exp_tm = localtime(&expected);
  
  TEST_ASSERT_EQUAL(exp_tm->tm_hour, calc_tm->tm_hour);
  TEST_ASSERT_EQUAL(exp_tm->tm_min, calc_tm->tm_min);
  
  printf("✓ 相対時刻計算テスト: 成功\n");
}

// 時刻フォーマットテスト
void test_time_formatting() {
  time_t test_time = time(NULL);
  struct tm* tm_info = localtime(&test_time);
  tm_info->tm_hour = 14;
  tm_info->tm_min = 25;
  test_time = mktime(tm_info);
  
  int hour, minute;
  TimeLogic::formatTime(test_time, hour, minute);
  
  TEST_ASSERT_EQUAL(14, hour);
  TEST_ASSERT_EQUAL(25, minute);
  
  char buffer[16];
  TimeLogic::formatTimeString(test_time, buffer, sizeof(buffer));
  TEST_ASSERT_EQUAL_STRING("14:25", buffer);
  
  printf("✓ 時刻フォーマットテスト: 成功\n");
}

void setUp(void) {
  // テスト前のセットアップ
}

void tearDown(void) {
  // テスト後のクリーンアップ
}

int main() {
  printf("=== TimeLogicクラステスト開始 ===\n");
  
  UNITY_BEGIN();
  
  RUN_TEST(test_time_validation);
  RUN_TEST(test_absolute_time_calculation);
  RUN_TEST(test_relative_time_calculation);
  RUN_TEST(test_time_formatting);
  
  UNITY_END();
  
  printf("=== TimeLogicクラステスト完了 ===\n");
  printf("全テストが完了しました！\n");
  
  return 0;
} 