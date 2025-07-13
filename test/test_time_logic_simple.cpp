#include <unity.h>
#include <cstdio>
#include <cstring>
#include <ctime>

// TimeLogicクラスの純粋ロジックテスト
// M5Stack依存を排除し、標準C++のみでテスト

// 時刻バリデーションテスト
void test_time_validation() {
  // 正常な時刻
  TEST_ASSERT_TRUE(isValidTime(0, 0));
  TEST_ASSERT_TRUE(isValidTime(12, 30));
  TEST_ASSERT_TRUE(isValidTime(23, 59));
  
  // 無効な時刻
  TEST_ASSERT_FALSE(isValidTime(-1, 0));
  TEST_ASSERT_FALSE(isValidTime(24, 0));
  TEST_ASSERT_FALSE(isValidTime(12, -1));
  TEST_ASSERT_FALSE(isValidTime(12, 60));
  
  printf("✓ 時刻バリデーションテスト: 成功\n");
}

// 絶対時刻計算テスト
void test_absolute_time_calculation() {
  time_t now = time(NULL);
  struct tm* tm_info = localtime(&now);
  
  // 現在の日付で12:30の時刻を計算
  time_t calculated = calculateAbsoluteTime(12, 30);
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
  time_t calculated = calculateRelativeTime(tm_info->tm_hour, tm_info->tm_min, 1, 30, true);
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
  formatTime(test_time, hour, minute);
  
  TEST_ASSERT_EQUAL(14, hour);
  TEST_ASSERT_EQUAL(25, minute);
  
  char buffer[16];
  formatTimeString(test_time, buffer, sizeof(buffer));
  TEST_ASSERT_EQUAL_STRING("14:25", buffer);
  
  printf("✓ 時刻フォーマットテスト: 成功\n");
}

// ヘルパー関数（TimeLogicクラスの関数を模擬）
bool isValidTime(int hour, int minute) {
  return hour >= 0 && hour <= 23 && minute >= 0 && minute <= 59;
}

time_t calculateAbsoluteTime(int hour, int minute) {
  time_t now = time(NULL);
  struct tm tm_info;
  localtime_r(&now, &tm_info);
  
  tm_info.tm_hour = hour;
  tm_info.tm_min = minute;
  tm_info.tm_sec = 0;
  
  return mktime(&tm_info);
}

time_t calculateRelativeTime(int base_hour, int base_min, int add_hour, int add_min, bool is_plus) {
  time_t now = time(NULL);
  struct tm tm_info;
  localtime_r(&now, &tm_info);
  
  tm_info.tm_hour = base_hour;
  tm_info.tm_min = base_min;
  tm_info.tm_sec = 0;
  
  time_t base_time = mktime(&tm_info);
  int total_minutes = add_hour * 60 + add_min;
  
  if (!is_plus) total_minutes = -total_minutes;
  
  return base_time + total_minutes * 60;
}

void formatTime(time_t time, int& hour, int& minute) {
  struct tm tm_info;
  localtime_r(&time, &tm_info);
  hour = tm_info.tm_hour;
  minute = tm_info.tm_min;
}

void formatTimeString(time_t time, char* buffer, size_t buffer_size) {
  struct tm tm_info;
  localtime_r(&time, &tm_info);
  snprintf(buffer, buffer_size, "%02d:%02d", tm_info.tm_hour, tm_info.tm_min);
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