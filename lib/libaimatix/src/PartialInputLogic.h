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
        
        ParsedTime() = default;
        ParsedTime(int h, int m, bool valid = true) : hour(h), minute(m), isValid(valid) {}
    };
    
    /**
     * 部分入力状態から時分を解析
     * バグ修正済みのロジック：分一桁のみ入力時はその値を分十桁として解釈
     * 
     * @param digits 各桁の値配列[4] (時十,時一,分十,分一)
     * @param entered 各桁の入力済みフラグ配列[4]
     * @return 解析された時分
     */
    static ParsedTime parsePartialInput(const int* digits, const bool* entered);
    
    /**
     * 時分を HH:MM 形式の文字列にフォーマット
     * 
     * @param hour 時（0-23）
     * @param minute 分（0-59）
     * @return フォーマットされた文字列
     */
    static std::string formatTime(int hour, int minute);
    
    /**
     * 時分の妥当性をチェック
     * 
     * @param hour 時
     * @param minute 分
     * @return true: 有効, false: 無効
     */
    static bool isValidTime(int hour, int minute);

private:
    static constexpr int HOURS_10 = 10;
    static constexpr int HOURS_24 = 24;
    static constexpr int MINUTES_60 = 60;
};