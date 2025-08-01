#include "AlarmLogic.h"
#include "PartialInputLogic.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cstdio> // For printf

// 定数定義
constexpr int HOURS_10 = 10;
constexpr int HOURS_24 = 24;
constexpr int HOURS_100 = 100;
constexpr int MINUTES_60 = 60;
constexpr int SECONDS_10 = 10;
constexpr int SECONDS_30 = 30;
constexpr int SECONDS_60 = 60;
constexpr int SECONDS_120 = 120;
constexpr int PERCENT_100 = 100;

auto AlarmLogic::initAlarms(std::vector<time_t>& alarms, time_t now) -> void {
    alarms.clear();
    alarms.push_back(now + SECONDS_10);    // +10秒
    alarms.push_back(now + SECONDS_30);    // +30秒
    alarms.push_back(now + SECONDS_60);    // +1分
    alarms.push_back(now + SECONDS_120);   // +2分
}

auto AlarmLogic::removePastAlarms(std::vector<time_t>& alarms, time_t now) -> void {
    alarms.erase(
        alarms.begin(),
        std::find_if(alarms.begin(), alarms.end(), [now](time_t time_value) { return time_value > now; })
    );
}

auto AlarmLogic::getRemainSec(const std::vector<time_t>& alarms, time_t now) -> int {
    if (alarms.empty()) {
        return 0;
    }
    return static_cast<int>(alarms.front() - now);
}

auto AlarmLogic::getRemainPercent(int remainSec, int totalSec) -> int {
    if (totalSec <= 0) {
        return 0;
    }
    int percent = (remainSec * PERCENT_100) / totalSec;
    if (percent < 0) {
        percent = 0;
    }
    if (percent > PERCENT_100) {
        percent = PERCENT_100;
    }
    return percent;
}

auto AlarmLogic::getAlarmTimeStrings(const std::vector<time_t>& alarms, std::vector<std::string>& out) -> void {
    out.clear();
    for (auto t : alarms) {
        std::tm* tm_alarm = std::localtime(&t);
        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(2) << tm_alarm->tm_hour << ":"
            << std::setw(2) << tm_alarm->tm_min;
        out.push_back(oss.str());
    }
} 

// addAlarm: 入力値をアラームとして追加。エラー時はresult, errorMsgに理由を格納。
auto AlarmLogic::addAlarm(std::vector<time_t>& alarms, time_t now, time_t input, AddAlarmResult& result, std::string& errorMsg) -> bool {
    if (input == -1) {
        result = AddAlarmResult::ErrorEmptyInput;
        errorMsg = "Input is empty.";
        return false;
    }
    struct tm* now_tm = localtime(&now);
    struct tm alarm_tm = *now_tm;
    alarm_tm.tm_sec = 0;
    alarm_tm.tm_isdst = -1;
    int hour = 0;
    int minute = 0;
    int add_day = 0;
    if (input < HOURS_100) {
        // 分のみ指定
        minute = input % MINUTES_60;
        hour = input / MINUTES_60;
        if (hour > 0) {
            // 分が60以上の場合、時分に正規化
            minute = input % MINUTES_60;
            hour = input / MINUTES_60;
        } else {
            // 分のみの場合
            minute = input;
            hour = 0;
        }
        // 現在時刻の次のその時刻を計算
        if (hour > 0 || minute > now_tm->tm_min) {
            // 時が指定されているか、分が現在分より大きい場合
            if (hour == 0) {
                hour = now_tm->tm_hour;
                if (minute <= now_tm->tm_min) {
                    hour += 1;
                }
            } else {
                hour += now_tm->tm_hour;
            }
        } else {
            hour = now_tm->tm_hour + 1;
        }
        // 繰り上げ
        if (hour >= HOURS_24) { hour -= HOURS_24; add_day = 1; }
        alarm_tm.tm_hour = hour;
        alarm_tm.tm_min = minute;
        alarm_tm.tm_mday += add_day;
        
        // 過去時刻チェック（分のみ指定の場合も）
        const time_t candidate = mktime(&alarm_tm);
        if (candidate <= now) {
            // 現在時刻より前なら翌日
            alarm_tm.tm_mday += 1;
        }
    } else {
        // 時分指定
        hour = input / HOURS_100;
        minute = input % HOURS_100;
        // 分繰り上げ
        if (minute >= MINUTES_60) { hour += minute / MINUTES_60; minute = minute % MINUTES_60; }
        // 時繰り上げ
        const int add_day = hour / HOURS_24;
        hour = hour % HOURS_24;
        alarm_tm.tm_hour = hour;
        alarm_tm.tm_min = minute;
        alarm_tm.tm_mday += add_day;
        const time_t candidate = mktime(&alarm_tm);
        if (candidate <= now) {
            // 現在時刻より前なら翌日
            alarm_tm.tm_mday += 1;
        }
    }
            const time_t alarmTime = mktime(&alarm_tm);
    
    // 最大数チェック
    constexpr int MAX_ALARMS = 5;
    if (alarms.size() >= MAX_ALARMS) {
        result = AddAlarmResult::ErrorMaxReached;
        errorMsg = "Max alarms reached (5)";
        return false;
    }
    
    // 重複チェック
    for (auto existing : alarms) {
        if (existing == alarmTime) {
            result = AddAlarmResult::ErrorDuplicate;
            errorMsg = "Duplicate alarm time";
            return false;
        }
    }
    
    alarms.push_back(alarmTime);
    std::sort(alarms.begin(), alarms.end());
    result = AddAlarmResult::Success;
    return true;
}

