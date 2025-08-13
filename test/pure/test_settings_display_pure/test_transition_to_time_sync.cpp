#include <unity.h>
#include "SettingsDisplayState.h"
#include "SettingsLogic.h"

class DummyTimeSyncState : public IState {
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

void test_settings_menu_select_time_sync_transitions_to_time_sync_state() {
    SettingsLogic logic;
    SettingsDisplayState settings(&logic);
    DummyTimeSyncState timeSync;
    StateManager manager;
    settings.setManager(&manager);
    settings.setTimeSyncDisplayState(&timeSync);
    manager.setState(&settings);

    logic.setSelectedItem(SettingsItem::SET_DATE_TIME);
    manager.handleButtonC();

    TEST_ASSERT_EQUAL((IState*)&timeSync, manager.getCurrentState());
}



