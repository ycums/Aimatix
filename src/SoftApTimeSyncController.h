#pragma once

#include "ITimeSyncController.h"
#include "TimeSyncLogic.h"
#include <string>

// SoftAP controller for ESP32 (Step1 scope: Wi‑Fi QR only)
class SoftApTimeSyncController : public ITimeSyncController {
public:
    SoftApTimeSyncController();

    void begin() override;
    void cancel() override;
    void loopTick() override;
    void reissue() override;

    void getCredentials(std::string& outSsid, std::string& outPsk) const override;

    // New ITimeSyncController APIs
    Status getStatus() const override;
    void getUrlPayload(std::string& outUrl) const override;
    const char* getErrorMessage() const override;

private:
    // Cached credentials mirrored from logic
    std::string ssid_;
    std::string psk_;
    std::string token_;

    bool running_;
    std::string lastError_;

    // Pure session logic (lib layer)
    TimeSyncLogic logic_{};
};


