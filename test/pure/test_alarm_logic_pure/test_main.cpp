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
    // 正常系: 不正値→補正
    ok = AlarmLogic::addAlarm(alarms, now, -99999, result, msg);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL((int)AlarmLogic::AddAlarmResult::Success, (int)result);
}

void test_alarm_correction_minute_only_1() {
    std::vector<time_t> alarms;
    time_t now;
    AlarmLogic::AddAlarmResult result;
    std::string msg;
    struct tm base_tm = {};
    base_tm.tm_year = 124; base_tm.tm_mon = 0; base_tm.tm_mday = 1; base_tm.tm_hour = 14; base_tm.tm_min = 35; base_tm.tm_sec = 0;
    now = mktime(&base_tm);
    int input = 1;
    alarms.clear();
    AlarmLogic::addAlarm(alarms, now, input, result, msg);
    struct tm* tm1 = localtime(&alarms[0]);
    TEST_ASSERT_EQUAL(15, tm1->tm_hour);
    TEST_ASSERT_EQUAL(1, tm1->tm_min);
}
void test_alarm_correction_minute_only_10() {
    std::vector<time_t> alarms;
    time_t now;
    AlarmLogic::AddAlarmResult result;
    std::string msg;
    struct tm base_tm = {};
    base_tm.tm_year = 124; base_tm.tm_mon = 0; base_tm.tm_mday = 1; base_tm.tm_hour = 14; base_tm.tm_min = 35; base_tm.tm_sec = 0;
    now = mktime(&base_tm);
    int input = 10;
    alarms.clear();
    AlarmLogic::addAlarm(alarms, now, input, result, msg);
    struct tm* tm2 = localtime(&alarms[0]);
    TEST_ASSERT_EQUAL(15, tm2->tm_hour);
    TEST_ASSERT_EQUAL(10, tm2->tm_min);
}
void test_alarm_correction_minute_only_12() {
    std::vector<time_t> alarms;
    time_t now;
    AlarmLogic::AddAlarmResult result;
    std::string msg;
    struct tm base_tm = {};
    base_tm.tm_year = 124; base_tm.tm_mon = 0; base_tm.tm_mday = 1; base_tm.tm_hour = 14; base_tm.tm_min = 35; base_tm.tm_sec = 0;
    now = mktime(&base_tm);
    int input = 12;
    alarms.clear();
    AlarmLogic::addAlarm(alarms, now, input, result, msg);
    struct tm* tm3 = localtime(&alarms[0]);
    TEST_ASSERT_EQUAL(15, tm3->tm_hour);
    TEST_ASSERT_EQUAL(12, tm3->tm_min);
}
void test_alarm_correction_minute_only_99() {
    std::vector<time_t> alarms;
    time_t now;
    AlarmLogic::AddAlarmResult result;
    std::string msg;
    struct tm base_tm = {};
    base_tm.tm_year = 124; base_tm.tm_mon = 0; base_tm.tm_mday = 1; base_tm.tm_hour = 14; base_tm.tm_min = 35; base_tm.tm_sec = 0;
    now = mktime(&base_tm);
    int input = 99; // = 1:39
    alarms.clear();
    AlarmLogic::addAlarm(alarms, now, input, result, msg);
    struct tm* tm4 = localtime(&alarms[0]);
    TEST_ASSERT_EQUAL(15, tm4->tm_hour);
    TEST_ASSERT_EQUAL(39, tm4->tm_min);
}
void test_alarm_correction_hour_minute_990() {
    std::vector<time_t> alarms;
    time_t now;
    AlarmLogic::AddAlarmResult result;
    std::string msg;
    struct tm base_tm = {};
    base_tm.tm_year = 124; base_tm.tm_mon = 0; base_tm.tm_mday = 1; base_tm.tm_hour = 14; base_tm.tm_min = 35; base_tm.tm_sec = 0;
    now = mktime(&base_tm);
    int input = 990;
    alarms.clear();
    AlarmLogic::addAlarm(alarms, now, input, result, msg);
    struct tm* tm5 = localtime(&alarms[0]);
    TEST_ASSERT_EQUAL(10, tm5->tm_hour);
    TEST_ASSERT_EQUAL(30, tm5->tm_min);
    TEST_ASSERT_EQUAL(2, tm5->tm_mday);
}
void test_alarm_correction_hour_minute_9999() {
    // 現在時刻: 2024年1月1日 14:35
    // 入力: 9999（99:99）
    // 期待値: 2024年1月1日99:99 → 2024年1月5日04:39
    std::vector<time_t> alarms;
    time_t now;
    AlarmLogic::AddAlarmResult result;
    std::string msg;
    struct tm base_tm = {};
    base_tm.tm_year = 124; base_tm.tm_mon = 0; base_tm.tm_mday = 1; base_tm.tm_hour = 14; base_tm.tm_min = 35; base_tm.tm_sec = 0;
    now = mktime(&base_tm);
    int input = 9999;
    alarms.clear();
    AlarmLogic::addAlarm(alarms, now, input, result, msg);
    struct tm* tm6 = localtime(&alarms[0]);
    
    TEST_ASSERT_EQUAL(4, tm6->tm_hour);
    TEST_ASSERT_EQUAL(39, tm6->tm_min);
    TEST_ASSERT_EQUAL(5, tm6->tm_mday);
}
void test_alarm_correction_past_hour_minute() {
    std::vector<time_t> alarms;
    time_t now;
    AlarmLogic::AddAlarmResult result;
    std::string msg;
    struct tm base_tm = {};
    base_tm.tm_year = 124; base_tm.tm_mon = 0; base_tm.tm_mday = 1; base_tm.tm_hour = 14; base_tm.tm_min = 35; base_tm.tm_sec = 0;
    now = mktime(&base_tm);
    int input = 1200; // 12:00（過去時刻）
    alarms.clear();
    AlarmLogic::addAlarm(alarms, now, input, result, msg);
    struct tm* tm7 = localtime(&alarms[0]);
    TEST_ASSERT_EQUAL(2, tm7->tm_mday); // 翌日
    TEST_ASSERT_EQUAL(12, tm7->tm_hour);
    TEST_ASSERT_EQUAL(0, tm7->tm_min);
}

