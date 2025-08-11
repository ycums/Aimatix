#pragma once
#include "ITimeSyncView.h"
#include <string>

class MockTimeSyncView : public ITimeSyncView {
public:
    void showTitle(const char* text) override { lastTitle = text ? text : ""; calledShowTitle = true; }
    void showHints(const char* a, const char* b, const char* c) override {
        lastHintA = a ? a : "";
        lastHintB = b ? b : "";
        lastHintC = c ? c : "";
        calledShowHints = true;
    }
    void showWifiQr(const char* payload) override { lastWifiQr = payload ? payload : ""; calledShowWifiQr = true; }
    void showUrlQr(const char* payload) override { lastUrlQr = payload ? payload : ""; calledShowUrlQr = true; }
    void showError(const char* message) override { lastError = message ? message : ""; calledShowError = true; }

    std::string lastTitle;
    std::string lastHintA, lastHintB, lastHintC;
    std::string lastWifiQr;
    std::string lastUrlQr;
    std::string lastError;
    bool calledShowTitle{false};
    bool calledShowHints{false};
    bool calledShowWifiQr{false};
    bool calledShowUrlQr{false};
    bool calledShowError{false};
};


