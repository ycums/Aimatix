#include <unity.h>
#include "TimeSyncDisplayState.h"
#include "../mock/MockTimeSyncView.h"

class DummySettingsState : public IState {
public:
    void onEnter() override {}
    void onExit() override {}
    void onDraw() override {}
    void onButtonA() override {}
    void onButtonB() override {}
    void onButtonC() override {}
    void onButtonALongPress() override {}
    void onButtonBLongPress() override {}
    void onButtonCLongPress() override {}
};

void setUp() {}
void tearDown() {}

void test_time_sync_title_join_ap() {
    MockTimeSyncView view;
    TimeSyncDisplayState state(&view);
    state.onEnter();
    TEST_ASSERT_TRUE(view.calledShowTitle);
    TEST_ASSERT_EQUAL_STRING("TIME SYNC | JOIN AP", view.lastTitle.c_str());
}

void test_time_sync_hints_reissue_exit() {
    MockTimeSyncView view;
    TimeSyncDisplayState state(&view);
    state.onEnter();
    TEST_ASSERT_TRUE(view.calledShowHints);
    TEST_ASSERT_EQUAL_STRING("REISSUE", view.lastHintA.c_str());
    TEST_ASSERT_EQUAL_STRING("", view.lastHintB.c_str());
    TEST_ASSERT_EQUAL_STRING("EXIT", view.lastHintC.c_str());
}

void test_time_sync_c_short_exit_to_settings() {
    StateManager manager;
    MockTimeSyncView view;
    TimeSyncDisplayState state(&view);
    DummySettingsState settings;
    state.setManager(&manager);
    state.setSettingsDisplayState(&settings);
    manager.setState(&state);
    manager.handleButtonC();
    TEST_ASSERT_EQUAL((IState*)&settings, manager.getCurrentState());
}

void test_time_sync_on_draw_does_not_redraw_title() {
    MockTimeSyncView view;
    TimeSyncDisplayState state(&view);
    state.onEnter();
    view.calledShowTitle = false; // reset
    state.onDraw();
    TEST_ASSERT_FALSE(view.calledShowTitle);
}

void test_time_sync_on_draw_does_not_redraw_hints() {
    MockTimeSyncView view;
    TimeSyncDisplayState state(&view);
    state.onEnter();
    view.calledShowHints = false; // reset
    state.onDraw();
    TEST_ASSERT_FALSE(view.calledShowHints);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_time_sync_title_join_ap);
    RUN_TEST(test_time_sync_hints_reissue_exit);
    RUN_TEST(test_time_sync_c_short_exit_to_settings);
    RUN_TEST(test_time_sync_on_draw_does_not_redraw_title);
    RUN_TEST(test_time_sync_on_draw_does_not_redraw_hints);
    return UNITY_END();
}


