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
    // 未入力（__:__）
    if (input == -1) {
        result = AddAlarmResult::ErrorEmptyInput;
        errorMsg = "Input is empty.";
        return false;
    }
    // 不正値（負値や異常値）
    if (input < 0 || input > 24*60*60) { // 24時間超は不正
        result = AddAlarmResult::ErrorInvalid;
        errorMsg = "Invalid time value.";
        return false;
    }
    // 上限超過
    if (alarms.size() >= 5) {
        result = AddAlarmResult::ErrorMaxReached;
        errorMsg = "Maximum number of alarms reached.";
        return false;
    }
    // 重複チェック
    for (const auto& t : alarms) {
        if (t == input) {
            result = AddAlarmResult::ErrorDuplicate;
            errorMsg = "Alarm already exists.";
            return false;
        }
    }
    // 過去時刻は翌日扱い
    time_t alarmTime = input;
    if (input <= now) {
        alarmTime = input + 24*60*60; // 翌日
    }
    // 再度重複チェック（翌日化で重複する場合）
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