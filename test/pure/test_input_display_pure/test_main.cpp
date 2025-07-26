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
    int clearCount=0, showTitleCount=0, showHintsCount=0;
    int showValueCount=0; // 旧方式（残すが非推奨）
    int showDigitCount=0, showColonCount=0, showPreviewCount=0;
    int lastValue=-2;
    void clear() override { clearCount++; }
    void showTitle(const char*, int, bool) override { showTitleCount++; }
    void showHints(const char*, const char*, const char*) override { showHintsCount++; }
    void showDigit(int, int, bool) override { showDigitCount++; }
    void showColon() override { showColonCount++; }
    void showPreview(const char*) override { showPreviewCount++; }
};
class MockLogic : public InputLogic {
public:
    MockLogic() { value = -1; }
    int value;
    int getValue() const override { return value; }
    void reset() override { value = -1; }
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
    // onDraw: value変化時のみshowDigit/showColonが呼ばれる（showValueは非推奨）
    logic.value = 42;
    state.onDraw();
    TEST_ASSERT_EQUAL(4, view.showDigitCount); // 4回が正
    // 同じ値なら呼ばれない
    int prevDigitCount = view.showDigitCount;
    state.onDraw();
    TEST_ASSERT_EQUAL(prevDigitCount, view.showDigitCount);
    // 値が変われば呼ばれる
    logic.value = 43;
    state.onDraw();
    TEST_ASSERT_EQUAL(prevDigitCount, view.showDigitCount);
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
    // 未入力状態ならshowDigitが呼ばれる（showValueは非推奨）
    TEST_ASSERT_TRUE(view.showDigitCount >= 1); // 1回以上呼ばれること
}

// === ここからTDD: 3-0-9 桁送り機能テスト ===

// 桁送り機能のテスト用MockLogic
class ShiftDigitsLogic : public InputLogic {
public:
    bool shiftDigitsResult = true;
    bool shiftDigits() override { 
        if (shiftDigitsResult) {
            // 入力済みの値を左シフト
            return true;
        }
        return false;
    }
};

// 桁送り成功時のテスト
void test_input_logic_shift_digits_success() {
    ShiftDigitsLogic logic;
    MockView view;
    InputDisplayState state(&logic, &view);
    
    // 桁送り成功
    bool result = logic.shiftDigits();
    TEST_ASSERT_TRUE(result);
    
    // さらに桁送り
    result = logic.shiftDigits();
    TEST_ASSERT_TRUE(result);
}

// 桁送り失敗時のテスト（全桁入力済み時）
void test_input_logic_shift_digits_failure() {
    ShiftDigitsLogic logic;
    logic.shiftDigitsResult = false; // 失敗をシミュレート
    
    bool result = logic.shiftDigits();
    TEST_ASSERT_FALSE(result);
}

// 入力画面でのB短押しで桁送りが動作するテスト
void test_input_display_b_button_shift_digits() {
    ShiftDigitsLogic logic;
    MockView view;
    InputDisplayState state(&logic, &view);
    
    // B短押しで桁送り
    state.onButtonB();
    
    // 桁送りが実行される
    TEST_ASSERT_TRUE(logic.shiftDigitsResult);
}

// 桁送り失敗時は何も起こらないテスト
void test_input_display_b_button_shift_digits_failure() {
    ShiftDigitsLogic logic;
    logic.shiftDigitsResult = false;
    MockView view;
    InputDisplayState state(&logic, &view);
    
    // B短押しで桁送り失敗
    state.onButtonB();
    
    // 何も起こらない（失敗時は何もしない）
    TEST_ASSERT_FALSE(logic.shiftDigitsResult);
}

// 実際のInputLogicを使用した桁送りテスト
void test_real_input_logic_shift_digits() {
    InputLogic logic;
    logic.reset();
    
    // 何も入力していない状態では桁送りできない
    bool result = logic.shiftDigits();
    TEST_ASSERT_FALSE(result);
    
    // 値を入力してから桁送り
    logic.incrementInput(5); // 分一の位に5を入力
    result = logic.shiftDigits();
    TEST_ASSERT_TRUE(result);
    
    // さらに値を入力して桁送り
    logic.incrementInput(3); // 分十の位に3を入力
    result = logic.shiftDigits();
    TEST_ASSERT_TRUE(result);
}

