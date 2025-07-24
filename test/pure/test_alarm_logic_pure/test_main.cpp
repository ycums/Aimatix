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

void test_alarmlogic_add_alarm() {
    std::vector<time_t> alarms;
    time_t now = 1000;
    AlarmLogic::AddAlarmResult result;
    std::string msg;
    // 正常系: 新規追加
    bool ok = AlarmLogic::addAlarm(alarms, now, now+60, result, msg);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL((int)AlarmLogic::AddAlarmResult::Success, (int)result);
    TEST_ASSERT_EQUAL(1, alarms.size());
    // 異常系: 未入力（__:__）
    ok = AlarmLogic::addAlarm(alarms, now, -1, result, msg);
    TEST_ASSERT_FALSE(ok);
    TEST_ASSERT_EQUAL((int)AlarmLogic::AddAlarmResult::ErrorEmptyInput, (int)result);
    // 異常系: 重複
    ok = AlarmLogic::addAlarm(alarms, now, now+60, result, msg);
    TEST_ASSERT_FALSE(ok);
    TEST_ASSERT_EQUAL((int)AlarmLogic::AddAlarmResult::ErrorDuplicate, (int)result);
    // 異常系: 上限超過
    alarms = {now+10, now+20, now+30, now+40, now+50};
    ok = AlarmLogic::addAlarm(alarms, now, now+60, result, msg);
    TEST_ASSERT_FALSE(ok);
    TEST_ASSERT_EQUAL((int)AlarmLogic::AddAlarmResult::ErrorMaxReached, (int)result);
    // 正常系: 過去時刻→翌日扱い
    alarms.clear();
    ok = AlarmLogic::addAlarm(alarms, now, now-10, result, msg);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL((int)AlarmLogic::AddAlarmResult::Success, (int)result);
    // 異常系: 不正値
    ok = AlarmLogic::addAlarm(alarms, now, -99999, result, msg);
    TEST_ASSERT_FALSE(ok);
    TEST_ASSERT_EQUAL((int)AlarmLogic::AddAlarmResult::ErrorInvalid, (int)result);
}

void setUp(void) {}
void tearDown(void) {}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_alarmlogic_init_and_remove);
    RUN_TEST(test_alarmlogic_remain_and_progress);
    RUN_TEST(test_alarmlogic_time_strings);
    RUN_TEST(test_alarmlogic_edge_and_error_cases);
    RUN_TEST(test_alarmlogic_add_alarm);
    UNITY_END();
    return 0;
} 