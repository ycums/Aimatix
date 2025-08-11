#pragma once
#include "StateManager.h"
#include "ITimeSyncView.h"
#include "ITimeSyncController.h"
#include "TimeSyncCore.h"
#include <string>

// Minimal MVP1 state: draws static title/hints and exits to settings on C short press.
class TimeSyncDisplayState : public IState {
public:
    TimeSyncDisplayState(ITimeSyncView* view = nullptr, ITimeSyncController* controller = nullptr)
        : manager(nullptr), settingsDisplayState(nullptr), view(view), controller(controller) {}

    void setManager(StateManager* m) { manager = m; }
    void setSettingsDisplayState(IState* st) { settingsDisplayState = st; }
    void setView(ITimeSyncView* v) { view = v; }
    void setController(ITimeSyncController* c) { controller = c; }

    void onEnter() override { drawStep1(); }
    void onExit() override {}
    void onDraw() override {
        if (controller) controller->loopTick();
        /* no redraw to avoid flicker */
    }
    void onButtonA() override {
        if (controller) {
            controller->reissue();
            std::string ssid, psk;
            controller->getCredentials(ssid, psk);
            const std::string payload = TimeSyncCore::buildWifiQrPayload(ssid, psk);
            if (view) view->showWifiQr(payload.c_str());
        }
    }
    void onButtonB() override { /* unused */ }
    void onButtonC() override {
        // EXIT → SETTINGS_MENU
        if (controller) controller->cancel();
        if (manager != nullptr && settingsDisplayState != nullptr) {
            manager->setState(settingsDisplayState);
        }
    }
    void onButtonALongPress() override {}
    void onButtonBLongPress() override {}
    void onButtonCLongPress() override { /* unassigned */ }

private:
    void drawStep1() {
        if (view) {
            view->showTitle("TIME SYNC > JOIN AP");
            view->showHints("REISSUE", "", "EXIT");
        }
        if (controller && view) {
            controller->begin();
            std::string ssid, psk;
            controller->getCredentials(ssid, psk);
            const std::string payload = TimeSyncCore::buildWifiQrPayload(ssid, psk);
            view->showWifiQr(payload.c_str());
        }
    }

    StateManager* manager;
    IState* settingsDisplayState;
    ITimeSyncView* view;
    ITimeSyncController* controller;
};


