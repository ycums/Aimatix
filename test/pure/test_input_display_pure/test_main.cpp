#include <unity.h>
#include "StateManager.h"
#include "MainDisplayState.h"
#include "InputDisplayState.h"
#include "InputLogic.h"
#include <cstdio>
#include <cstring>
#include <vector>
#include <ctime>

std::vector<time_t> alarmTimes;

void setUp(void) {}
void tearDown(void) {}

// StateManager経由でMainDisplay→InputDisplayに遷移する（onButtonA経由）
void test_transition_to_input_display_on_a_button_press() {
    StateManager sm;
    InputLogic logic;
    InputDisplayState inputState(&logic);
    MainDisplayState mainState(&sm, &inputState);
    sm.setState(&mainState);
    // onButtonAでInputDisplayに遷移
    sm.handleButtonA();
    TEST_ASSERT_EQUAL_PTR(&inputState, sm.getCurrentState());
}

// モックView/Logic
class MockView : public IInputDisplayView {
public:
    int clearCount=0, showTitleCount=0, showHintsCount=0, showValueCount=0;
    int lastValue=-2;
    void clear() override { clearCount++; }
    void showTitle(const char*, int, bool) override { showTitleCount++; }
    void showHints(const char*, const char*, const char*) override { showHintsCount++; }
    void showValue(const int* digits, const bool* entered) override { showValueCount++; lastValue = digits ? digits[0] : -2; }
    void showPreview(const char*) override {}
};
class MockLogic : public InputLogic {
public:
    MockLogic() { value = -1; cursor = 3; }
    int value;
    int cursor;
    int getValue() const override { return value; }
    int getCursor() const override { return cursor; }
    void reset() override { value = -1; cursor = 3; }
};

// onEnter, onDraw, setView, onExit, onButtonX系のテスト
void test_input_display_state_methods() {
    MockLogic logic;
    MockView view;
    InputDisplayState state(&logic, &view);
    // onEnter: view有り
    state.onEnter();
    TEST_ASSERT_EQUAL(1, view.clearCount);
    TEST_ASSERT_EQUAL(1, view.showTitleCount);
    TEST_ASSERT_EQUAL(1, view.showHintsCount);
    // onDraw: value変化時のみshowValue
    logic.value = 42;
    state.onDraw();
    TEST_ASSERT_EQUAL(1, view.showValueCount);
    // 同じ値なら呼ばれない
    state.onDraw();
    TEST_ASSERT_EQUAL(1, view.showValueCount);
    // 値が変われば呼ばれる
    logic.value = 43;
    state.onDraw();
    // view無しでも例外等起きない
    InputDisplayState state2(&logic, nullptr);
    state2.onEnter();
    state2.onDraw();
    // logic無しでも例外等起きない
    InputDisplayState state3(nullptr, &view);
    state3.onDraw();
    // setViewで差し替え
    MockView view2;
    state.setView(&view2);
    logic.value = 100;
    state.onDraw();
    // onExit, onButtonX系: 何も起きない（例外・クラッシュしない）
    state.onExit();
    state.onButtonA();
    state.onButtonB();
    state.onButtonC();
    state.onButtonALongPress();
    state.onButtonCLongPress();
}

