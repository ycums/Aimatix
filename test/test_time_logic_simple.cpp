#include <unity.h>
#include <cstdio>
#include <cstring>
#include <ctime>
#include "time_logic.h"

// TimeLogicクラスの純粋ロジックテスト
// M5Stack依存を排除し、標準C++のみでテスト

void setUp(void) {}
void tearDown(void) {}

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
  struct tm* calc_info = localtime(&calculated);
  
  TEST_ASSERT_EQUAL(12, calc_info->tm_hour);
  TEST_ASSERT_EQUAL(30, calc_info->tm_min);
  
  printf("✓ 絶対時刻計算テスト: 成功\n");
}

// 相対時刻計算テスト
void test_relative_time_calculation() {
  time_t now = time(NULL);
  struct tm* tm_info = localtime(&now);
  
  // 現在時刻から1時間30分後を計算
  time_t calculated = TimeLogic::calculateRelativeTime(tm_info->tm_hour, tm_info->tm_min, 1, 30, true);
  struct tm* calc_info = localtime(&calculated);
  
  int expected_hour = (tm_info->tm_hour + 1) % 24;
  int expected_min = tm_info->tm_min + 30;
  if (expected_min >= 60) {
    expected_min -= 60;
    expected_hour = (expected_hour + 1) % 24;
  }
  
  TEST_ASSERT_EQUAL(expected_hour, calc_info->tm_hour);
  TEST_ASSERT_EQUAL(expected_min, calc_info->tm_min);
  
  printf("✓ 相対時刻計算テスト: 成功\n");
}

// 時刻フォーマットテスト
void test_time_formatting() {
  time_t test_time = 1000000000; // 2001年9月9日 01:46:40 UTC
  int hour, minute;
  
  TimeLogic::formatTime(test_time, hour, minute);
  TEST_ASSERT_EQUAL(1, hour);
  TEST_ASSERT_EQUAL(46, minute);
  
  char buffer[16];
  TimeLogic::formatTimeString(test_time, buffer, sizeof(buffer));
  TEST_ASSERT_EQUAL_STRING("01:46", buffer);
  
  printf("✓ 時刻フォーマットテスト: 成功\n");
}

// アラーム時刻計算テスト
void test_alarm_time_calculation() {
  time_t now = time(NULL);
  
  // 絶対時刻入力モード
  time_t absolute = TimeLogic::calculateAlarmTime(14, 30, 0, now);
  struct tm* abs_info = localtime(&absolute);
  TEST_ASSERT_EQUAL(14, abs_info->tm_hour);
  TEST_ASSERT_EQUAL(30, abs_info->tm_min);
  
  // 相対時刻入力モード
  time_t relative = TimeLogic::calculateAlarmTime(1, 30, 1, now);
  TEST_ASSERT_GREATER_THAN(relative, now);
  
  printf("✓ アラーム時刻計算テスト: 成功\n");
}

// メイン関数
int main() {
  UNITY_BEGIN();
  
  printf("=== TimeLogic Unit Test ===\n");
  
  RUN_TEST(test_time_validation);
  RUN_TEST(test_absolute_time_calculation);
  RUN_TEST(test_relative_time_calculation);
  RUN_TEST(test_time_formatting);
  RUN_TEST(test_alarm_time_calculation);
  
  printf("=== 全テスト完了 ===\n");
  
  return UNITY_END();
} 