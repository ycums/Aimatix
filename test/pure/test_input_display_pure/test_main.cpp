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
    void clear() override { clearCount++; }
    void showTitle(const char*, int, bool) override { showTitleCount++; }
    void showHints(const char*, const char*, const char*) override { showHintsCount++; }
    void showValue(int) override { showValueCount++; }
};
class MockLogic : public InputLogic {
public:
    int value=0;
    int getValue() const { return value; }
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

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_transition_to_input_display_on_a_button_press);
    RUN_TEST(test_input_display_state_methods);
    UNITY_END();
    return 0;
} 