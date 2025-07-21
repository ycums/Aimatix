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

// InputDisplayがちらつかず安定して値を描画できる
void test_input_display_stable_rendering() {
    InputLogic logic;
    InputDisplayState inputState(&logic);
    // 標準出力を一時バッファにリダイレクト
    char buf[128] = {0};
    FILE* orig = stdout;
    FILE* tmp = freopen("test_output.txt", "w", stdout);
    inputState.onDraw();
    fflush(stdout);
    freopen("CON", "w", stdout); // Windows用: 標準出力を元に戻す
    FILE* f = fopen("test_output.txt", "r");
    if (f) {
        fgets(buf, sizeof(buf), f);
        fclose(f);
        remove("test_output.txt");
    }
    TEST_ASSERT_TRUE(strstr(buf, "[InputDisplay] value=0") != nullptr);
}

void test_no_transition_on_a_button_long_press() {
    // TODO: Enable after implementing long-press event mock
    TEST_IGNORE_MESSAGE("Enable after implementing long-press event mock.");
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_transition_to_input_display_on_a_button_press);
    RUN_TEST(test_input_display_stable_rendering);
    RUN_TEST(test_no_transition_on_a_button_long_press);
    UNITY_END();
    return 0;
} 