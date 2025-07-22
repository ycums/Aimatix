#pragma once

class InputLogic {
public:
    static constexpr int EMPTY_VALUE = -1;
    static constexpr int LAST_VALUE_INIT = -2;
    static constexpr int CURSOR_INITIAL = 3;
    InputLogic() { reset(); }
    virtual int getValue() const { return value; }
    virtual int getCursor() const { return cursor; }
    virtual void reset() { value = EMPTY_VALUE; cursor = CURSOR_INITIAL; }
protected:
    int value;   // -1: 未入力, 0以上: 入力済み値
    int cursor;  // 0=時十, 1=時一, 2=分十, 3=分一
}; 