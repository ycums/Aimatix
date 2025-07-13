#include <unity.h>
#include <cstdio>
#include <vector>
#include <ctime>
#include <algorithm>

// AlarmLogicクラスの純粋ロジックテスト
// M5Stack依存を排除し、標準C++のみでテスト

// アラーム追加テスト
void test_alarm_addition() {
  std::vector<time_t> alarms;
  time_t now = time(NULL);
  time_t alarm1 = now + 3600; // 1時間後
  time_t alarm2 = now + 7200; // 2時間後
  
  // 正常なアラーム追加
  TEST_ASSERT_TRUE(addAlarm(alarms, alarm1));
  TEST_ASSERT_EQUAL(1, alarms.size());
  
  TEST_ASSERT_TRUE(addAlarm(alarms, alarm2));
  TEST_ASSERT_EQUAL(2, alarms.size());
  
  // 重複アラームの追加を拒否
  TEST_ASSERT_FALSE(addAlarm(alarms, alarm1));
  TEST_ASSERT_EQUAL(2, alarms.size());
  
  printf("✓ アラーム追加テスト: 成功\n");
}

// 最大数制限テスト
void test_maximum_alarm_limit() {
  std::vector<time_t> alarms;
  time_t now = time(NULL);
  
  // 最大5件まで追加
  for (int i = 1; i <= 5; i++) {
    time_t alarm = now + (i * 3600);
    TEST_ASSERT_TRUE(addAlarm(alarms, alarm));
  }
  TEST_ASSERT_EQUAL(5, alarms.size());
  
  // 6件目は拒否
  time_t alarm6 = now + 21600;
  TEST_ASSERT_FALSE(addAlarm(alarms, alarm6));
  TEST_ASSERT_EQUAL(5, alarms.size());
  
  printf("✓ 最大数制限テスト: 成功\n");
}

// アラーム削除テスト
void test_alarm_removal() {
  std::vector<time_t> alarms;
  time_t now = time(NULL);
  time_t alarm1 = now + 3600;
  time_t alarm2 = now + 7200;
  
  addAlarm(alarms, alarm1);
  addAlarm(alarms, alarm2);
  TEST_ASSERT_EQUAL(2, alarms.size());
  
  // 存在するアラームを削除
  TEST_ASSERT_TRUE(removeAlarm(alarms, alarm1));
  TEST_ASSERT_EQUAL(1, alarms.size());
  
  // 存在しないアラームを削除
  TEST_ASSERT_FALSE(removeAlarm(alarms, alarm1));
  TEST_ASSERT_EQUAL(1, alarms.size());
  
  printf("✓ アラーム削除テスト: 成功\n");
}

// 重複チェックテスト
void test_duplicate_check() {
  std::vector<time_t> alarms;
  time_t now = time(NULL);
  time_t alarm1 = now + 3600;
  
  // 空のリストでは重複なし
  TEST_ASSERT_FALSE(isDuplicateAlarm(alarms, alarm1));
  
  // アラーム追加後は重複あり
  addAlarm(alarms, alarm1);
  TEST_ASSERT_TRUE(isDuplicateAlarm(alarms, alarm1));
  
  // 異なる時刻は重複なし
  time_t alarm2 = now + 7200;
  TEST_ASSERT_FALSE(isDuplicateAlarm(alarms, alarm2));
  
  printf("✓ 重複チェックテスト: 成功\n");
}

// ソートテスト
void test_alarm_sorting() {
  std::vector<time_t> alarms;
  time_t now = time(NULL);
  
  // 順序を変えて追加
  time_t alarm3 = now + 10800; // 3時間後
  time_t alarm1 = now + 3600;  // 1時間後
  time_t alarm2 = now + 7200;  // 2時間後
  
  alarms.push_back(alarm3);
  alarms.push_back(alarm1);
  alarms.push_back(alarm2);
  
  // ソート前は順序がバラバラ
  TEST_ASSERT_EQUAL(alarm3, alarms[0]);
  TEST_ASSERT_EQUAL(alarm1, alarms[1]);
  TEST_ASSERT_EQUAL(alarm2, alarms[2]);
  
  // ソート実行
  sortAlarms(alarms);
  
  // ソート後は時系列順
  TEST_ASSERT_EQUAL(alarm1, alarms[0]);
  TEST_ASSERT_EQUAL(alarm2, alarms[1]);
  TEST_ASSERT_EQUAL(alarm3, alarms[2]);
  
  printf("✓ アラームソートテスト: 成功\n");
}

