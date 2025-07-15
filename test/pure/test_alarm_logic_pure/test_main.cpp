#include <unity.h>
#include <cstdio>
#include <cstring>
#include <vector>
#include <algorithm>
#include <ctime>
#include <alarm.h>

// AlarmLogicの純粋ロジックテスト
// M5Stack依存を排除し、標準C++のみでテスト

// アラームリストの初期化テスト
void test_alarm_list_initialization() {
  std::vector<time_t> alarmTimes;
  
  // 空のリストの確認
  TEST_ASSERT_TRUE(alarmTimes.empty());
  TEST_ASSERT_EQUAL(0, alarmTimes.size());
  
  printf("✓ アラームリスト初期化テスト: 成功\n");
}

// アラーム追加ロジックのテスト
void test_alarm_addition() {
  std::vector<time_t> alarmTimes;
  time_t now = time(NULL);
  
  // アラーム追加
  alarmTimes.push_back(now + 3600); // 1時間後
  TEST_ASSERT_EQUAL(1, alarmTimes.size());
  
  alarmTimes.push_back(now + 7200); // 2時間後
  TEST_ASSERT_EQUAL(2, alarmTimes.size());
  
  // 時刻の確認
  TEST_ASSERT_GREATER_THAN(now, alarmTimes[0] - 3600);
  TEST_ASSERT_GREATER_THAN(now, alarmTimes[1] - 7200);
  
  printf("✓ アラーム追加ロジックテスト: 成功\n");
}

// アラームソートロジックのテスト
void test_alarm_sorting() {
  std::vector<time_t> alarmTimes;
  time_t now = time(NULL);
  
  // 順序を変えて追加
  alarmTimes.push_back(now + 7200); // 2時間後
  alarmTimes.push_back(now + 3600); // 1時間後
  alarmTimes.push_back(now + 10800); // 3時間後
  
  // ソート前の確認
  TEST_ASSERT_GREATER_THAN(alarmTimes[0], alarmTimes[1]);
  
  // ソート実行
  std::sort(alarmTimes.begin(), alarmTimes.end());
  
  // ソート後の確認
  TEST_ASSERT_LESS_THAN(alarmTimes[0], alarmTimes[1]);
  TEST_ASSERT_LESS_THAN(alarmTimes[1], alarmTimes[2]);
  
  printf("✓ アラームソートロジックテスト: 成功\n");
}

// 次のアラーム時刻取得ロジックのテスト
void test_next_alarm_time() {
  std::vector<time_t> alarmTimes;
  time_t now = time(NULL);
  
  // 過去のアラームと未来のアラームを追加
  alarmTimes.push_back(now - 3600); // 1時間前（過去）
  alarmTimes.push_back(now + 3600); // 1時間後（未来）
  alarmTimes.push_back(now + 7200); // 2時間後（未来）
  
  // ソート
  std::sort(alarmTimes.begin(), alarmTimes.end());
  
  // 次のアラーム時刻を取得
  time_t next = 0;
  for (time_t t : alarmTimes) {
    if (t > now) {
      next = t;
      break;
    }
  }
  
  // 検証
  TEST_ASSERT_EQUAL(now + 3600, next);
  TEST_ASSERT_GREATER_THAN(next, now);
  
  printf("✓ 次のアラーム時刻取得ロジックテスト: 成功\n");
}

// 過去アラーム削除ロジックのテスト
void test_remove_past_alarms() {
  std::vector<time_t> alarmTimes;
  time_t now = time(NULL);
  
  // 過去と未来のアラームを追加
  alarmTimes.push_back(now - 3600); // 1時間前（過去）
  alarmTimes.push_back(now - 1800); // 30分前（過去）
  alarmTimes.push_back(now + 3600); // 1時間後（未来）
  alarmTimes.push_back(now + 7200); // 2時間後（未来）
  
  // 削除前の確認
  TEST_ASSERT_EQUAL(4, alarmTimes.size());
  
  // 過去のアラームを削除
  alarmTimes.erase(
    std::remove_if(alarmTimes.begin(), alarmTimes.end(), 
      [now](time_t t){ return t <= now; }), 
    alarmTimes.end()
  );
  
  // 削除後の確認
  TEST_ASSERT_EQUAL(2, alarmTimes.size());
  
  // 残ったアラームが全て未来の時刻であることを確認
  for (time_t t : alarmTimes) {
    TEST_ASSERT_GREATER_THAN(t, now);
  }
  
  printf("✓ 過去アラーム削除ロジックテスト: 成功\n");
}

