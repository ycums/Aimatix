#pragma once
#include "StateManager.h"
#include "ITimeSyncView.h"

// Minimal MVP1 state: draws static title/hints and exits to settings on C short press.
class TimeSyncDisplayState : public IState {
public:
    TimeSyncDisplayState(ITimeSyncView* view = nullptr)
        : manager(nullptr), settingsDisplayState(nullptr), view(view) {}

    void setManager(StateManager* m) { manager = m; }
    void setSettingsDisplayState(IState* st) { settingsDisplayState = st; }
    void setView(ITimeSyncView* v) { view = v; }

    void onEnter() override { drawStatic(); }
    void onExit() override {}
    void onDraw() override { /* draw only once on enter to avoid flicker */ }
    void onButtonA() override { /* REISSUE reserved for later MVPs */ }
    void onButtonB() override { /* unused */ }
    void onButtonC() override {
        // EXIT → SETTINGS_MENU
        if (manager != nullptr && settingsDisplayState != nullptr) {
            manager->setState(settingsDisplayState);
        }
    }
    void onButtonALongPress() override {}
    void onButtonBLongPress() override {}
    void onButtonCLongPress() override { /* unassigned */ }

private:
    void drawStatic() {
        if (view) {
            view->showTitle("TIME SYNC > JOIN AP");
            view->showHints("REISSUE", "", "EXIT");
        }
    }

    StateManager* manager;
    IState* settingsDisplayState;
    ITimeSyncView* view;
};


