#include <unity.h>
#include "StateManager.h"

// モックIState実装
class MockState : public IState {
public:
    int enterCount = 0;
    int exitCount = 0;
    int drawCount = 0;
    int buttonACount = 0;
    int buttonBCount = 0;
    int buttonCCount = 0;
    int buttonALongCount = 0;
    int buttonCLongCount = 0;
    void onEnter() override { enterCount++; }
    void onExit() override { exitCount++; }
    void onDraw() override { drawCount++; }
    void onButtonA() override { buttonACount++; }
    void onButtonB() override { buttonBCount++; }
    void onButtonC() override { buttonCCount++; }
    void onButtonALongPress() override { buttonALongCount++; }
    void onButtonCLongPress() override { buttonCLongCount++; }
};

void setUp(void) {}
void tearDown(void) {}

// StateManagerでMainDisplay→InputDisplayに遷移できる
void test_state_transition_main_to_input() {
    StateManager sm;
    MockState mainState, inputState;
    sm.setState(&mainState);
    TEST_ASSERT_EQUAL(1, mainState.enterCount);
    sm.setState(&inputState);
    TEST_ASSERT_EQUAL(1, mainState.exitCount);
    TEST_ASSERT_EQUAL(1, inputState.enterCount);
}

// StateManagerが現在の状態を正しく保持する
void test_state_manager_holds_current_state() {
    StateManager sm;
    MockState s1, s2;
    sm.setState(&s1);
    TEST_ASSERT_EQUAL_PTR(&s1, sm.getCurrentState());
    sm.setState(&s2);
    TEST_ASSERT_EQUAL_PTR(&s2, sm.getCurrentState());
}

// StateManager経由でボタンイベントが現在の状態に伝播される
void test_button_event_propagation() {
    StateManager sm;
    MockState s;
    sm.setState(&s);
    sm.handleButtonA();
    sm.handleButtonB();
    sm.handleButtonC();
    sm.handleButtonALongPress();
    sm.handleButtonCLongPress();
    TEST_ASSERT_EQUAL(1, s.buttonACount);
    TEST_ASSERT_EQUAL(1, s.buttonBCount);
    TEST_ASSERT_EQUAL(1, s.buttonCCount);
    TEST_ASSERT_EQUAL(1, s.buttonALongCount);
    TEST_ASSERT_EQUAL(1, s.buttonCLongCount);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_state_transition_main_to_input);
    RUN_TEST(test_state_manager_holds_current_state);
    RUN_TEST(test_button_event_propagation);
    UNITY_END();
    return 0;
} 