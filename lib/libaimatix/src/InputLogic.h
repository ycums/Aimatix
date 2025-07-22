#pragma once

class InputLogic {
public:
    static constexpr int EMPTY_VALUE = -1;
    static constexpr int LAST_VALUE_INIT = -2;
    static constexpr int CURSOR_INITIAL = 3;
    InputLogic() { reset(); }
    virtual int getValue() const {
        for(int i=0;i<4;++i) if(!entered[i]) return EMPTY_VALUE;
        return digits[0]*1000 + digits[1]*100 + digits[2]*10 + digits[3];
    }
    virtual int getCursor() const { return cursor; }
    virtual void reset() {
        for(int i=0;i<4;++i) { digits[i]=0; entered[i]=false; }
        cursor = CURSOR_INITIAL;
    }
    virtual void incrementAtCursor(int amount) {
        if (!entered[cursor]) {
            digits[cursor] = amount;
            entered[cursor] = true;
        } else {
            digits[cursor] += amount;
        }
        // 桁ごとの上限
        if (cursor==0 && digits[0]>2) digits[0]=2;
        if (cursor==1 && (digits[0]==2 && digits[1]>3)) digits[1]=3;
        if (cursor==1 && digits[1]>9) digits[1]=9;
        if (cursor==2 && digits[2]>5) digits[2]=5;
        if (cursor==3 && digits[3]>9) digits[3]=9;
        // 23:59超えたら23:59でストップ
        int h = digits[0]*10+digits[1];
        int m = digits[2]*10+digits[3];
        if (h>23) { digits[0]=2; digits[1]=3; }
        if (m>59) { digits[2]=5; digits[3]=9; }
        if (digits[0]*1000+digits[1]*100+digits[2]*10+digits[3]>2359) {
            digits[0]=2; digits[1]=3; digits[2]=5; digits[3]=9;
        }
        entered[cursor] = true;
    }
    // 各桁の値・入力済み状態を取得
    int getDigit(int idx) const { return digits[idx]; }
    bool isEntered(int idx) const { return entered[idx]; }
    const int* getDigits() const { return digits; }
    const bool* getEntered() const { return entered; }
protected:
    int digits[4]; // 各桁の値
    bool entered[4]; // 各桁の入力済みフラグ
    int cursor;  // 0=時十, 1=時一, 2=分十, 3=分一
}; 