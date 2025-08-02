#pragma once
#include <string>

/**
 * 部分入力ロジック
 * 絶対時刻入力や相対時刻入力での部分入力状態から時分を解釈する共通ロジック
 */
class PartialInputLogic {
public:
    /**
     * 部分入力解析結果
     */
    struct ParsedTime {
        int hour = 0;        // 時（0-23）
        int minute = 0;      // 分（0-59）
        bool isValid = true; // 解析結果が有効かどうか
        bool hourSpecified = false;  // 時が指定されているか
        bool minuteSpecified = false; // 分が指定されているか
        
        ParsedTime() = default;
        ParsedTime(int h, int m, bool valid = true, bool hSpec = false, bool mSpec = false) 
            : hour(h), minute(m), isValid(valid), hourSpecified(hSpec), minuteSpecified(mSpec) {}
    };
    
    /**
     * 部分入力状態から時分を解析
     * 分一桁のみ入力時はその値を分一桁として解釈（直感的な動作）
     * 
     * @param digits 各桁の値配列[4] (時十,時一,分十,分一)
     * @param entered 各桁の入力済みフラグ配列[4]
     * @return 解析された時分
     */
    static auto parsePartialInput(const int* digits, const bool* entered) -> ParsedTime;
    
    /**
     * 時分を HH:MM 形式の文字列にフォーマット
     * 
     * @param hour 時（0-23）
     * @param minute 分（0-59）
     * @return フォーマットされた文字列
     */
    static auto formatTime(int hour, int minute) -> std::string;
    
    /**
     * 時分の妥当性をチェック
     * 
     * @param hour 時
     * @param minute 分
     * @return true: 有効, false: 無効
     */
    static auto isValidTime(int hour, int minute) -> bool;

private:
    static constexpr int HOURS_10 = 10;
    static constexpr int HOURS_24 = 24;
    static constexpr int MINUTES_60 = 60;
    static constexpr int DIGITS_COUNT = 4;
    static constexpr int STRING_BUFFER_SIZE = 32;
};