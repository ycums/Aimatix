#pragma once

#include "ITimeSyncController.h"
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

private:
    void generateCredentials();

    std::string ssid_;
    std::string psk_;
    std::string token_;

    bool running_;
};