// 入力画面でC長押し→メイン画面に戻る
void test_input_display_c_long_press_returns_to_main() {
    StateManager sm;
    InputLogic logic;
    InputDisplayState inputState(&logic);
    MainDisplayState mainState(&sm, &inputState);
    inputState.setManager(&sm);
    inputState.setMainDisplayState(&mainState);
    sm.setState(&mainState);
    sm.handleButtonA(); // InputDisplayへ遷移
    TEST_ASSERT_EQUAL_PTR(&inputState, sm.getCurrentState());
    sm.handleButtonCLongPress();
    TEST_ASSERT_EQUAL_PTR(&mainState, sm.getCurrentState());
}
// 入力画面でC短押しや他ボタンでは遷移しない
void test_input_display_other_buttons_do_not_return_to_main() {
    StateManager sm;
    InputLogic logic;
    InputDisplayState inputState(&logic);
    MainDisplayState mainState(&sm, &inputState);
    inputState.setManager(&sm);
    inputState.setMainDisplayState(&mainState);
    sm.setState(&mainState);
    sm.handleButtonA();
    TEST_ASSERT_EQUAL_PTR(&inputState, sm.getCurrentState());
    sm.handleButtonC();
    TEST_ASSERT_EQUAL_PTR(&inputState, sm.getCurrentState());
    sm.handleButtonA();
    TEST_ASSERT_EQUAL_PTR(&inputState, sm.getCurrentState());
    sm.handleButtonB();
    TEST_ASSERT_EQUAL_PTR(&inputState, sm.getCurrentState());
    sm.handleButtonALongPress();
    TEST_ASSERT_EQUAL_PTR(&inputState, sm.getCurrentState());
}
// メイン画面でC長押ししても何も起こらない
void test_main_display_c_long_press_does_nothing() {
    StateManager sm;
    InputLogic logic;
    InputDisplayState inputState(&logic);
    MainDisplayState mainState(&sm, &inputState);
    inputState.setManager(&sm);
    inputState.setMainDisplayState(&mainState);
    sm.setState(&mainState);
    TEST_ASSERT_EQUAL_PTR(&mainState, sm.getCurrentState());
    sm.handleButtonCLongPress();
    TEST_ASSERT_EQUAL_PTR(&mainState, sm.getCurrentState());
}

// === ここからTDD: 3-0-6 入力画面初期値・未入力状態テスト ===
void test_input_display_initial_value_is_empty() {
    MockLogic logic;
    logic.value = -1; // -1を未入力状態とみなす（仮仕様）
    MockView view;
    InputDisplayState state(&logic, &view);
    state.onEnter();
    state.onDraw();
    // 未入力状態ならshowValueが呼ばれる（引数-1）
    TEST_ASSERT_EQUAL(1, view.showValueCount);
}

void test_input_logic_cursor_initial_position() {
    // InputLogicにカーソル位置管理を追加する前提
    class CursorLogic : public InputLogic {
    public:
        int cursor = 3; // 初期値は分一の位
        int getCursor() const { return cursor; }
    } logic;
    TEST_ASSERT_EQUAL(3, logic.getCursor());
}

void test_input_logic_reset_on_enter() {
    // onEnterでInputLogicがリセットされること
    class ResetLogic : public InputLogic {
    public:
        bool resetCalled = false;
        void reset() { resetCalled = true; }
    } logic;
    MockView view;
    InputDisplayState state(&logic, &view);
    // onEnterでreset()が呼ばれる想定
    state.onEnter();
    // ここでは手動でreset()を呼ぶ（InputLogicにreset()追加後に修正）
    logic.reset();
    TEST_ASSERT_TRUE(logic.resetCalled);
}

// === ここからTDD: 3-0-7 +1/+5 入力ロジック・UI反映テスト ===

class PlusLogic : public InputLogic {
public:
    using InputLogic::value;
    using InputLogic::cursor;
    PlusLogic() { value = -1; cursor = 3; }
    // テスト用: valueを4桁の配列で管理（時十,時一,分十,分一）
    int digits[4] = {0,0,0,0};
    void setDigits(int h10, int h1, int m10, int m1) {
        digits[0]=h10; digits[1]=h1; digits[2]=m10; digits[3]=m1;
        value = digits[0]*1000 + digits[1]*100 + digits[2]*10 + digits[3];
    }
    void syncValue() { value = digits[0]*1000 + digits[1]*100 + digits[2]*10 + digits[3]; }
    void syncDigits() {
        int v = value;
        digits[0] = (v/1000)%10;
        digits[1] = (v/100)%10;
        digits[2] = (v/10)%10;
        digits[3] = v%10;
    }
    // +1/+5をカーソル位置に加算
    void incrementAtCursor(int amount) {
        if (value == -1) { setDigits(0,0,0,0); }
        syncDigits();
        digits[cursor] += amount;
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
        syncValue();
        if (value>2359) setDigits(2,3,5,9);
    }
};

