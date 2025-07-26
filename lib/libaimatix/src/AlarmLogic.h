#pragma once
#include <vector>
#include <ctime>
#include <string>

class AlarmLogic {
public:
    // アラームリストの初期化（+10秒, +30秒, +1分, +2分）
    static void initAlarms(std::vector<time_t>& alarms, time_t now);
    // 過去アラームの消化
    static void removePastAlarms(std::vector<time_t>& alarms, time_t now);
    // 残り時間（秒）を返す（次のアラームまで）
    static int getRemainSec(const std::vector<time_t>& alarms, time_t now);
    // 残り割合（0-100）を返す（右から左へ縮むバー用）
    static int getRemainPercent(int remainSec, int totalSec);
    // アラームリストの時刻文字列を取得
    static void getAlarmTimeStrings(const std::vector<time_t>& alarms, std::vector<std::string>& out);

    enum class AddAlarmResult {
        Success,
        ErrorEmptyInput,
        ErrorMaxReached,
        ErrorDuplicate,
        ErrorInvalid,
    };
    // 入力値（時刻: time_t）をアラームとして追加。エラー時はresult, errorMsgに理由を格納。
    static bool addAlarm(std::vector<time_t>& alarms, time_t now, time_t input, AddAlarmResult& result, std::string& errorMsg);
    
    // 部分的な入力状態（digits[4], entered[4]）からアラームを追加
    static bool addAlarmFromPartialInput(
        std::vector<time_t>& alarms, 
        time_t now, 
        const int* digits, 
        const bool* entered, 
        AddAlarmResult& result, 
        std::string& errorMsg
    );
}; 