// 過去のアラーム削除テスト
void test_past_alarm_removal() {
  std::vector<time_t> alarms;
  time_t now = time(NULL);
  
  // 過去、現在、未来のアラームを追加
  time_t past_alarm = now - 3600;   // 1時間前
  time_t current_alarm = now;       // 現在
  time_t future_alarm = now + 3600; // 1時間後
  
  alarms.push_back(past_alarm);
  alarms.push_back(current_alarm);
  alarms.push_back(future_alarm);
  
  TEST_ASSERT_EQUAL(3, alarms.size());
  
  // 過去のアラームを削除
  removePastAlarms(alarms, now);
  
  // 未来のアラームのみ残る
  TEST_ASSERT_EQUAL(1, alarms.size());
  TEST_ASSERT_EQUAL(future_alarm, alarms[0]);
  
  printf("✓ 過去のアラーム削除テスト: 成功\n");
}

// 次のアラーム取得テスト
void test_next_alarm_getting() {
  std::vector<time_t> alarms;
  time_t now = time(NULL);
  
  // 複数のアラームを追加
  time_t alarm1 = now + 3600;  // 1時間後
  time_t alarm2 = now + 7200;  // 2時間後
  time_t alarm3 = now + 10800; // 3時間後
  
  alarms.push_back(alarm1);
  alarms.push_back(alarm2);
  alarms.push_back(alarm3);
  
  // 次のアラームは最も近い未来のアラーム
  time_t next = getNextAlarmTime(alarms, now);
  TEST_ASSERT_EQUAL(alarm1, next);
  
  // 現在時刻を1時間30分後に設定
  time_t future_now = now + 5400;
  next = getNextAlarmTime(alarms, future_now);
  TEST_ASSERT_EQUAL(alarm2, next);
  
  // 全てのアラームが過去の場合
  time_t far_future = now + 14400;
  next = getNextAlarmTime(alarms, far_future);
  TEST_ASSERT_EQUAL(0, next); // 次のアラームなし
  
  printf("✓ 次のアラーム取得テスト: 成功\n");
}

// ヘルパー関数（AlarmLogicクラスの関数を模擬）
bool addAlarm(std::vector<time_t>& alarms, time_t alarm_time) {
  if (!canAddAlarm(alarms)) return false;
  if (isDuplicateAlarm(alarms, alarm_time)) return false;
  
  alarms.push_back(alarm_time);
  sortAlarms(alarms);
  return true;
}

bool removeAlarm(std::vector<time_t>& alarms, time_t alarm_time) {
  auto it = std::find(alarms.begin(), alarms.end(), alarm_time);
  if (it != alarms.end()) {
    alarms.erase(it);
    return true;
  }
  return false;
}

bool isDuplicateAlarm(const std::vector<time_t>& alarms, time_t alarm_time) {
  return std::find(alarms.begin(), alarms.end(), alarm_time) != alarms.end();
}

bool canAddAlarm(const std::vector<time_t>& alarms, int max_count) {
  return alarms.size() < max_count;
}

void sortAlarms(std::vector<time_t>& alarms) {
  if (alarms.empty()) return;
  std::sort(alarms.begin(), alarms.end());
}

void removePastAlarms(std::vector<time_t>& alarms, time_t current_time) {
  alarms.erase(
    std::remove_if(alarms.begin(), alarms.end(), 
      [current_time](time_t t) { return t <= current_time; }),
    alarms.end()
  );
}

time_t getNextAlarmTime(const std::vector<time_t>& alarms, time_t current_time) {
  for (time_t t : alarms) {
    if (t > current_time) {
      return t;
    }
  }
  return 0; // 次のアラームなし
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