// 絶対時刻（time_t）をアラームとして追加。エラー時はresult, errorMsgに理由を格納。
auto AlarmLogic::addAlarmAtTime(std::vector<time_t>& alarms, time_t alarmTime, AddAlarmResult& result, std::string& errorMsg) -> bool {
    // 最大数チェック
    constexpr int MAX_ALARMS = 5;
    if (alarms.size() >= MAX_ALARMS) {
        result = AddAlarmResult::ErrorMaxReached;
        errorMsg = "Max alarms reached (5)";
        return false;
    }
    
    // 重複チェック
    for (auto existing : alarms) {
        if (existing == alarmTime) {
            result = AddAlarmResult::ErrorDuplicate;
            errorMsg = "Duplicate alarm time";
            return false;
        }
    }
    
    alarms.push_back(alarmTime);
    std::sort(alarms.begin(), alarms.end());
    result = AddAlarmResult::Success;
    return true;
}

// 部分的な入力状態（digits[4], entered[4]）からアラームを追加
auto AlarmLogic::addAlarmFromPartialInput(
    std::vector<time_t>& alarms, 
    time_t now, 
    const int* digits, 
    const bool* entered, 
    AddAlarmResult& result, 
    std::string& errorMsg
) -> bool {
    // 入力チェック
    if (digits == nullptr || entered == nullptr) {
        result = AddAlarmResult::ErrorInvalid;
        errorMsg = "Invalid input data";
        return false;
    }
    
    // 部分的な入力状態を完全な時分に変換（PartialInputLogicを使用）
    auto parsedTime = PartialInputLogic::parsePartialInput(digits, entered);
    if (!parsedTime.isValid) {
        result = AddAlarmResult::ErrorInvalid;
        errorMsg = "Invalid time format";
        return false;
    }
    
    int hour = parsedTime.hour;
    int minute = parsedTime.minute;
    
    // 時分を直接指定してアラーム追加（既存のaddAlarm関数を拡張して使用）
    struct tm* now_tm = localtime(&now);
    struct tm alarm_tm = *now_tm;
    alarm_tm.tm_sec = 0;
    alarm_tm.tm_isdst = -1;
    
    // 分繰り上げ
    if (minute >= MINUTES_60) { hour += minute / MINUTES_60; minute = minute % MINUTES_60; }
    // 時繰り上げ
    const int add_day = hour / HOURS_24;
    hour = hour % HOURS_24;
    
    alarm_tm.tm_hour = hour;
    alarm_tm.tm_min = minute;
    alarm_tm.tm_mday += add_day;
    
    const time_t candidate = mktime(&alarm_tm);
    if (candidate <= now) {
        // 現在時刻より前なら翌日
        alarm_tm.tm_mday += 1;
    }
    
            const time_t alarmTime = mktime(&alarm_tm);
    
    // 最大数チェック
    constexpr int MAX_ALARMS_PARTIAL = 5;
    if (alarms.size() >= MAX_ALARMS_PARTIAL) {
        result = AddAlarmResult::ErrorMaxReached;
        errorMsg = "Max alarms reached (5)";
        return false;
    }
    
    // 重複チェック
    for (auto existing : alarms) {
        if (existing == alarmTime) {
            result = AddAlarmResult::ErrorDuplicate;
            errorMsg = "Duplicate alarm time";
            return false;
        }
    }
    
    alarms.push_back(alarmTime);
    std::sort(alarms.begin(), alarms.end());
    result = AddAlarmResult::Success;
    return true;
}

// 指定インデックスのアラームを削除
auto AlarmLogic::deleteAlarm(std::vector<time_t>& alarms, size_t index) -> bool {
    if (index >= alarms.size()) {
        return false;
    }
    
    // 指定されたインデックスのアラームを削除
    alarms.erase(alarms.begin() + index);
    
    // 削除後にソート（念のため）
    std::sort(alarms.begin(), alarms.end());
    
    return true;
}

// アラームリストを取得（時刻順でソート済み）
auto AlarmLogic::getAlarms(const std::vector<time_t>& alarms) -> std::vector<time_t> {
    std::vector<time_t> sortedAlarms{alarms};
    std::sort(sortedAlarms.begin(), sortedAlarms.end());
    return sortedAlarms;
} 