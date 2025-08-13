#include "AlarmLogic.h"
#include "PartialInputLogic.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cstdio> // For printf
#include "TimeThreadSafe.h"

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

void AlarmLogic::initAlarms(std::vector<time_t>& alarms, time_t now) {
    alarms.clear();
    alarms.push_back(now + SECONDS_10);    // +10秒
    alarms.push_back(now + SECONDS_30);    // +30秒
    alarms.push_back(now + SECONDS_60);    // +1分
    alarms.push_back(now + SECONDS_120);   // +2分
}

void AlarmLogic::removePastAlarms(std::vector<time_t>& alarms, time_t now) {
    alarms.erase(
        alarms.begin(),
        std::find_if(alarms.begin(), alarms.end(), [now](time_t time_value) { return time_value > now; })
    );
}

int AlarmLogic::getRemainSec(const std::vector<time_t>& alarms, time_t now) {
    if (alarms.empty()) {
        return 0;
    }
    return static_cast<int>(alarms.front() - now);
}

int AlarmLogic::getRemainPercent(int remainSec, int totalSec) {
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

void AlarmLogic::getAlarmTimeStrings(const std::vector<time_t>& alarms, std::vector<std::string>& out) {
    out.clear();
    for (const time_t& t : alarms) {
        std::tm tm_alarm{};
        if (!TimeThreadSafe::toLocalTime(t, tm_alarm)) {
            continue;
        }
        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(2) << tm_alarm.tm_hour << ":"
            << std::setw(2) << tm_alarm.tm_min;
        out.push_back(oss.str());
    }
} 

// addAlarm: 入力値をアラームとして追加。エラー時はresult, errorMsgに理由を格納。
bool AlarmLogic::addAlarm(std::vector<time_t>& alarms, time_t now, time_t input, AddAlarmResult& result, std::string& errorMsg) {
    if (input == -1) {
        result = AddAlarmResult::ErrorEmptyInput;
        errorMsg = "Input is empty.";
        return false;
    }
    struct tm now_tm_buf{};
    if (!TimeThreadSafe::toLocalTime(now, now_tm_buf)) {
        result = AddAlarmResult::ErrorInvalid;
        errorMsg = "Invalid current time";
        return false;
    }
    struct tm alarm_tm = now_tm_buf;
    alarm_tm.tm_sec = 0;
    alarm_tm.tm_isdst = -1;
    int hour = 0;
    int minute = 0;
    int add_day = 0;
    if (input < HOURS_100) {
        // 分のみ指定
        const long inputLong = static_cast<long>(input);
        minute = static_cast<int>(inputLong % MINUTES_60);
        hour = static_cast<int>(inputLong / MINUTES_60);
        if (hour > 0) {
            // 分が60以上の場合、時分に正規化
            minute = static_cast<int>(inputLong % MINUTES_60);
            hour = static_cast<int>(inputLong / MINUTES_60);
        } else {
            // 分のみの場合
            minute = static_cast<int>(inputLong);
            hour = 0;
        }
        // 現在時刻の次のその時刻を計算
        if (hour > 0 || minute > now_tm_buf.tm_min) {
            // 時が指定されているか、分が現在分より大きい場合
            if (hour == 0) {
                hour = now_tm_buf.tm_hour;
                if (minute <= now_tm_buf.tm_min) {
                    hour += 1;
                }
            }
        } else {
            // 分が現在分以下の場合、次の時間の同じ分として設定
            hour = (now_tm_buf.tm_hour + 1) % HOURS_24;
        }
    } else {
        // 時分指定（HHMM形式）
        const long inputLong = static_cast<long>(input);
        hour = static_cast<int>(inputLong / HOURS_100);
        minute = static_cast<int>(inputLong % HOURS_100);
        
        // 分繰り上げ
        if (minute >= MINUTES_60) { 
            hour += minute / MINUTES_60; 
            minute = minute % MINUTES_60; 
        }
        // 時繰り上げ
        add_day = hour / HOURS_24;
        hour = hour % HOURS_24;
        alarm_tm.tm_mday += add_day;
        
        const time_t candidate = mktime(&alarm_tm);
        if (candidate <= now) {
            // 過去時刻の場合：翌日の同じ時刻として処理
            alarm_tm.tm_mday += 1;
        }
    }
    
    alarm_tm.tm_hour = hour;
    alarm_tm.tm_min = minute;
    
    const time_t alarmTime = mktime(&alarm_tm);
    
    // 最大数チェック
    constexpr int MAX_ALARMS = 5;
    if (alarms.size() >= MAX_ALARMS) {
        result = AddAlarmResult::ErrorMaxReached;
        errorMsg = "Max alarms reached (5)";
        return false;
    }
    
    // 重複チェック
    for (const time_t& existing : alarms) {
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
bool AlarmLogic::addAlarmAtTime(std::vector<time_t>& alarms, time_t alarmTime, AddAlarmResult& result, std::string& errorMsg) {
    // 最大数チェック
    constexpr int MAX_ALARMS = 5;
    if (alarms.size() >= MAX_ALARMS) {
        result = AddAlarmResult::ErrorMaxReached;
        errorMsg = "Max alarms reached (5)";
        return false;
    }
    
    // 重複チェック
    for (const time_t& existing : alarms) {
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
bool AlarmLogic::addAlarmFromPartialInput(
    std::vector<time_t>& alarms, 
    time_t now, 
    const int* digits, 
    const bool* entered, 
    AddAlarmResult& result, 
    std::string& errorMsg
) {
    // 入力チェック
    if (digits == nullptr || entered == nullptr) {
        result = AddAlarmResult::ErrorInvalid;
        errorMsg = "Invalid input data";
        return false;
    }
    
    // 部分的な入力状態を完全な時分に変換（PartialInputLogicを使用）
    const PartialInputLogic::ParsedTime parsedTime = PartialInputLogic::parsePartialInput(digits, entered);
    if (!parsedTime.isValid) {
        result = AddAlarmResult::ErrorInvalid;
        errorMsg = "Invalid time format";
        return false;
    }
    
    int hour = parsedTime.hour;
    int minute = parsedTime.minute;
    
    // 時分を直接指定してアラーム追加
    struct tm now_tm_buf{};
    if (!TimeThreadSafe::toLocalTime(now, now_tm_buf)) {
        result = AddAlarmResult::ErrorInvalid;
        errorMsg = "Invalid current time";
        return false;
    }
    
    struct tm alarm_tm = now_tm_buf;
    alarm_tm.tm_sec = 0;
    alarm_tm.tm_isdst = -1;
    
    // 時が指定されていない場合の処理
    if (!parsedTime.hourSpecified) {
        // 分のみで過去か未来かを判定
        if (minute <= now_tm_buf.tm_min) {
            // 分が現在分以下なら、次の時間の同じ分として設定
            hour = (now_tm_buf.tm_hour + 1) % HOURS_24;
        } else {
            // 分が現在分より大きいなら、現在時間の同じ分として設定
            hour = now_tm_buf.tm_hour;
        }
    } else {
        // 時が指定されている場合：通常の処理
        // 分繰り上げ
        if (minute >= MINUTES_60) { 
            hour += minute / MINUTES_60; 
            minute = minute % MINUTES_60; 
        }
        // 時繰り上げ
        const int add_day = hour / HOURS_24;
        hour = hour % HOURS_24;
        alarm_tm.tm_mday += add_day;
        
        // 時刻をセット
        alarm_tm.tm_hour = hour;
        alarm_tm.tm_min = minute;
        
        const time_t candidate = mktime(&alarm_tm);
        
        // 現在時刻も秒を0にして比較（秒の差で誤判定されるのを防ぐ）
        struct tm now_tm_compare = now_tm_buf;
        now_tm_compare.tm_sec = 0;
        const time_t now_compare = mktime(&now_tm_compare);
        
        if (candidate <= now_compare) {
            // 過去時刻の場合：翌日の同じ時刻として処理
            alarm_tm.tm_mday += 1;
        }
    }
    
    alarm_tm.tm_hour = hour;
    alarm_tm.tm_min = minute;
    
    const time_t alarmTime = mktime(&alarm_tm);
    
    // 最大数チェック
    constexpr int MAX_ALARMS_PARTIAL = 5;
    if (alarms.size() >= MAX_ALARMS_PARTIAL) {
        result = AddAlarmResult::ErrorMaxReached;
        errorMsg = "Max alarms reached (5)";
        return false;
    }
    
    // 重複チェック
    for (const time_t& existing : alarms) {
        if (existing == alarmTime) {
            result = AddAlarmResult::ErrorDuplicate;
            errorMsg = "Duplicate alarm time";
            return false;
        }
    }
    
    // アラーム追加
    alarms.push_back(alarmTime);
    std::sort(alarms.begin(), alarms.end());
    result = AddAlarmResult::Success;
    return true;
}

// 指定インデックスのアラームを削除
bool AlarmLogic::deleteAlarm(std::vector<time_t>& alarms, size_t index) {
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
std::vector<time_t> AlarmLogic::getAlarms(const std::vector<time_t>& alarms) {
    std::vector<time_t> sortedAlarms{alarms};
    std::sort(sortedAlarms.begin(), sortedAlarms.end());
    return sortedAlarms;
} 