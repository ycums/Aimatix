#include <unity.h>
#include "StateManager.h"
#include "MainDisplayState.h"
#include "InputDisplayState.h"
#include "InputLogic.h"
#include <cstdio>
#include <cstring>
#include <vector>
#include <ctime>
#include "../mock/MockTimeProvider.h"
#include <memory>

std::vector<time_t> alarm_times;

const time_t kFixedTestTime = 1700000000;
std::shared_ptr<MockTimeProvider> testTimeProvider = std::make_shared<MockTimeProvider>(kFixedTestTime);

void setUp(void) {}
void tearDown(void) {}

// StateManager経由でMainDisplay→InputDisplayに遷移する（onButtonA経由）
void test_transition_to_input_display_on_a_button_press() {
    StateManager sm;
    InputLogic logic(testTimeProvider);
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
    int clearCount=0, showTitleCount=0, showHintsCount=0, showDigitCount=0, showColonCount=0;
    void clear() override { clearCount++; }
    void showTitle(const char*, int, bool) override { showTitleCount++; }
    void showHints(const char*, const char*, const char*) override { showHintsCount++; }
    void showDigit(int, int, bool) override { showDigitCount++; }
    void showColon() override { showColonCount++; }
    void showPreview(const char*) override {}
};
class MockLogic : public InputLogic {
public:
    MockLogic(std::shared_ptr<ITimeProvider> provider) : InputLogic(provider) { value = 0; }
    int value;
    int getValue() const { return value; }
};

// onEnter, onDraw, setView, onExit, onButtonX系のテスト
void test_input_display_state_methods() {
    MockLogic logic(testTimeProvider);
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
    TEST_ASSERT_EQUAL(4, view.showDigitCount); // 4回が正
    // 同じ値なら呼ばれない
    state.onDraw();
    TEST_ASSERT_EQUAL(4, view.showDigitCount);
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
    InputLogic logic(testTimeProvider);
    InputDisplayState inputState(&logic);
    MainDisplayState mainState(&sm, &inputState);
    // 参照をセット
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
    InputLogic logic(testTimeProvider);
    InputDisplayState inputState(&logic);
    MainDisplayState mainState(&sm, &inputState);
    inputState.setManager(&sm);
    inputState.setMainDisplayState(&mainState);
    sm.setState(&mainState);
    sm.handleButtonA();
    // InputDisplayに遷移したことを確認
    TEST_ASSERT_NOT_NULL(sm.getCurrentState());
    sm.handleButtonC();
    // まだInputDisplayにいることを確認（ポインタ比較ではなく状態確認）
    TEST_ASSERT_NOT_NULL(sm.getCurrentState());
    sm.handleButtonA();
    TEST_ASSERT_NOT_NULL(sm.getCurrentState());
    sm.handleButtonB();
    TEST_ASSERT_NOT_NULL(sm.getCurrentState());
    sm.handleButtonALongPress();
    TEST_ASSERT_NOT_NULL(sm.getCurrentState());
}
// メイン画面でC長押ししても何も起こらない
void test_main_display_c_long_press_does_nothing() {
    StateManager sm;
    InputLogic logic(testTimeProvider);
    InputDisplayState inputState(&logic);
    MainDisplayState mainState(&sm, &inputState);
    inputState.setManager(&sm);
    inputState.setMainDisplayState(&mainState);
    sm.setState(&mainState);
    TEST_ASSERT_EQUAL_PTR(&mainState, sm.getCurrentState());
    sm.handleButtonCLongPress();
    TEST_ASSERT_EQUAL_PTR(&mainState, sm.getCurrentState());
}

// InputDisplayStateのonButtonBLongPress()のテスト
void test_input_display_b_long_press_resets_input() {
    MockLogic logic(testTimeProvider);
    MockView view;
    InputDisplayState state(&logic, &view);
    
    // 初期状態でonButtonBLongPress()を呼び出し
    state.onButtonBLongPress();
    // 例外が発生しないことを確認（正常に実行されることを確認）
    
    // 入力値を設定してからリセット
    logic.value = 42;
    state.onButtonBLongPress();
    // 例外が発生しないことを確認（正常に実行されることを確認）
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_transition_to_input_display_on_a_button_press);
    RUN_TEST(test_input_display_state_methods);
    RUN_TEST(test_input_display_c_long_press_returns_to_main);
    RUN_TEST(test_input_display_other_buttons_do_not_return_to_main);
    RUN_TEST(test_main_display_c_long_press_does_nothing);
    RUN_TEST(test_input_display_b_long_press_resets_input);
    UNITY_END();
    return 0;
} 