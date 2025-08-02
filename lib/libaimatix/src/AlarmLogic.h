#pragma once
#include <vector>
#include <ctime>
#include <string>

class AlarmLogic {
public:
    // アラームリストの初期化（+10秒, +30秒, +1分, +2分）
    static auto initAlarms(std::vector<time_t>& alarms, time_t now) -> void;
    // 過去アラームの消化
    static auto removePastAlarms(std::vector<time_t>& alarms, time_t now) -> void;
    // 残り時間（秒）を返す（次のアラームまで）
    static auto getRemainSec(const std::vector<time_t>& alarms, time_t now) -> int;
    // 残り割合（0-100）を返す（右から左へ縮むバー用）
    static auto getRemainPercent(int remainSec, int totalSec) -> int;
    // アラームリストの時刻文字列を取得
    static auto getAlarmTimeStrings(const std::vector<time_t>& alarms, std::vector<std::string>& out) -> void;

    enum class AddAlarmResult {
        Success,
        ErrorEmptyInput,
        ErrorMaxReached,
        ErrorDuplicate,
        ErrorInvalid,
    };
    // 入力値（時刻: time_t）をアラームとして追加。エラー時はresult, errorMsgに理由を格納。
    static auto addAlarm(std::vector<time_t>& alarms, time_t now, time_t input, AddAlarmResult& result, std::string& errorMsg) -> bool;
    
    // 絶対時刻（time_t）をアラームとして追加。エラー時はresult, errorMsgに理由を格納。
    static auto addAlarmAtTime(std::vector<time_t>& alarms, time_t alarmTime, AddAlarmResult& result, std::string& errorMsg) -> bool;
    
    // 部分的な入力状態（digits[4], entered[4]）からアラームを追加
    static auto addAlarmFromPartialInput(
        std::vector<time_t>& alarms, 
        time_t now, 
        const int* digits, 
        const bool* entered, 
        AddAlarmResult& result, 
        std::string& errorMsg
    ) -> bool;
    
    // 指定インデックスのアラームを削除
    static auto deleteAlarm(std::vector<time_t>& alarms, size_t index) -> bool;
    
    // アラームリストを取得（時刻順でソート済み）
    static auto getAlarms(const std::vector<time_t>& alarms) -> std::vector<time_t>;
}; 