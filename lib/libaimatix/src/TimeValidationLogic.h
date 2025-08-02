#pragma once
#include "ITimeProvider.h"
#include <ctime>

/**
 * 時刻検証・補正ロジック
 * システム起動時の時刻検証と最小時刻への補正を担当する純粋ロジッククラス
 */
class TimeValidationLogic {
public:
    // 最小システム時刻: 2025/01/01 00:00:00
    static constexpr int MIN_SYSTEM_YEAR = 2025;
    static constexpr int MIN_SYSTEM_MONTH = 1;
    static constexpr int MIN_SYSTEM_DAY = 1;
    static constexpr int MIN_SYSTEM_HOUR = 0;
    static constexpr int MIN_SYSTEM_MINUTE = 0;
    static constexpr int MIN_SYSTEM_SECOND = 0;
    
    /**
     * システム時刻が最小時刻以前かどうかをチェック
     * @param timeProvider 時刻プロバイダ
     * @return true: 最小時刻以前, false: 最小時刻以降
     */
    static bool isSystemTimeBeforeMinimum(ITimeProvider* timeProvider);
    
    /**
     * システム時刻を最小時刻に補正
     * @param timeProvider 時刻プロバイダ
     * @return true: 補正成功, false: 補正失敗
     */
    static bool correctSystemTimeToMinimum(ITimeProvider* timeProvider);
    
    /**
     * 最小時刻のtime_t値を取得
     * @return 最小時刻のtime_t値
     */
    static time_t getMinimumSystemTime();
    
    /**
     * システム時刻の検証と必要に応じた補正を実行
     * @param timeProvider 時刻プロバイダ
     * @return true: 補正が実行された, false: 補正不要または補正失敗
     */
    static bool validateAndCorrectSystemTime(ITimeProvider* timeProvider);
};