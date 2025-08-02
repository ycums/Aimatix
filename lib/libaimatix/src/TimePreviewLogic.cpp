#include "TimePreviewLogic.h"
#include <cstdio>
#include <cstring>

TimePreviewLogic::PreviewResult TimePreviewLogic::generatePreview(
    const int* digits, 
    const bool* entered, 
    ITimeProvider* timeProvider,
    bool isRelativeMode
) {
    PreviewResult result;
    result.isValid = false;
    result.preview = "";
    
    if (!timeProvider || !digits || !entered) {
        return result;
    }
    
    if (isRelativeMode) {
        // 相対値モードの場合、InputLogicから相対値を取得する必要がある
        // この実装では相対値の計算は呼び出し側で行う
        return result;
    }
    
    // 絶対値モードの場合
    time_t absoluteTime = calculateAbsoluteTime(digits, entered, timeProvider);
    if (absoluteTime == -1) {
        return result;
    }
    
    result.preview = formatPreview(absoluteTime, timeProvider, false);
    result.isValid = true;
    return result;
}

TimePreviewLogic::PreviewResult TimePreviewLogic::generateRelativePreview(
    time_t relativeTime,
    ITimeProvider* timeProvider
) {
    PreviewResult result;
    result.isValid = false;
    result.preview = "";
    
    if (!timeProvider || relativeTime == -1) {
        return result;
    }
    
    result.preview = formatPreview(relativeTime, timeProvider, true);
    result.isValid = true;
    return result;
}

time_t TimePreviewLogic::calculateAbsoluteTime(
    const int* digits, 
    const bool* entered, 
    ITimeProvider* timeProvider
) {
    if (!timeProvider || !digits || !entered) {
        return -1;
    }
    
    auto parsedTime = PartialInputLogic::parsePartialInput(digits, entered);
    if (!parsedTime.isValid) {
        return -1;
    }
    
    // 現在時刻を取得
    time_t now = timeProvider->now();
    struct tm* now_tm = timeProvider->localtime(&now);
    if (now_tm == nullptr) {
        return -1;
    }
    
    // アラーム時刻を計算
    struct tm alarm_tm = *now_tm;
    alarm_tm.tm_sec = 0;
    alarm_tm.tm_isdst = -1;
    
    int hour = parsedTime.hour;
    int minute = parsedTime.minute;
    
    // 時が指定されていない場合の処理
    if (!parsedTime.hourSpecified) {
        // 分のみで過去か未来かを判定
        if (minute <= now_tm->tm_min) {
            hour = (now_tm->tm_hour + 1) % 24;
        } else {
            hour = now_tm->tm_hour;
        }
    } else {
        // 時が指定されている場合：通常の処理
        if (minute >= 60) { 
            hour += minute / 60; 
            minute = minute % 60; 
        }
        const int add_day = hour / 24;
        hour = hour % 24;
        alarm_tm.tm_mday += add_day;
    }
    
    // 時刻をセット
    alarm_tm.tm_hour = hour;
    alarm_tm.tm_min = minute;
    
    const time_t candidate = mktime(&alarm_tm);
    printf("[DEBUG] calculateAbsoluteTime: candidate=%ld, now=%ld, candidate<=now=%s\n", 
           candidate, now, (candidate <= now) ? "true" : "false");
    if (candidate <= now) {
        // 過去時刻の場合：翌日の同じ時刻として処理
        alarm_tm.tm_mday += 1;
        time_t result = mktime(&alarm_tm);
        printf("[DEBUG] calculateAbsoluteTime: past time, moved to next day, result=%ld\n", result);
        return result;
    }
    
    printf("[DEBUG] calculateAbsoluteTime: future time, result=%ld\n", candidate);
    return candidate;
}

