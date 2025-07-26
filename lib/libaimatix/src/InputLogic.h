#pragma once

class InputLogic {
public:
    static constexpr int EMPTY_VALUE = -1;
    static constexpr int LAST_VALUE_INIT = -2;
    InputLogic() { reset(); }
    virtual int getValue() const {
        for(int i=0;i<4;++i) if(!entered[i]) return EMPTY_VALUE;
        return digits[0]*1000 + digits[1]*100 + digits[2]*10 + digits[3];
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
        // 全桁入力済みなら拒絶
        bool allEntered = true;
        for (int i = 0; i < 4; ++i) {
            if (!entered[i]) {
                allEntered = false;
                break;
            }
        }
        if (allEntered) return false;
        
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
        for (int i = 0; i < 3; ++i) {
            digits[i] = tempDigits[i + 1];
            entered[i] = tempEntered[i + 1];
        }
        digits[3] = 0;
        entered[3] = false;
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
}; 