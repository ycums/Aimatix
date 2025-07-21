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
    int remainPercent = AlarmLogic::getRemainPercent(remain, 120);
    TEST_ASSERT_EQUAL(10, remain);
    TEST_ASSERT_EQUAL(8, remainPercent); // 10/120*100=8
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
        TEST_ASSERT_EQUAL(5, (int)s.size());
        TEST_ASSERT_EQUAL(':', s[2]);
    }
}

void test_alarmlogic_edge_and_error_cases() {
    // removePastAlarms: 全て過去
    std::vector<time_t> alarms = {100, 200, 300};
    AlarmLogic::removePastAlarms(alarms, 500);
    TEST_ASSERT_EQUAL(0, alarms.size());
    // removePastAlarms: 1つだけ未来
    alarms = {100, 200, 600};
    AlarmLogic::removePastAlarms(alarms, 500);
    TEST_ASSERT_EQUAL(1, alarms.size());
    TEST_ASSERT_EQUAL(600, alarms[0]);
    // getRemainSec: 空リスト
    alarms.clear();
    int remain = AlarmLogic::getRemainSec(alarms, 1000);
    TEST_ASSERT_EQUAL(0, remain);
    // getRemainSec: 負の値
    alarms = {900};
    remain = AlarmLogic::getRemainSec(alarms, 1000);
    TEST_ASSERT_EQUAL(-100, remain);
    // getRemainPercent: totalSec=0, remainSec負値、100超
    TEST_ASSERT_EQUAL(0, AlarmLogic::getRemainPercent(10, 0));
    TEST_ASSERT_EQUAL(0, AlarmLogic::getRemainPercent(-10, 100));
    TEST_ASSERT_EQUAL(100, AlarmLogic::getRemainPercent(200, 100));
    // getAlarmTimeStrings: 空リスト
    alarms.clear();
    std::vector<std::string> strs;
    AlarmLogic::getAlarmTimeStrings(alarms, strs);
    TEST_ASSERT_EQUAL(0, strs.size());
}

void setUp(void) {}
void tearDown(void) {}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_alarmlogic_init_and_remove);
    RUN_TEST(test_alarmlogic_remain_and_progress);
    RUN_TEST(test_alarmlogic_time_strings);
    RUN_TEST(test_alarmlogic_edge_and_error_cases);
    UNITY_END();
    return 0;
} 