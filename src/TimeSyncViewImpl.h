#pragma once

#include "ITimeSyncView.h"
#include "DisplayAdapter.h"
#include "ui_constants.h"
#include <string>

// Hardware-dependent implementation of ITimeSyncView for M5Stack devices.
class TimeSyncViewImpl : public ITimeSyncView {
public:
    explicit TimeSyncViewImpl(DisplayAdapter* adapter) : adapter_(adapter) {}

    void showTitle(const char* text) override;
    void showHints(const char* hintA, const char* hintB, const char* hintC) override;
    void showWifiQr(const char* payload) override;
    void showUrlQr(const char* payload) override;
    void showError(const char* message) override;

private:
    DisplayAdapter* adapter_;
};


