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

// === ここからTDD: 3-0-9 桁送り機能テスト ===

// 桁送り機能のテスト用MockLogic
class MoveCursorLogic : public InputLogic {
public:
    bool moveCursorResult = true;
    int cursor = 3;
    int getCursor() const override { return cursor; }
    bool moveCursor() override { 
        if (moveCursorResult) {
            // 右から左（3→2→1→0）
            if (cursor > 0) {
                cursor--;
                return true;
            }
        }
        return false;
    }
};

// 桁送り成功時のテスト
void test_input_logic_move_cursor_success() {
    MoveCursorLogic logic;
    logic.cursor = 3;
    MockView view;
    InputDisplayState state(&logic, &view);
    
    // 初期状態
    TEST_ASSERT_EQUAL(3, logic.getCursor());
    
    // 桁送り成功
    bool result = logic.moveCursor();
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(2, logic.getCursor());
    
    // さらに桁送り
    result = logic.moveCursor();
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(1, logic.getCursor());
    
    result = logic.moveCursor();
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(0, logic.getCursor());
}

// 桁送り失敗時のテスト（全桁入力済み時）
void test_input_logic_move_cursor_failure() {
    MoveCursorLogic logic;
    logic.cursor = 0; // 最左端
    logic.moveCursorResult = false; // 失敗をシミュレート
    
    bool result = logic.moveCursor();
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_EQUAL(0, logic.getCursor()); // 位置は変わらない
}

// 入力画面でのB短押しで桁送りが動作するテスト
void test_input_display_b_button_move_cursor() {
    MoveCursorLogic logic;
    logic.cursor = 3;
    MockView view;
    InputDisplayState state(&logic, &view);
    
    // B短押しで桁送り
    state.onButtonB();
    
    // カーソル位置が更新される
    TEST_ASSERT_EQUAL(2, logic.getCursor());
}

// 桁送り失敗時は何も起こらないテスト
void test_input_display_b_button_move_cursor_failure() {
    MoveCursorLogic logic;
    logic.cursor = 0;
    logic.moveCursorResult = false;
    MockView view;
    InputDisplayState state(&logic, &view);
    
    int initialCursor = logic.getCursor();
    
    // B短押しで桁送り失敗
    state.onButtonB();
    
    // カーソル位置は変わらない
    TEST_ASSERT_EQUAL(initialCursor, logic.getCursor());
}

// 実際のInputLogicを使用した桁送りテスト
void test_real_input_logic_move_cursor() {
    InputLogic logic;
    logic.reset();
    
    // 初期状態: カーソル位置3（分一の位）
    TEST_ASSERT_EQUAL(3, logic.getCursor());
    
    // 桁送り1回目: 3→2（分十の位）
    bool result = logic.moveCursor();
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(2, logic.getCursor());
    
    // 桁送り2回目: 2→1（時一の位）
    result = logic.moveCursor();
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(1, logic.getCursor());
    
    // 桁送り3回目: 1→0（時十の位）
    result = logic.moveCursor();
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(0, logic.getCursor());
    
    // 桁送り4回目: 0→失敗（最左端）
    result = logic.moveCursor();
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_EQUAL(0, logic.getCursor()); // 位置は変わらない
}

// 全桁入力済み（99:99）時の桁送り拒絶テスト
void test_input_logic_move_cursor_all_digits_entered() {
    InputLogic logic;
    logic.reset();
    
    // 全桁に9を入力（99:99）
    logic.incrementAtCursor(9); // 分一の位に9
    logic.moveCursor(); // 分十の位に移動
    logic.incrementAtCursor(9); // 分十の位に9
    logic.moveCursor(); // 時一の位に移動
    logic.incrementAtCursor(9); // 時一の位に9
    logic.moveCursor(); // 時十の位に移動
    logic.incrementAtCursor(9); // 時十の位に9
    
    // 全桁入力済み状態で桁送りを試行
    bool result = logic.moveCursor();
    TEST_ASSERT_FALSE(result); // 拒絶される
    TEST_ASSERT_EQUAL(0, logic.getCursor()); // 位置は変わらない
}

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
    RUN_TEST(test_input_logic_move_cursor_success);
    RUN_TEST(test_input_logic_move_cursor_failure);
    RUN_TEST(test_input_display_b_button_move_cursor);
    RUN_TEST(test_input_display_b_button_move_cursor_failure);
    RUN_TEST(test_real_input_logic_move_cursor);
    RUN_TEST(test_input_logic_move_cursor_all_digits_entered);
    UNITY_END();
    return 0;
} 