#include <unity.h>
#include <cstdio>
#include <vector>
#include <ctime>
#include "time_logic.h"

// AlarmLogicクラスの純粋ロジックテスト
// M5Stack依存を排除し、標準C++のみでテスト

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
    time_t alarm = base_time + i * 3600;
    TEST_ASSERT_TRUE(AlarmLogic::addAlarm(alarms, alarm));
  }
  TEST_ASSERT_EQUAL(5, alarms.size());
  
  // 6個目は拒否
  time_t alarm6 = base_time + 6 * 3600;
  TEST_ASSERT_FALSE(AlarmLogic::addAlarm(alarms, alarm6));
  TEST_ASSERT_EQUAL(5, alarms.size());
  
  printf("✓ 最大アラーム数制限テスト: 成功\n");
}

// アラーム削除テスト
void test_alarm_removal() {
  std::vector<time_t> alarms;
  time_t alarm1 = 1000000000;
  time_t alarm2 = 1000003600;
  
  // アラームを追加
  AlarmLogic::addAlarm(alarms, alarm1);
  AlarmLogic::addAlarm(alarms, alarm2);
  TEST_ASSERT_EQUAL(2, alarms.size());
  
  // 正常な削除
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
  time_t alarm2 = 1000003600;
  
  // 空のリストでは重複なし
  TEST_ASSERT_FALSE(AlarmLogic::isDuplicateAlarm(alarms, alarm1));
  
  // アラーム追加後は重複あり
  AlarmLogic::addAlarm(alarms, alarm1);
  TEST_ASSERT_TRUE(AlarmLogic::isDuplicateAlarm(alarms, alarm1));
  
  // 異なるアラームは重複なし
  TEST_ASSERT_FALSE(AlarmLogic::isDuplicateAlarm(alarms, alarm2));
  
  printf("✓ 重複チェックテスト: 成功\n");
}

// アラームソートテスト
void test_alarm_sorting() {
  std::vector<time_t> alarms;
  time_t alarm1 = 1000003600; // 2番目
  time_t alarm2 = 1000000000; // 1番目
  time_t alarm3 = 1000007200; // 3番目
  
  // 順序を変えて追加
  AlarmLogic::addAlarm(alarms, alarm1);
  AlarmLogic::addAlarm(alarms, alarm2);
  AlarmLogic::addAlarm(alarms, alarm3);
  
  // ソートされていることを確認
  TEST_ASSERT_EQUAL(alarm2, alarms[0]);
  TEST_ASSERT_EQUAL(alarm1, alarms[1]);
  TEST_ASSERT_EQUAL(alarm3, alarms[2]);
  
  printf("✓ アラームソートテスト: 成功\n");
}

// 過去のアラーム削除テスト
void test_past_alarm_removal() {
  std::vector<time_t> alarms;
  time_t now = 1000000000;
  time_t past1 = now - 3600;   // 1時間前
  time_t past2 = now - 1800;   // 30分前
  time_t future1 = now + 3600; // 1時間後
  time_t future2 = now + 1800; // 30分後
  
  // アラームを追加
  AlarmLogic::addAlarm(alarms, past1);
  AlarmLogic::addAlarm(alarms, past2);
  AlarmLogic::addAlarm(alarms, future1);
  AlarmLogic::addAlarm(alarms, future2);
  TEST_ASSERT_EQUAL(4, alarms.size());
  
  // 過去のアラームを削除
  AlarmLogic::removePastAlarms(alarms, now);
  TEST_ASSERT_EQUAL(2, alarms.size());
  
  // 未来のアラームのみ残っていることを確認
  TEST_ASSERT_EQUAL(future2, alarms[0]);
  TEST_ASSERT_EQUAL(future1, alarms[1]);
  
  printf("✓ 過去のアラーム削除テスト: 成功\n");
}

// 次のアラーム取得テスト
void test_next_alarm_getting() {
  std::vector<time_t> alarms;
  time_t now = 1000000000;
  time_t alarm1 = now + 1800;  // 30分後
  time_t alarm2 = now + 3600;  // 1時間後
  time_t alarm3 = now + 7200;  // 2時間後
  
  // アラームを追加
  AlarmLogic::addAlarm(alarms, alarm2);
  AlarmLogic::addAlarm(alarms, alarm1);
  AlarmLogic::addAlarm(alarms, alarm3);
  
  // 次のアラームを取得
  time_t next = AlarmLogic::getNextAlarmTime(alarms, now);
  TEST_ASSERT_EQUAL(alarm1, next);
  
  printf("✓ 次のアラーム取得テスト: 成功\n");
}

void setUp(void) {
  // テスト前のセットアップ
}

void tearDown(void) {
  // テスト後のクリーンアップ
}

int main() {
  printf("=== AlarmLogicクラステスト開始 ===\n");
  
  UNITY_BEGIN();
  
  RUN_TEST(test_alarm_addition);
  RUN_TEST(test_maximum_alarm_limit);
  RUN_TEST(test_alarm_removal);
  RUN_TEST(test_duplicate_check);
  RUN_TEST(test_alarm_sorting);
  RUN_TEST(test_past_alarm_removal);
  RUN_TEST(test_next_alarm_getting);
  
  UNITY_END();
  
  printf("=== AlarmLogicクラステスト完了 ===\n");
  printf("全テストが完了しました！\n");
  
  return 0;
} 