// === ここからTDD: 部分的な入力状態の解釈テスト ===
void test_partial_input_hour_only() {
    std::vector<time_t> alarms;
    time_t now;
    AlarmLogic::AddAlarmResult result;
    std::string msg;
    struct tm base_tm = {};
    base_tm.tm_year = 124; base_tm.tm_mon = 0; base_tm.tm_mday = 1; base_tm.tm_hour = 14; base_tm.tm_min = 35; base_tm.tm_sec = 0;
    now = mktime(&base_tm);
    
    // _1:__ → 01:00（修正後：時一桁のみ入力時は時一桁として解釈）
    int digits[4] = {0, 0, 0, 0};
    bool entered[4] = {false, false, false, false};
    digits[1] = 1; entered[1] = true; // 時一桁のみ入力
    
    alarms.clear();
    bool ok = AlarmLogic::addAlarmFromPartialInput(alarms, now, digits, entered, result, msg);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL((int)AlarmLogic::AddAlarmResult::Success, (int)result);
    
    struct tm* tm = localtime(&alarms[0]);
    TEST_ASSERT_EQUAL(2, tm->tm_mday); // 翌日（01:00は現在時刻14:35より過去）
    TEST_ASSERT_EQUAL(1, tm->tm_hour); // 修正後：1（修正前：10）
    TEST_ASSERT_EQUAL(0, tm->tm_min);
}

void test_partial_input_minute_only() {
    std::vector<time_t> alarms;
    time_t now;
    AlarmLogic::AddAlarmResult result;
    std::string msg;
    struct tm base_tm = {};
    base_tm.tm_year = 124; base_tm.tm_mon = 0; base_tm.tm_mday = 1; base_tm.tm_hour = 14; base_tm.tm_min = 35; base_tm.tm_sec = 0;
    now = mktime(&base_tm);
    
    // __:01 → 15:01（分のみ入力時は現在時間の同じ分として設定）
    int digits[4] = {0, 0, 0, 0};
    bool entered[4] = {false, false, false, false};
    digits[3] = 1; entered[3] = true; // 分一桁のみ入力
    
    alarms.clear();
    bool ok = AlarmLogic::addAlarmFromPartialInput(alarms, now, digits, entered, result, msg);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL((int)AlarmLogic::AddAlarmResult::Success, (int)result);
    
    struct tm* tm = localtime(&alarms[0]);
    TEST_ASSERT_EQUAL(1, tm->tm_mday); // 当日（15:01は現在時刻14:35より未来）
    TEST_ASSERT_EQUAL(15, tm->tm_hour); // 現在時間の次の時間
    TEST_ASSERT_EQUAL(1, tm->tm_min); // 修正後：分一桁のみ入力時は分一桁として解釈
}

