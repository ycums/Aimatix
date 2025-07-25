#include "AlarmLogic.h"
#include <algorithm>
#include <iomanip>
#include <sstream>

void AlarmLogic::initAlarms(std::vector<time_t>& alarms, time_t now) {
    alarms.clear();
    alarms.push_back(now + 10);    // +10秒
    alarms.push_back(now + 30);    // +30秒
    alarms.push_back(now + 60);    // +1分
    alarms.push_back(now + 120);   // +2分
}

void AlarmLogic::removePastAlarms(std::vector<time_t>& alarms, time_t now) {
    alarms.erase(
        alarms.begin(),
        std::find_if(alarms.begin(), alarms.end(), [now](time_t t) { return t > now; })
    );
}

int AlarmLogic::getRemainSec(const std::vector<time_t>& alarms, time_t now) {
    if (alarms.empty()) return 0;
    return static_cast<int>(alarms.front() - now);
}

int AlarmLogic::getRemainPercent(int remainSec, int totalSec) {
    if (totalSec <= 0) return 0;
    int percent = (remainSec * 100) / totalSec;
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;
    return percent;
}

void AlarmLogic::getAlarmTimeStrings(const std::vector<time_t>& alarms, std::vector<std::string>& out) {
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
bool AlarmLogic::addAlarm(std::vector<time_t>& alarms, time_t now, time_t input, AddAlarmResult& result, std::string& errorMsg) {
    if (input == -1) {
        result = AddAlarmResult::ErrorEmptyInput;
        errorMsg = "Input is empty.";
        return false;
    }
    struct tm* now_tm = localtime(&now);
    struct tm alarm_tm = *now_tm;
    alarm_tm.tm_sec = 0;
    alarm_tm.tm_isdst = -1;
    int h, m, add_day = 0;
    if (input < 100) {
        // 分のみ指定
        m = input % 60;
        h = input / 60;
        if (h > 0) {
            // 分が60以上の場合、時分に正規化
            m = input % 60;
            h = input / 60;
        } else {
            // 分のみの場合
            m = input;
            h = 0;
        }
        // 現在時刻の次のその時刻を計算
        if (h > 0 || m > now_tm->tm_min) {
            // 時が指定されているか、分が現在分より大きい場合
            if (h == 0) {
                h = now_tm->tm_hour;
                if (m <= now_tm->tm_min) {
                    h += 1;
                }
            } else {
                h += now_tm->tm_hour;
            }
        } else {
            h = now_tm->tm_hour + 1;
        }
        // 繰り上げ
        if (h >= 24) { h -= 24; add_day = 1; }
        alarm_tm.tm_hour = h;
        alarm_tm.tm_min = m;
        alarm_tm.tm_mday += add_day;
    } else {
        // 時分指定
        h = input / 100;
        m = input % 100;
        // 分繰り上げ
        if (m >= 60) { h += m / 60; m = m % 60; }
        // 時繰り上げ
        add_day = h / 24;
        h = h % 24;
        alarm_tm.tm_hour = h;
        alarm_tm.tm_min = m;
        alarm_tm.tm_mday += add_day;
        time_t candidate = mktime(&alarm_tm);
        if (candidate <= now) {
            // 現在時刻より前なら翌日
            alarm_tm.tm_mday += 1;
        }
    }
    time_t alarmTime = mktime(&alarm_tm);
    if (alarms.size() >= 5) {
        result = AddAlarmResult::ErrorMaxReached;
        errorMsg = "Maximum number of alarms reached.";
        return false;
    }
    for (const auto& t : alarms) {
        if (t == alarmTime) {
            result = AddAlarmResult::ErrorDuplicate;
            errorMsg = "Alarm already exists.";
            return false;
        }
    }
    alarms.push_back(alarmTime);
    std::sort(alarms.begin(), alarms.end());
    result = AddAlarmResult::Success;
    errorMsg = "";
    return true;
} 