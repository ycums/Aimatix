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

    std::string lastTitle;
    std::string lastHintA, lastHintB, lastHintC;
    bool calledShowTitle{false};
    bool calledShowHints{false};
};