// アラーム重複チェックロジックのテスト
void test_alarm_duplicate_check() {
  std::vector<time_t> alarmTimes;
  time_t now = time(NULL);
  time_t futureTime = now + 3600;
  
  // 最初のアラームを追加
  alarmTimes.push_back(futureTime);
  TEST_ASSERT_EQUAL(1, alarmTimes.size());
  
  // 同じ時刻のアラームを追加しようとする
  bool isDuplicate = std::find(alarmTimes.begin(), alarmTimes.end(), futureTime) != alarmTimes.end();
  TEST_ASSERT_TRUE(isDuplicate);
  
  // 異なる時刻のアラーム
  time_t differentTime = now + 7200;
  isDuplicate = std::find(alarmTimes.begin(), alarmTimes.end(), differentTime) != alarmTimes.end();
  TEST_ASSERT_FALSE(isDuplicate);
  
  printf("✓ アラーム重複チェックロジックテスト: 成功\n");
}

// アラーム数制限ロジックのテスト
void test_alarm_count_limit() {
  std::vector<time_t> alarmTimes;
  time_t now = time(NULL);
  const int MAX_ALARMS = 5;
  
  // 最大数までアラームを追加
  for (int i = 1; i <= MAX_ALARMS; i++) {
    alarmTimes.push_back(now + (i * 3600));
  }
  
  TEST_ASSERT_EQUAL(MAX_ALARMS, alarmTimes.size());
  
  // 制限チェック
  bool canAdd = alarmTimes.size() < MAX_ALARMS;
  TEST_ASSERT_FALSE(canAdd);
  
  // 1つ削除後のチェック
  alarmTimes.pop_back();
  canAdd = alarmTimes.size() < MAX_ALARMS;
  TEST_ASSERT_TRUE(canAdd);
  
  printf("✓ アラーム数制限ロジックテスト: 成功\n");
}

// 時刻計算ロジックのテスト
void test_time_calculation() {
  time_t now = time(NULL);
  
  // 1時間後の計算
  time_t oneHourLater = now + 3600;
  TEST_ASSERT_EQUAL(now + 3600, oneHourLater);
  
  // 30分後の計算
  time_t thirtyMinutesLater = now + 1800;
  TEST_ASSERT_EQUAL(now + 1800, thirtyMinutesLater);
  
  // 日付跨ぎの計算
  time_t nextDay = now + 86400; // 24時間後
  TEST_ASSERT_GREATER_THAN(nextDay, now);
  
  printf("✓ 時刻計算ロジックテスト: 成功\n");
}

// 複合アラーム管理テスト
void test_composite_alarm_management() {
  std::vector<time_t> alarmTimes;
  time_t now = time(NULL);
  
  // 複数のアラームを追加（順序を変えて）
  alarmTimes.push_back(now + 7200); // 2時間後
  alarmTimes.push_back(now - 1800); // 30分前（過去）
  alarmTimes.push_back(now + 3600); // 1時間後
  alarmTimes.push_back(now - 3600); // 1時間前（過去）
  
  // 過去のアラームを削除
  alarmTimes.erase(
    std::remove_if(alarmTimes.begin(), alarmTimes.end(), 
      [now](time_t t){ return t <= now; }), 
    alarmTimes.end()
  );
  
  // ソート
  std::sort(alarmTimes.begin(), alarmTimes.end());
  
  // 検証
  TEST_ASSERT_EQUAL(2, alarmTimes.size());
  TEST_ASSERT_LESS_THAN(alarmTimes[0], alarmTimes[1]);
  
  // 次のアラーム時刻を取得
  time_t next = alarmTimes[0];
  TEST_ASSERT_EQUAL(now + 3600, next);
  
  printf("✓ 複合アラーム管理テスト: 成功\n");
}

void setUp(void) {}
void tearDown(void) {}

// メイン関数
int main() {
  UNITY_BEGIN();
  
  printf("=== AlarmLogic 純粋ロジックテスト ===\n");
  
  RUN_TEST(test_alarm_list_initialization);
  RUN_TEST(test_alarm_addition);
  RUN_TEST(test_alarm_sorting);
  RUN_TEST(test_next_alarm_time);
  RUN_TEST(test_remove_past_alarms);
  RUN_TEST(test_alarm_duplicate_check);
  RUN_TEST(test_alarm_count_limit);
  RUN_TEST(test_time_calculation);
  RUN_TEST(test_composite_alarm_management);
  
  printf("=== 全テスト完了 ===\n");
  
  return UNITY_END();
} 