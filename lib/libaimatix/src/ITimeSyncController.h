#pragma once

#include <string>

// Abstract controller interface for SoftAP Time Sync (Step1 scope).
class ITimeSyncController {
public:
    virtual ~ITimeSyncController() {}

    // Start SoftAP and prepare credentials (SSID/PSK/token)
    virtual void begin() = 0;

    // Stop SoftAP
    virtual void cancel() = 0;

    // Periodic tick from UI (~20Hz). May be no-op in Step1.
    virtual void loopTick() = 0;

    // Reissue credentials (new SSID/PSK/token). Should not restart AP if unnecessary.
    virtual void reissue() = 0;

    // Get current SSID and PSK (Step1 scope). Implementations copy to outputs.
    virtual void getCredentials(std::string& outSsid, std::string& outPsk) const = 0;
};


