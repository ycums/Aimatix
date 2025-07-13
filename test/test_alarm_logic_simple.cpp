#include <unity.h>
#include <cstdio>
#include <vector>
#include <ctime>
#include "time_logic.h"

// AlarmLogicクラスの純粋ロジックテスト
// M5Stack依存を排除し、標準C++のみでテスト

void setUp(void) {}
void tearDown(void) {}

// アラーム追加テスト
void test_alarm_addition() {
  std::vector<time_t> alarms;
  time_t alarm1 = 1000000000; // 2001年9月9日 01:46:40 UTC
  time_t alarm2 = 1000003600; // 2001年9月9日 02:46:40 UTC
  
  // 正常な追加
  TEST_ASSERT_TRUE(AlarmLogic::addAlarm(alarms, alarm1));
  TEST_ASSERT_EQUAL(1, alarms.size());
  
  TEST_ASSERT_TRUE(AlarmLogic::addAlarm(alarms, alarm2));
  TEST_ASSERT_EQUAL(2, alarms.size());
  
  // 重複追加の拒否
  TEST_ASSERT_FALSE(AlarmLogic::addAlarm(alarms, alarm1));
  TEST_ASSERT_EQUAL(2, alarms.size());
  
  printf("✓ アラーム追加テスト: 成功\n");
}

// 最大アラーム数制限テスト
void test_maximum_alarm_limit() {
  std::vector<time_t> alarms;
  time_t base_time = 1000000000;
  
  // 5個まで追加可能
  for (int i = 0; i < 5; i++) {
    TEST_ASSERT_TRUE(AlarmLogic::addAlarm(alarms, base_time + i * 3600));
  }
  TEST_ASSERT_EQUAL(5, alarms.size());
  
  // 6個目は拒否
  TEST_ASSERT_FALSE(AlarmLogic::addAlarm(alarms, base_time + 18000));
  TEST_ASSERT_EQUAL(5, alarms.size());
  
  printf("✓ 最大アラーム数制限テスト: 成功\n");
}

// アラーム削除テスト
void test_alarm_removal() {
  std::vector<time_t> alarms;
  time_t alarm1 = 1000000000;
  time_t alarm2 = 1000003600;
  
  AlarmLogic::addAlarm(alarms, alarm1);
  AlarmLogic::addAlarm(alarms, alarm2);
  TEST_ASSERT_EQUAL(2, alarms.size());
  
  // 存在するアラームの削除
  TEST_ASSERT_TRUE(AlarmLogic::removeAlarm(alarms, alarm1));
  TEST_ASSERT_EQUAL(1, alarms.size());
  
  // 存在しないアラームの削除
  TEST_ASSERT_FALSE(AlarmLogic::removeAlarm(alarms, alarm1));
  TEST_ASSERT_EQUAL(1, alarms.size());
  
  printf("✓ アラーム削除テスト: 成功\n");
}

// 重複チェックテスト
void test_duplicate_check() {
  std::vector<time_t> alarms;
  time_t alarm1 = 1000000000;
  
  // 空のリストでは重複なし
  TEST_ASSERT_FALSE(AlarmLogic::isDuplicateAlarm(alarms, alarm1));
  
  // アラーム追加後は重複あり
  AlarmLogic::addAlarm(alarms, alarm1);
  TEST_ASSERT_TRUE(AlarmLogic::isDuplicateAlarm(alarms, alarm1));
  
  printf("✓ 重複チェックテスト: 成功\n");
}

// ソートテスト
void test_alarm_sorting() {
  std::vector<time_t> alarms;
  time_t alarm1 = 1000003600; // 2番目
  time_t alarm2 = 1000000000; // 1番目
  time_t alarm3 = 1000007200; // 3番目
  
  // 順序を変えて追加
  AlarmLogic::addAlarm(alarms, alarm1);
  AlarmLogic::addAlarm(alarms, alarm2);
  AlarmLogic::addAlarm(alarms, alarm3);
  
  // 自動ソートされていることを確認
  TEST_ASSERT_EQUAL(alarm2, alarms[0]);
  TEST_ASSERT_EQUAL(alarm1, alarms[1]);
  TEST_ASSERT_EQUAL(alarm3, alarms[2]);
  
  printf("✓ ソートテスト: 成功\n");
}

// 過去アラーム削除テスト
void test_past_alarm_removal() {
  std::vector<time_t> alarms;
  time_t now = time(NULL);
  time_t past_alarm = now - 3600; // 1時間前
  time_t future_alarm = now + 3600; // 1時間後
  
  AlarmLogic::addAlarm(alarms, past_alarm);
  AlarmLogic::addAlarm(alarms, future_alarm);
  TEST_ASSERT_EQUAL(2, alarms.size());
  
  // 過去のアラームを削除
  AlarmLogic::removePastAlarms(alarms, now);
  TEST_ASSERT_EQUAL(1, alarms.size());
  TEST_ASSERT_EQUAL(future_alarm, alarms[0]);
  
  printf("✓ 過去アラーム削除テスト: 成功\n");
}

// 次のアラーム取得テスト
void test_next_alarm_time() {
  std::vector<time_t> alarms;
  time_t now = time(NULL);
  time_t alarm1 = now + 1800; // 30分後
  time_t alarm2 = now + 3600; // 1時間後
  
  // アラームなしの場合
  TEST_ASSERT_EQUAL(0, AlarmLogic::getNextAlarmTime(alarms, now));
  
  // アラームありの場合
  AlarmLogic::addAlarm(alarms, alarm2);
  AlarmLogic::addAlarm(alarms, alarm1);
  
  time_t next = AlarmLogic::getNextAlarmTime(alarms, now);
  TEST_ASSERT_EQUAL(alarm1, next);
  
  printf("✓ 次のアラーム取得テスト: 成功\n");
}

// メイン関数
int main() {
  UNITY_BEGIN();
  
  printf("=== AlarmLogic Unit Test ===\n");
  
  RUN_TEST(test_alarm_addition);
  RUN_TEST(test_maximum_alarm_limit);
  RUN_TEST(test_alarm_removal);
  RUN_TEST(test_duplicate_check);
  RUN_TEST(test_alarm_sorting);
  RUN_TEST(test_past_alarm_removal);
  RUN_TEST(test_next_alarm_time);
  
  printf("=== 全テスト完了 ===\n");
  
  return UNITY_END();
} 