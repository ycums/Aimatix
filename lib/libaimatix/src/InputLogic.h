#pragma once
#include <cassert>
#include <memory>
#include "ITimeProvider.h"

class InputLogic {
public:
    static constexpr int EMPTY_VALUE = -1;
    static constexpr int LAST_VALUE_INIT = -2;
    InputLogic(std::shared_ptr<ITimeProvider> timeProvider)
        : timeProvider_(timeProvider) {
        assert(timeProvider_ && "ITimeProvider must not be nullptr");
        reset();
    }
    // 現在の入力値を取得（全桁入力済みの場合のみ有効な値を返す）
    virtual int getValue() const {
        bool allEntered = true;
        for (int i = 0; i < 4; ++i) {
            if (!entered[i]) {
                allEntered = false;
                break;
            }
        }
        if (!allEntered) return EMPTY_VALUE;
        
        return digits[0] * 1000 + digits[1] * 100 + digits[2] * 10 + digits[3];
    }
    
    // 相対値として現在の入力値を絶対時刻に変換
    virtual time_t getAbsoluteValue() const {
        int inputValue = 0;
        bool hasInput = false;
        for (int i = 0; i < 4; ++i) {
            if (entered[i]) {
                inputValue = inputValue * 10 + digits[i];
                hasInput = true;
            }
        }
        if (!hasInput) return -1; // 未入力
        int inputHour = inputValue / 100;
        int inputMinute = inputValue % 100;
        time_t now = timeProvider_->now();
        struct tm* tm_now = timeProvider_->localtime(&now);
        struct tm alarm_tm = *tm_now;
        alarm_tm.tm_sec = tm_now->tm_sec; // 現在時刻の秒を保持
        alarm_tm.tm_isdst = -1;
        if (inputMinute >= 60) { inputHour += inputMinute / 60; inputMinute = inputMinute % 60; }
        int add_day = inputHour / 24;
        inputHour = inputHour % 24;
        alarm_tm.tm_hour += inputHour;
        alarm_tm.tm_min += inputMinute;
        alarm_tm.tm_mday += add_day;
        return mktime(&alarm_tm);
    }
    

    virtual void reset() {
        for(int i=0;i<4;++i) { digits[i]=0; entered[i]=false; }
    }
    // 右端（分一の位）に値を入力
    virtual void incrementInput(int amount) {
        if (!entered[3]) {
            // 未入力なら値を設定
            digits[3] = amount % 10;
            entered[3] = true;
        } else {
            // 入力済みなら値を加算
            digits[3] = (digits[3] + amount) % 10;
        }
    }
    // 桁送り機能（入力済みの値を左シフト）
    virtual bool shiftDigits() {
        // 全桁入力済みなら拒絶（厳密に）
        bool allEntered = true;
        for (int i = 0; i < 4; ++i) {
            if (!entered[i]) {
                allEntered = false;
                break;
            }
        }
        if (allEntered) return false;
        
        // 左端が入力済みなら拒絶（データロス防止）
        if (entered[0]) return false;
        
        // 入力済みの値を左にシフト
        bool hasInput = false;
        for (int i = 0; i < 4; ++i) {
            if (entered[i]) { hasInput = true; break; }
        }
        if (!hasInput) return false;
        
        int tempDigits[4];
        bool tempEntered[4];
        for (int i = 0; i < 4; ++i) {
            tempDigits[i] = digits[i];
            tempEntered[i] = entered[i];
        }
        // 左シフト（digits[0-2], entered[0-2]）
        for (int i = 0; i < 3; ++i) {
            digits[i] = tempDigits[i + 1];
            entered[i] = tempEntered[i + 1];
        }
        // 右端のみ0/未入力にする
        digits[3] = 0;
        entered[3] = false;
        // 右から2番目（digits[2]）は0だが入力済みとする
        if (digits[2] == 0) {
            entered[2] = true;
        }
        return true;
    }
    // 各桁の値・入力済み状態を取得
    int getDigit(int idx) const { return digits[idx]; }
    bool isEntered(int idx) const { return entered[idx]; }
    const int* getDigits() const { return digits; }
    const bool* getEntered() const { return entered; }
protected:
    int digits[4]; // 各桁の値
    bool entered[4]; // 各桁の入力済みフラグ
private:
    std::shared_ptr<ITimeProvider> timeProvider_;
}; 