void test_partial_input_hour_minute_partial() {
    std::vector<time_t> alarms;
    time_t now;
    AlarmLogic::AddAlarmResult result;
    std::string msg;
    struct tm base_tm = {};
    base_tm.tm_year = 124; base_tm.tm_mon = 0; base_tm.tm_mday = 1; base_tm.tm_hour = 14; base_tm.tm_min = 35; base_tm.tm_sec = 0;
    now = mktime(&base_tm);
    
    // _0:1_ → 00:10（修正後：時が未入力、分が未入力として扱う）
    int digits[4] = {0, 0, 0, 0};
    bool entered[4] = {false, false, false, false};
    digits[0] = 0; entered[0] = true; // 時十桁入力（時が未入力として扱う）
    digits[2] = 1; entered[2] = true; // 分十桁入力（分が未入力として扱う）
    
    alarms.clear();
    bool ok = AlarmLogic::addAlarmFromPartialInput(alarms, now, digits, entered, result, msg);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL((int)AlarmLogic::AddAlarmResult::Success, (int)result);
    
    struct tm* tm = localtime(&alarms[0]);
    TEST_ASSERT_EQUAL(1, tm->tm_mday); // 当日（時が未入力、分が未入力なので現在時間の次の時間）
    TEST_ASSERT_EQUAL(15, tm->tm_hour); // 現在時間の次の時間
    TEST_ASSERT_EQUAL(0, tm->tm_min); // 分が未入力なので0
}

void test_partial_input_future_time() {
    std::vector<time_t> alarms;
    time_t now;
    AlarmLogic::AddAlarmResult result;
    std::string msg;
    struct tm base_tm = {};
    base_tm.tm_year = 124; base_tm.tm_mon = 0; base_tm.tm_mday = 1; base_tm.tm_hour = 14; base_tm.tm_min = 35; base_tm.tm_sec = 0;
    now = mktime(&base_tm);
    
    // _5:__ → 05:00（未来時刻なので当日）
    int digits[4] = {0, 0, 0, 0};
    bool entered[4] = {false, false, false, false};
    digits[1] = 5; entered[1] = true; // 時一桁のみ入力
    
    alarms.clear();
    bool ok = AlarmLogic::addAlarmFromPartialInput(alarms, now, digits, entered, result, msg);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL((int)AlarmLogic::AddAlarmResult::Success, (int)result);
    
    struct tm* tm = localtime(&alarms[0]);
    TEST_ASSERT_EQUAL(2, tm->tm_mday); // 翌日（05:00は現在時刻14:35より過去）
    TEST_ASSERT_EQUAL(5, tm->tm_hour);
    TEST_ASSERT_EQUAL(0, tm->tm_min);
}

// === ここからTDD: addAlarmAtTimeメソッドのテスト ===
void test_add_alarm_at_time_success() {
    std::vector<time_t> alarms;
    time_t alarmTime = 1000; // 固定時刻
    AlarmLogic::AddAlarmResult result;
    std::string msg;
    
    // 正常系: 新規追加
    bool ok = AlarmLogic::addAlarmAtTime(alarms, alarmTime, result, msg);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL((int)AlarmLogic::AddAlarmResult::Success, (int)result);
    TEST_ASSERT_EQUAL(1, alarms.size());
    TEST_ASSERT_EQUAL(alarmTime, alarms[0]);
}

