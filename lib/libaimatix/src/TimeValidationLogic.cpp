#include "TimeValidationLogic.h"
#include <cstring>

bool TimeValidationLogic::isSystemTimeBeforeMinimum(ITimeProvider* timeProvider) {
    if (timeProvider == nullptr) {
        return true; // timeProviderがnullの場合は補正が必要とみなす
    }
    
    const time_t currentTime = timeProvider->now();
    const time_t minimumTime = getMinimumSystemTime();
    
    return currentTime < minimumTime;
}

bool TimeValidationLogic::correctSystemTimeToMinimum(ITimeProvider* timeProvider) {
    if (timeProvider == nullptr) {
        return false; // timeProviderがnullの場合は補正不可
    }
    
    const time_t minimumTime = getMinimumSystemTime();
    return timeProvider->setSystemTime(minimumTime);
}

time_t TimeValidationLogic::getMinimumSystemTime() {
    struct tm minTime = {};
    minTime.tm_year = MIN_SYSTEM_YEAR - 1900; // tm_yearは1900年からの経過年数
    minTime.tm_mon = MIN_SYSTEM_MONTH - 1;    // tm_monは0ベース (0-11)
    minTime.tm_mday = MIN_SYSTEM_DAY;
    minTime.tm_hour = MIN_SYSTEM_HOUR;
    minTime.tm_min = MIN_SYSTEM_MINUTE;
    minTime.tm_sec = MIN_SYSTEM_SECOND;
    minTime.tm_isdst = -1; // 自動判定
    
    return mktime(&minTime);
}

bool TimeValidationLogic::validateAndCorrectSystemTime(ITimeProvider* timeProvider) {
    if (timeProvider == nullptr) {
        return false; // timeProviderがnullの場合は何もしない
    }
    
    if (isSystemTimeBeforeMinimum(timeProvider)) {
        // 最小時刻以前の場合は補正を実行
        return correctSystemTimeToMinimum(timeProvider);
    }
    
    // 補正不要
    return false;
}