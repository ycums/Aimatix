#include <unity.h>
#include <cstdio>
#include <ctime>
#include <alarm.h>

// alarm.cpp/hの本体ロジックを直接テスト

void setUp(void) {
    alarmTimes.clear();
}
void tearDown(void) {}

void test_addDebugAlarms() {
    alarmTimes.clear();
    addDebugAlarms();
    TEST_ASSERT_EQUAL(5, alarmTimes.size());
    printf("✓ addDebugAlarms: 成功\n");
}

void test_sortAlarms() {
    alarmTimes.clear();
    time_t now = time(NULL);
    alarmTimes.push_back(now + 7200);
    alarmTimes.push_back(now + 3600);
    alarmTimes.push_back(now + 10800);
    sortAlarms();
    // ここでnowを使って正しい順序を検証
    TEST_ASSERT_EQUAL(now + 3600, alarmTimes[0]);
    TEST_ASSERT_EQUAL(now + 7200, alarmTimes[1]);
    TEST_ASSERT_EQUAL(now + 10800, alarmTimes[2]);
    printf("✓ sortAlarms: 成功\n");
}

void test_getNextAlarmTime() {
    alarmTimes.clear();
    time_t now = time(NULL);
    alarmTimes.push_back(now - 3600);
    alarmTimes.push_back(now + 3600);
    alarmTimes.push_back(now + 7200);
    sortAlarms();
    time_t next = getNextAlarmTime();
    TEST_ASSERT_EQUAL(now + 3600, next);
    printf("✓ getNextAlarmTime: 成功\n");
}

void test_removePastAlarms() {
    alarmTimes.clear();
    time_t now = time(NULL);
    alarmTimes.push_back(now - 3600);
    alarmTimes.push_back(now - 1800);
    alarmTimes.push_back(now + 3600);
    alarmTimes.push_back(now + 7200);
    removePastAlarms();
    // 未来のアラームのみが残ることを検証
    TEST_ASSERT_EQUAL(2, alarmTimes.size());
    TEST_ASSERT_EQUAL(now + 3600, alarmTimes[0]);
    TEST_ASSERT_EQUAL(now + 7200, alarmTimes[1]);
    printf("✓ removePastAlarms: 成功\n");
}

int main() {
    UNITY_BEGIN();
    printf("=== alarm.cpp/h 本体ロジック直接テスト ===\n");
    RUN_TEST(test_addDebugAlarms);
    RUN_TEST(test_sortAlarms);
    RUN_TEST(test_getNextAlarmTime);
    RUN_TEST(test_removePastAlarms);
    printf("=== 全テスト完了 ===\n");
    return UNITY_END();
} 