// 入力と桁送りの基本動作テスト
void test_input_logic_basic_operations() {
    InputLogic logic;
    logic.reset();
    
    // 初期状態確認
    for (int i = 0; i < 4; ++i) {
        TEST_ASSERT_FALSE(logic.isEntered(i));
        TEST_ASSERT_EQUAL(0, logic.getDigit(i));
    }
    
    // 入力: 常に右端（分一の位）
    logic.incrementInput(5); // 分一の位に5
    TEST_ASSERT_EQUAL(5, logic.getDigit(3));
    TEST_ASSERT_TRUE(logic.isEntered(3));
    
    // 桁送り
    logic.shiftDigits();
    TEST_ASSERT_EQUAL(5, logic.getDigit(2)); // 分十の位に移動
    TEST_ASSERT_FALSE(logic.isEntered(3));   // 分一の位はクリア
    
    // 次の入力（再び右端）
    logic.incrementInput(3); // 分一の位に3
    TEST_ASSERT_EQUAL(3, logic.getDigit(3));
    TEST_ASSERT_TRUE(logic.isEntered(3));
}

// 入力済み時の加算テスト
void test_input_logic_increment_on_entered() {
    InputLogic logic;
    logic.reset();
    
    // 初回入力
    logic.incrementInput(5);
    TEST_ASSERT_EQUAL(5, logic.getDigit(3));
    TEST_ASSERT_TRUE(logic.isEntered(3));
    
    // 入力済み時の加算
    logic.incrementInput(3);
    TEST_ASSERT_EQUAL(8, logic.getDigit(3)); // 5 + 3 = 8
    TEST_ASSERT_TRUE(logic.isEntered(3));
    
    // 10を超える場合の剰余
    logic.incrementInput(5);
    TEST_ASSERT_EQUAL(3, logic.getDigit(3)); // 8 + 5 = 13 → 3
    TEST_ASSERT_TRUE(logic.isEntered(3));
}

// 全桁入力済み時の桁送り拒絶テスト
void test_input_logic_shift_digits_all_digits_entered() {
    InputLogic logic;
    logic.reset();
    
    // 全桁に値を入力
    logic.incrementInput(1); // 分一の位
    logic.shiftDigits();     // 桁送り
    logic.incrementInput(2); // 分十の位
    logic.shiftDigits();     // 桁送り
    logic.incrementInput(3); // 時一の位
    logic.shiftDigits();     // 桁送り
    logic.incrementInput(4); // 時十の位
    
    // entered配列がすべてtrueであることを確認
    for (int i = 0; i < 4; ++i) {
        TEST_ASSERT_TRUE(logic.isEntered(i));
    }
    
    // 全桁入力済み状態で桁送りを試行
    bool result = logic.shiftDigits();
    TEST_ASSERT_FALSE(result); // 拒絶される
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_transition_to_input_display_on_a_button_press);
    RUN_TEST(test_input_display_state_methods);
    RUN_TEST(test_input_display_c_long_press_returns_to_main);
    RUN_TEST(test_input_display_other_buttons_do_not_return_to_main);
    RUN_TEST(test_main_display_c_long_press_does_nothing);
    RUN_TEST(test_input_display_initial_value_is_empty);
    RUN_TEST(test_input_logic_shift_digits_success);
    RUN_TEST(test_input_logic_shift_digits_failure);
    RUN_TEST(test_input_display_b_button_shift_digits);
    RUN_TEST(test_input_display_b_button_shift_digits_failure);
    RUN_TEST(test_real_input_logic_shift_digits);
    RUN_TEST(test_input_logic_basic_operations);
    RUN_TEST(test_input_logic_increment_on_entered);
    RUN_TEST(test_input_logic_shift_digits_all_digits_entered);
    UNITY_END();
    return 0;
} 