std::string TimePreviewLogic::formatPreview(
    time_t time, 
    ITimeProvider* timeProvider,
    bool isRelativeMode
) {
    if (!timeProvider) {
        return "";
    }
    
    time_t now = timeProvider->now();
    int dayDiff = calculateDayDifference(time, now, timeProvider);
    
    // 静的バッファ問題を回避するため、値をコピー
    time_t time_copy = time;
    struct tm* time_tm = timeProvider->localtime(&time_copy);
    if (!time_tm) {
        return "";
    }
    
    // 値をコピーして静的バッファ問題を回避
    struct tm time_tm_copy = *time_tm;
    
    char buffer[32];
    if (isRelativeMode) {
        // 相対値モードの場合
        if (dayDiff > 0) {
            snprintf(buffer, sizeof(buffer), "+1d %02d:%02d", 
                    time_tm_copy.tm_hour, time_tm_copy.tm_min);
        } else {
            snprintf(buffer, sizeof(buffer), "%02d:%02d", 
                    time_tm_copy.tm_hour, time_tm_copy.tm_min);
        }
    } else {
        // 絶対値モードの場合
        if (dayDiff > 0) {
            snprintf(buffer, sizeof(buffer), "+%dd %02d:%02d", 
                    dayDiff, time_tm_copy.tm_hour, time_tm_copy.tm_min);
        } else {
            snprintf(buffer, sizeof(buffer), "%02d:%02d", 
                    time_tm_copy.tm_hour, time_tm_copy.tm_min);
        }
    }
    
    return std::string(buffer);
}

int TimePreviewLogic::calculateDayDifference(
    time_t targetTime,
    time_t currentTime,
    ITimeProvider* timeProvider
) {
    if (!timeProvider) {
        return 0;
    }
    
    // 静的バッファ問題を回避するため、別々の変数でlocaltimeを呼び出す
    time_t target_copy1 = targetTime;
    time_t current_copy1 = currentTime;
    
    struct tm* target_tm = timeProvider->localtime(&target_copy1);
    if (!target_tm) {
        return 0;
    }
    
    // target_tmの値を先にコピー
    struct tm target_tm_copy = *target_tm;
    
    // 別の変数でcurrent_tmを取得
    time_t current_copy2 = currentTime;
    struct tm* current_tm = timeProvider->localtime(&current_copy2);
    if (!current_tm) {
        return 0;
    }
    
    // current_tmの値をコピー
    struct tm current_tm_copy = *current_tm;
    
    // 日付跨ぎ判定（現在日付との差分を計算）
    int dayDiff = target_tm_copy.tm_mday - current_tm_copy.tm_mday;
    
    // 月をまたぐ場合の処理
    if (dayDiff < 0) {
        // 前月の場合、月の日数を加算
        struct tm temp_tm = current_tm_copy;
        temp_tm.tm_mday = 1;
        temp_tm.tm_mon++;
        time_t nextMonth = mktime(&temp_tm);
        struct tm* nextMonth_tm = localtime(&nextMonth);
        if (nextMonth_tm != nullptr) {
            int daysInMonth = nextMonth_tm->tm_mday - 1;
            dayDiff += daysInMonth;
        }
    }
    
    // 年をまたぐ場合の処理
    if (target_tm_copy.tm_year != current_tm_copy.tm_year) {
        // 年が異なる場合は、年をまたいでいる
        // この場合は単純に日数差を計算
        dayDiff = (target_tm_copy.tm_yday - current_tm_copy.tm_yday);
    }
    
    // デバッグ情報
    printf("[DEBUG] calculateDayDifference: target=%04d-%02d-%02d %02d:%02d, current=%04d-%02d-%02d %02d:%02d, dayDiff=%d\n",
           target_tm_copy.tm_year+1900, target_tm_copy.tm_mon+1, target_tm_copy.tm_mday, target_tm_copy.tm_hour, target_tm_copy.tm_min,
           current_tm_copy.tm_year+1900, current_tm_copy.tm_mon+1, current_tm_copy.tm_mday, current_tm_copy.tm_hour, current_tm_copy.tm_min,
           dayDiff);
    
    return dayDiff;
} 