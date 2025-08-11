#include <unity.h>
#include "TimeSyncDisplayState.h"
#include "../mock/MockTimeSyncView.h"
#include "../mock/MockTimeSyncController.h"
#include <string>

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
    TEST_ASSERT_EQUAL_STRING("TIME SYNC > JOIN AP", view.lastTitle.c_str());
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

// --- New tests for controller integration (Step1) ---

void test_time_sync_on_enter_calls_begin_once() {
    MockTimeSyncView view;
    MockTimeSyncController controller;
    controller.setInitialCredentials("SSID", "PSK");
    TimeSyncDisplayState state(&view, &controller);
    state.onEnter();
    TEST_ASSERT_EQUAL(1, controller.beginCount);
}

void test_time_sync_on_enter_shows_wifi_qr_called() {
    MockTimeSyncView view;
    MockTimeSyncController controller;
    controller.setInitialCredentials("SSID", "PSK");
    TimeSyncDisplayState state(&view, &controller);
    state.onEnter();
    TEST_ASSERT_TRUE(view.calledShowWifiQr);
}

void test_time_sync_on_enter_shows_wifi_qr_payload_exact() {
    MockTimeSyncView view;
    MockTimeSyncController controller;
    controller.setInitialCredentials("SSID", "PSK");
    TimeSyncDisplayState state(&view, &controller);
    state.onEnter();
    TEST_ASSERT_EQUAL_STRING("WIFI:T:WPA;S:SSID;P:PSK;H:false;;", view.lastWifiQr.c_str());
}

void test_time_sync_on_draw_calls_loop_tick_once() {
    MockTimeSyncView view;
    MockTimeSyncController controller;
    controller.setInitialCredentials("SSID", "PSK");
    TimeSyncDisplayState state(&view, &controller);
    state.onEnter();
    controller.loopTickCount = 0;
    state.onDraw();
    TEST_ASSERT_EQUAL(1, controller.loopTickCount);
}

void test_time_sync_on_draw_does_not_redraw_qr() {
    MockTimeSyncView view;
    MockTimeSyncController controller;
    controller.setInitialCredentials("SSID", "PSK");
    TimeSyncDisplayState state(&view, &controller);
    state.onEnter();
    view.calledShowWifiQr = false;
    state.onDraw();
    TEST_ASSERT_FALSE(view.calledShowWifiQr);
}

void test_time_sync_button_a_calls_reissue_once() {
    MockTimeSyncView view;
    MockTimeSyncController controller;
    controller.setInitialCredentials("S1", "P1");
    controller.setReissueCredentials("S2", "P2");
    TimeSyncDisplayState state(&view, &controller);
    state.onEnter();
    controller.reissueCount = 0;
    state.onButtonA();
    TEST_ASSERT_EQUAL(1, controller.reissueCount);
}

void test_time_sync_button_a_updates_qr_payload() {
    MockTimeSyncView view;
    MockTimeSyncController controller;
    controller.setInitialCredentials("S1", "P1");
    controller.setReissueCredentials("S2", "P2");
    TimeSyncDisplayState state(&view, &controller);
    state.onEnter();
    view.calledShowWifiQr = false;
    state.onButtonA();
    TEST_ASSERT_EQUAL_STRING("WIFI:T:WPA;S:S2;P:P2;H:false;;", view.lastWifiQr.c_str());
}

void test_time_sync_button_c_calls_cancel_once() {
    StateManager manager;
    MockTimeSyncView view;
    MockTimeSyncController controller;
    controller.setInitialCredentials("S1", "P1");
    TimeSyncDisplayState state(&view, &controller);
    DummySettingsState settings;
    state.setManager(&manager);
    state.setSettingsDisplayState(&settings);
    manager.setState(&state);
    controller.cancelCount = 0;
    manager.handleButtonC();
    TEST_ASSERT_EQUAL(1, controller.cancelCount);
}

void test_time_sync_button_c_transitions_to_settings_with_controller() {
    StateManager manager;
    MockTimeSyncView view;
    MockTimeSyncController controller;
    controller.setInitialCredentials("S1", "P1");
    TimeSyncDisplayState state(&view, &controller);
    DummySettingsState settings;
    state.setManager(&manager);
    state.setSettingsDisplayState(&settings);
    manager.setState(&state);
    manager.handleButtonC();
    TEST_ASSERT_EQUAL((IState*)&settings, manager.getCurrentState());
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_time_sync_title_join_ap);
    RUN_TEST(test_time_sync_hints_reissue_exit);
    RUN_TEST(test_time_sync_c_short_exit_to_settings);
    RUN_TEST(test_time_sync_on_draw_does_not_redraw_title);
    RUN_TEST(test_time_sync_on_draw_does_not_redraw_hints);
    // New tests for Step1 behavior with controller
    void test_time_sync_on_enter_calls_begin_once();
    void test_time_sync_on_enter_shows_wifi_qr_called();
    void test_time_sync_on_enter_shows_wifi_qr_payload_exact();
    void test_time_sync_on_draw_calls_loop_tick_once();
    void test_time_sync_on_draw_does_not_redraw_qr();
    void test_time_sync_button_a_calls_reissue_once();
    void test_time_sync_button_a_updates_qr_payload();
    void test_time_sync_button_c_calls_cancel_once();
    void test_time_sync_button_c_transitions_to_settings_with_controller();
    RUN_TEST(test_time_sync_on_enter_calls_begin_once);
    RUN_TEST(test_time_sync_on_enter_shows_wifi_qr_called);
    RUN_TEST(test_time_sync_on_enter_shows_wifi_qr_payload_exact);
    RUN_TEST(test_time_sync_on_draw_calls_loop_tick_once);
    RUN_TEST(test_time_sync_on_draw_does_not_redraw_qr);
    RUN_TEST(test_time_sync_button_a_calls_reissue_once);
    RUN_TEST(test_time_sync_button_a_updates_qr_payload);
    RUN_TEST(test_time_sync_button_c_calls_cancel_once);
    RUN_TEST(test_time_sync_button_c_transitions_to_settings_with_controller);
    return UNITY_END();
}