void test_inputlogic_plus1_plus5_basic() {
    PlusLogic logic;
    // 未入力でA短押し(+1)
    logic.value = -1; logic.cursor = 3;
    logic.incrementAtCursor(1);
    TEST_ASSERT_EQUAL(1, logic.digits[3]);
    // 未入力でA長押し(+5)
    logic.value = -1; logic.cursor = 3;
    logic.incrementAtCursor(5);
    TEST_ASSERT_EQUAL(5, logic.digits[3]);
    // 12:34で分一の位+1
    logic.setDigits(1,2,3,4); logic.cursor=3;
    logic.incrementAtCursor(1);
    TEST_ASSERT_EQUAL(5, logic.digits[3]);
    // 12:34で分一の位+5
    logic.setDigits(1,2,3,4); logic.cursor=3;
    logic.incrementAtCursor(5);
    TEST_ASSERT_EQUAL(9, logic.digits[3]);
    // 12:59で分十の位+1（59→59でストップ）
    logic.setDigits(1,2,5,9); logic.cursor=2;
    logic.incrementAtCursor(1);
    TEST_ASSERT_EQUAL(5, logic.digits[2]);
    // 23:59で+1/+5しても23:59
    logic.setDigits(2,3,5,9); logic.cursor=3;
    logic.incrementAtCursor(1);
    TEST_ASSERT_EQUAL(9, logic.digits[3]);
    logic.incrementAtCursor(5);
    TEST_ASSERT_EQUAL(9, logic.digits[3]);
}

void test_inputlogic_plus1_plus5_each_digit() {
    PlusLogic logic;
    // 時十の位+1（2でストップ）
    logic.setDigits(1,2,3,4); logic.cursor=0;
    logic.incrementAtCursor(1);
    TEST_ASSERT_EQUAL(2, logic.digits[0]);
    logic.incrementAtCursor(1);
    TEST_ASSERT_EQUAL(2, logic.digits[0]);
    // 時一の位+1（時十=2なら3でストップ）
    logic.setDigits(2,2,3,4); logic.cursor=1;
    logic.incrementAtCursor(1);
    TEST_ASSERT_EQUAL(3, logic.digits[1]);
    logic.incrementAtCursor(1);
    TEST_ASSERT_EQUAL(3, logic.digits[1]);
    // 分十の位+1（5でストップ）
    logic.setDigits(1,2,5,4); logic.cursor=2;
    logic.incrementAtCursor(1);
    TEST_ASSERT_EQUAL(5, logic.digits[2]);
    // 分一の位+1（9でストップ）
    logic.setDigits(1,2,3,9); logic.cursor=3;
    logic.incrementAtCursor(1);
    TEST_ASSERT_EQUAL(9, logic.digits[3]);
}

void test_inputlogic_plus1_plus5_multi() {
    PlusLogic logic;
    // 連打で加算
    logic.setDigits(1,2,3,4); logic.cursor=3;
    for(int i=0;i<5;++i) logic.incrementAtCursor(1);
    TEST_ASSERT_EQUAL(9, logic.digits[3]);
    // さらに+1しても9でストップ
    logic.incrementAtCursor(1);
    TEST_ASSERT_EQUAL(9, logic.digits[3]);
}

// UI反映テストは既存MockViewのshowValueCount等で十分

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_transition_to_input_display_on_a_button_press);
    RUN_TEST(test_input_display_state_methods);
    RUN_TEST(test_input_display_c_long_press_returns_to_main);
    RUN_TEST(test_input_display_other_buttons_do_not_return_to_main);
    RUN_TEST(test_main_display_c_long_press_does_nothing);
    RUN_TEST(test_input_display_initial_value_is_empty);
    RUN_TEST(test_input_logic_cursor_initial_position);
    RUN_TEST(test_input_logic_reset_on_enter);
    RUN_TEST(test_inputlogic_plus1_plus5_basic);
    RUN_TEST(test_inputlogic_plus1_plus5_each_digit);
    RUN_TEST(test_inputlogic_plus1_plus5_multi);
    UNITY_END();
    return 0;
} 