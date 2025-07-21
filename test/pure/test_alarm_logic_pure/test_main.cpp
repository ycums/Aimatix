#include <unity.h>
#include <vector>
#include <ctime>
#include <string>
#include "AlarmLogic.h"

void test_alarmlogic_init_and_remove() {
    std::vector<time_t> alarms;
    time_t now = 1000;
    AlarmLogic::initAlarms(alarms, now);
    TEST_ASSERT_EQUAL(4, alarms.size());
    // +10, +30, +60, +120
    TEST_ASSERT_EQUAL(now+10, alarms[0]);
    TEST_ASSERT_EQUAL(now+30, alarms[1]);
    TEST_ASSERT_EQUAL(now+60, alarms[2]);
    TEST_ASSERT_EQUAL(now+120, alarms[3]);
    // removePastAlarms
    AlarmLogic::removePastAlarms(alarms, now+31);
    TEST_ASSERT_EQUAL(2, alarms.size());
    TEST_ASSERT_EQUAL(now+60, alarms[0]);
    TEST_ASSERT_EQUAL(now+120, alarms[1]);
}

void test_alarmlogic_remain_and_progress() {
    std::vector<time_t> alarms;
    time_t now = 2000;
    AlarmLogic::initAlarms(alarms, now);
    int remain = AlarmLogic::getRemainSec(alarms, now);
    TEST_ASSERT_EQUAL(10, remain);
    int progress = AlarmLogic::getProgressPercent(remain, 120);
    TEST_ASSERT_EQUAL(8, progress); // 10/120*100=8
}

void test_alarmlogic_time_strings() {
    std::vector<time_t> alarms;
    time_t now = 3000;
    AlarmLogic::initAlarms(alarms, now);
    std::vector<std::string> strs;
    AlarmLogic::getAlarmTimeStrings(alarms, strs);
    TEST_ASSERT_EQUAL(4, strs.size());
    // 文字列の形式だけ簡易チェック
    for (const auto& s : strs) {
        TEST_ASSERT_EQUAL(8, (int)s.size());
        TEST_ASSERT_EQUAL(':', s[2]);
        TEST_ASSERT_EQUAL(':', s[5]);
    }
}

void setUp(void) {}
void tearDown(void) {}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_alarmlogic_init_and_remove);
    RUN_TEST(test_alarmlogic_remain_and_progress);
    RUN_TEST(test_alarmlogic_time_strings);
    UNITY_END();
    return 0;
} 