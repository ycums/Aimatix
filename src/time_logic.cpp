#include "time_logic.h"
#include <algorithm>
#include <cstring>
#include <sstream>
#include <iomanip>

// TimeLogic 実装
bool TimeLogic::isValidTime(int hour, int minute) {
    return hour >= 0 && hour <= 23 && minute >= 0 && minute <= 59;
}

time_t TimeLogic::calculateRelativeTime(int base_hour, int base_min, int add_hour, int add_min, bool is_plus) {
    time_t now = time(NULL);
    struct tm tm_info;
    localtime_r(&now, &tm_info);
    
    tm_info.tm_hour = base_hour;
    tm_info.tm_min = base_min;
    tm_info.tm_sec = 0;
    
    time_t base_time = mktime(&tm_info);
    int total_minutes = add_hour * 60 + add_min;
    
    if (!is_plus) total_minutes = -total_minutes;
    
    return base_time + total_minutes * 60;
}

time_t TimeLogic::calculateAbsoluteTime(int hour, int minute) {
    time_t now = time(NULL);
    struct tm tm_info;
    localtime_r(&now, &tm_info);
    
    tm_info.tm_hour = hour;
    tm_info.tm_min = minute;
    tm_info.tm_sec = 0;
    
    return mktime(&tm_info);
}

time_t TimeLogic::calculateAlarmTime(int input_hour, int input_min, int mode, time_t current_time) {
    if (mode == 1 || mode == 2) { // REL_PLUS_TIME_INPUT || REL_MINUS_TIME_INPUT
        // 相対時刻入力
        struct tm tm_info;
        localtime_r(&current_time, &tm_info);
        int total = input_hour * 60 + input_min;
        if (mode == 2) total = -total; // REL_MINUS_TIME_INPUT
        
        time_t base = current_time;
        base += total * 60;
        struct tm t;
        localtime_r(&base, &t);
        t.tm_sec = 0;
        return mktime(&t);
    } else {
        // 絶対時刻入力
        return calculateAbsoluteTime(input_hour, input_min);
    }
}

void TimeLogic::formatTime(time_t time, int& hour, int& minute) {
    struct tm tm_info;
    localtime_r(&time, &tm_info);
    hour = tm_info.tm_hour;
    minute = tm_info.tm_min;
}

void TimeLogic::formatTimeString(time_t time, char* buffer, size_t buffer_size) {
    struct tm tm_info;
    localtime_r(&time, &tm_info);
    snprintf(buffer, buffer_size, "%02d:%02d", tm_info.tm_hour, tm_info.tm_min);
}

// AlarmLogic 実装
bool AlarmLogic::addAlarm(std::vector<time_t>& alarms, time_t alarm_time) {
    if (!canAddAlarm(alarms)) return false;
    if (isDuplicateAlarm(alarms, alarm_time)) return false;
    
    alarms.push_back(alarm_time);
    sortAlarms(alarms);
    return true;
}

bool AlarmLogic::removeAlarm(std::vector<time_t>& alarms, time_t alarm_time) {
    auto it = std::find(alarms.begin(), alarms.end(), alarm_time);
    if (it != alarms.end()) {
        alarms.erase(it);
        return true;
    }
    return false;
}

bool AlarmLogic::isDuplicateAlarm(const std::vector<time_t>& alarms, time_t alarm_time) {
    return std::find(alarms.begin(), alarms.end(), alarm_time) != alarms.end();
}

bool AlarmLogic::canAddAlarm(const std::vector<time_t>& alarms, int max_count) {
    return alarms.size() < max_count;
}

void AlarmLogic::sortAlarms(std::vector<time_t>& alarms) {
    if (alarms.empty()) return;
    std::sort(alarms.begin(), alarms.end());
}

void AlarmLogic::removePastAlarms(std::vector<time_t>& alarms, time_t current_time) {
    alarms.erase(
        std::remove_if(alarms.begin(), alarms.end(), 
            [current_time](time_t t) { return t <= current_time; }),
        alarms.end()
    );
}

time_t AlarmLogic::getNextAlarmTime(const std::vector<time_t>& alarms, time_t current_time) {
    for (time_t t : alarms) {
        if (t > current_time) {
            return t;
        }
    }
    return 0; // 次のアラームなし
}

// InputLogic 実装
void InputLogic::incrementDigit(int& digit, int max_value, int increment) {
    digit = (digit + increment) % (max_value + 1);
}

void InputLogic::decrementDigit(int& digit, int max_value, int decrement) {
    digit = (digit - decrement + max_value + 1) % (max_value + 1);
}

bool InputLogic::isValidHourTens(int tens, int ones) {
    if (tens == 2) return ones <= 3;
    return tens <= 1;
}

bool InputLogic::isValidHourOnes(int tens, int ones) {
    if (tens == 2) return ones <= 3;
    return ones <= 9;
}

bool InputLogic::isValidMinTens(int tens) {
    return tens <= 5;
}

bool InputLogic::isValidMinOnes(int ones) {
    return ones <= 9;
}

void InputLogic::inputToTime(int hour_tens, int hour_ones, int min_tens, int min_ones, int& hour, int& minute) {
    hour = hour_tens * 10 + hour_ones;
    minute = min_tens * 10 + min_ones;
}

void InputLogic::timeToInput(int hour, int minute, int& hour_tens, int& hour_ones, int& min_tens, int& min_ones) {
    hour_tens = hour / 10;
    hour_ones = hour % 10;
    min_tens = minute / 10;
    min_ones = minute % 10;
} 