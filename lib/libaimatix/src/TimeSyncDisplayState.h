#pragma once
#include "StateManager.h"
#include "ITimeSyncView.h"
#include "ITimeSyncController.h"
#include "TimeSyncCore.h"
#include <string>
#include <string>

// Minimal MVP1 state: draws static title/hints and exits to settings on C short press.
class TimeSyncDisplayState : public IState {
public:
    TimeSyncDisplayState(ITimeSyncView* view = nullptr, ITimeSyncController* controller = nullptr)
        : manager(nullptr), settingsDisplayState(nullptr), view(view), controller(controller) {}

    void setManager(StateManager* m) { manager = m; }
    void setSettingsDisplayState(IState* st) { settingsDisplayState = st; }
    void setMainDisplayState(IState* st) { mainDisplayState = st; }
    void setView(ITimeSyncView* v) { view = v; }
    void setController(ITimeSyncController* c) { controller = c; }

    void onEnter() override {
        step2Drawn = false;
        errorCountdownTicks = 0;
        drawStep1();
    }
    void onExit() override {}
    void onDraw() override {
        if (controller) {
            controller->loopTick();
            auto st = controller->getStatus();
            if (st == ITimeSyncController::Status::Step2) {
                if (!step2Drawn) {
                    if (view) {
                        view->showTitle("TIME SYNC > OPEN URL");
                        view->showHints("REISSUE", "", "EXIT");
                        std::string url;
                        controller->getUrlPayload(url);
                        view->showUrlQr(url.c_str());
                    }
                    step2Drawn = true;
                }
            } else if (st == ITimeSyncController::Status::AppliedOk) {
                if (manager != nullptr && mainDisplayState != nullptr) {
                    manager->setState(mainDisplayState);
                }
            } else if (st == ITimeSyncController::Status::Error) {
                if (errorCountdownTicks == 0) {
                    if (view) {
                        view->showTitle("TIME SYNC > ERROR");
                        view->showError(controller->getErrorMessage());
                    }
                    errorCountdownTicks = 40; // ~2s at ~20Hz
                } else {
                    errorCountdownTicks--;
                    if (errorCountdownTicks == 0) {
                        if (manager != nullptr && settingsDisplayState != nullptr) {
                            manager->setState(settingsDisplayState);
                        }
                    }
                }
            }
        }
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
    IState* mainDisplayState{nullptr};
    ITimeSyncView* view;
    ITimeSyncController* controller;

    bool step2Drawn{false};
    int errorCountdownTicks{0};
};


