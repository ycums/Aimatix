#ifndef TIME_LOGIC_H
#define TIME_LOGIC_H

#include <vector>
#include <ctime>

// 純粋な時刻計算ロジック（ハードウェア依存なし）
class TimeLogic {
public:
    // 時刻バリデーション
    static bool isValidTime(int hour, int minute);
    
    // 相対時刻計算
    static time_t calculateRelativeTime(int base_hour, int base_min, int add_hour, int add_min, bool is_plus);
    
    // 絶対時刻計算
    static time_t calculateAbsoluteTime(int hour, int minute);
    
    // アラーム時刻の計算（モード別）
    static time_t calculateAlarmTime(int input_hour, int input_min, int mode, time_t current_time);
    
    // 時刻フォーマット
    static void formatTime(time_t time, int& hour, int& minute);
    static void formatTimeString(time_t time, char* buffer, size_t buffer_size);
    static time_t getCurrentTime() { return time(NULL); }
};

// アラーム管理ロジック（ハードウェア依存なし）
class AlarmLogic {
public:
    // アラーム追加
    static bool addAlarm(std::vector<time_t>& alarms, time_t alarm_time);
    
    // アラーム削除
    static bool removeAlarm(std::vector<time_t>& alarms, time_t alarm_time);
    
    // 重複チェック
    static bool isDuplicateAlarm(const std::vector<time_t>& alarms, time_t alarm_time);
    
    // 最大数チェック
    static bool canAddAlarm(const std::vector<time_t>& alarms, int max_count = 5);
    
    // ソート
    static void sortAlarms(std::vector<time_t>& alarms);
    
    // 過去のアラーム削除
    static void removePastAlarms(std::vector<time_t>& alarms, time_t current_time);
    
    // 次のアラーム取得
    static time_t getNextAlarmTime(const std::vector<time_t>& alarms, time_t current_time);
};

// 入力値管理ロジック（ハードウェア依存なし）
class InputLogic {
public:
    // 桁ごと編集の値更新
    static void incrementDigit(int& digit, int max_value, int increment = 1);
    static void decrementDigit(int& digit, int max_value, int decrement = 1);
    
    // 時刻の桁制約チェック
    static bool isValidHourTens(int tens, int ones);
    static bool isValidHourOnes(int tens, int ones);
    static bool isValidMinTens(int tens);
    static bool isValidMinOnes(int ones);
    
    // 入力値から時刻への変換
    static void inputToTime(int hour_tens, int hour_ones, int min_tens, int min_ones, int& hour, int& minute);
    static void timeToInput(int hour, int minute, int& hour_tens, int& hour_ones, int& min_tens, int& min_ones);
};

#endif // TIME_LOGIC_H 