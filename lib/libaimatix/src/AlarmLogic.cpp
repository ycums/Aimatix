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

int AlarmLogic::getProgressPercent(int remainSec, int totalSec) {
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
            << std::setw(2) << tm_alarm->tm_min << ":"
            << std::setw(2) << tm_alarm->tm_sec;
        out.push_back(oss.str());
    }
} 