void test_add_alarm_at_time_duplicate() {
    std::vector<time_t> alarms;
    time_t alarmTime = 1000;
    AlarmLogic::AddAlarmResult result;
    std::string msg;
    
    // 最初のアラームを追加
    AlarmLogic::addAlarmAtTime(alarms, alarmTime, result, msg);
    
    // 重複アラームを追加
    bool ok = AlarmLogic::addAlarmAtTime(alarms, alarmTime, result, msg);
    TEST_ASSERT_FALSE(ok);
    TEST_ASSERT_EQUAL((int)AlarmLogic::AddAlarmResult::ErrorDuplicate, (int)result);
    TEST_ASSERT_EQUAL(1, alarms.size()); // サイズは変わらない
}

void test_add_alarm_at_time_max_reached() {
    std::vector<time_t> alarms = {1000, 2000, 3000, 4000, 5000}; // 最大数に達している
    time_t alarmTime = 6000;
    AlarmLogic::AddAlarmResult result;
    std::string msg;
    
    // 上限超過のアラームを追加
    bool ok = AlarmLogic::addAlarmAtTime(alarms, alarmTime, result, msg);
    TEST_ASSERT_FALSE(ok);
    TEST_ASSERT_EQUAL((int)AlarmLogic::AddAlarmResult::ErrorMaxReached, (int)result);
    TEST_ASSERT_EQUAL(5, alarms.size()); // サイズは変わらない
}

void test_add_alarm_at_time_sorting() {
    std::vector<time_t> alarms;
    AlarmLogic::AddAlarmResult result;
    std::string msg;
    
    // 順序を変えてアラームを追加
    AlarmLogic::addAlarmAtTime(alarms, 3000, result, msg);
    AlarmLogic::addAlarmAtTime(alarms, 1000, result, msg);
    AlarmLogic::addAlarmAtTime(alarms, 2000, result, msg);
    
    // ソートされていることを確認
    TEST_ASSERT_EQUAL(3, alarms.size());
    TEST_ASSERT_EQUAL(1000, alarms[0]);
    TEST_ASSERT_EQUAL(2000, alarms[1]);
    TEST_ASSERT_EQUAL(3000, alarms[2]);
}

void test_add_alarm_at_time_multiple_success() {
    std::vector<time_t> alarms;
    AlarmLogic::AddAlarmResult result;
    std::string msg;
    
    // 複数のアラームを正常に追加
    TEST_ASSERT_TRUE(AlarmLogic::addAlarmAtTime(alarms, 1000, result, msg));
    TEST_ASSERT_TRUE(AlarmLogic::addAlarmAtTime(alarms, 2000, result, msg));
    TEST_ASSERT_TRUE(AlarmLogic::addAlarmAtTime(alarms, 3000, result, msg));
    
    TEST_ASSERT_EQUAL(3, alarms.size());
    TEST_ASSERT_EQUAL(1000, alarms[0]);
    TEST_ASSERT_EQUAL(2000, alarms[1]);
    TEST_ASSERT_EQUAL(3000, alarms[2]);
}

// 完全未入力時の確定拒絶テスト（バグレポート修正確認）
void test_partial_input_no_input_rejection() {
    std::vector<time_t> alarms;
    time_t now;
    AlarmLogic::AddAlarmResult result;
    std::string msg;
    struct tm base_tm = {};
    base_tm.tm_year = 124; base_tm.tm_mon = 0; base_tm.tm_mday = 1; 
    base_tm.tm_hour = 14; base_tm.tm_min = 35; base_tm.tm_sec = 0;
    now = mktime(&base_tm);
    
    // __:__ → 完全未入力（確定拒絶されるべき）
    int digits[4] = {0, 0, 0, 0};
    bool entered[4] = {false, false, false, false};
    
    alarms.clear();
    bool ok = AlarmLogic::addAlarmFromPartialInput(alarms, now, digits, entered, result, msg);
    TEST_ASSERT_FALSE(ok);
    TEST_ASSERT_EQUAL((int)AlarmLogic::AddAlarmResult::ErrorInvalid, (int)result);
    TEST_ASSERT_EQUAL_STRING("Invalid time format", msg.c_str());
    TEST_ASSERT_EQUAL(0, alarms.size()); // アラームは追加されない
}

// deleteAlarm()のテスト
// バグレポート3-0-14の具体的ケース: __:5_ → 00:50
void test_bugreport_3_0_14_minute_only_5() {
    std::vector<time_t> alarms;
    time_t now;
    AlarmLogic::AddAlarmResult result;
    std::string msg;
    
    // 現在時刻: 00:00 を設定
    struct tm base_tm = {};
    base_tm.tm_year = 124; base_tm.tm_mon = 0; base_tm.tm_mday = 1; 
    base_tm.tm_hour = 0; base_tm.tm_min = 0; base_tm.tm_sec = 0;
    now = mktime(&base_tm);
    
    // __:5_ → 分一桁のみに5を入力
    int digits[4] = {0, 0, 0, 0};
    bool entered[4] = {false, false, false, false};
    digits[3] = 5; entered[3] = true; // 分一桁のみ入力
    
    alarms.clear();
    bool ok = AlarmLogic::addAlarmFromPartialInput(alarms, now, digits, entered, result, msg);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL((int)AlarmLogic::AddAlarmResult::Success, (int)result);
    
    // 期待値: 00:05 (修正後)
    struct tm* tm = localtime(&alarms[0]);
    TEST_ASSERT_EQUAL(1, tm->tm_mday); // 同日（00:05は未来時刻のため）
    TEST_ASSERT_EQUAL(0, tm->tm_hour);
    TEST_ASSERT_EQUAL(5, tm->tm_min); // 修正後の期待値：分一桁として解釈
}

void test_alarmlogic_delete_alarm() {
    std::vector<time_t> alarms = {1000, 2000, 3000, 4000};
    
    // 正常系: 存在するアラームを削除
    bool ok = AlarmLogic::deleteAlarm(alarms, 1); // インデックス1の2000を削除
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL(3, alarms.size());
    TEST_ASSERT_EQUAL(1000, alarms[0]);
    TEST_ASSERT_EQUAL(3000, alarms[1]);
    TEST_ASSERT_EQUAL(4000, alarms[2]);
    
    // 正常系: 最初のアラームを削除
    ok = AlarmLogic::deleteAlarm(alarms, 0);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL(2, alarms.size());
    TEST_ASSERT_EQUAL(3000, alarms[0]);
    TEST_ASSERT_EQUAL(4000, alarms[1]);
    
    // 正常系: 最後のアラームを削除
    ok = AlarmLogic::deleteAlarm(alarms, 1);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL(1, alarms.size());
    TEST_ASSERT_EQUAL(3000, alarms[0]);
    
    // 異常系: インデックスが範囲外
    ok = AlarmLogic::deleteAlarm(alarms, 1);
    TEST_ASSERT_FALSE(ok);
    TEST_ASSERT_EQUAL(1, alarms.size()); // サイズは変わらない
    
    // 異常系: 空のリスト
    std::vector<time_t> empty_alarms;
    ok = AlarmLogic::deleteAlarm(empty_alarms, 0);
    TEST_ASSERT_FALSE(ok);
    TEST_ASSERT_EQUAL(0, empty_alarms.size());
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
    RUN_TEST(test_alarm_correction_minute_only_1);
    RUN_TEST(test_alarm_correction_minute_only_10);
    RUN_TEST(test_alarm_correction_minute_only_12);
    RUN_TEST(test_alarm_correction_minute_only_99);
    RUN_TEST(test_alarm_correction_hour_minute_990);
    RUN_TEST(test_alarm_correction_hour_minute_9999);
    RUN_TEST(test_alarm_correction_past_hour_minute);
    RUN_TEST(test_partial_input_hour_only);
    RUN_TEST(test_partial_input_minute_only);
    RUN_TEST(test_partial_input_hour_minute_partial);
    RUN_TEST(test_partial_input_future_time);
    RUN_TEST(test_add_alarm_at_time_success);
    RUN_TEST(test_add_alarm_at_time_duplicate);
    RUN_TEST(test_add_alarm_at_time_max_reached);
    RUN_TEST(test_add_alarm_at_time_sorting);
    RUN_TEST(test_add_alarm_at_time_multiple_success);
    RUN_TEST(test_partial_input_no_input_rejection);
    RUN_TEST(test_bugreport_3_0_14_minute_only_5);
    RUN_TEST(test_alarmlogic_delete_alarm);
    UNITY_END();
    return 0;
} 