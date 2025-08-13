#pragma once

#include "ITimeSyncController.h"
#include <string>

class MockTimeSyncController : public ITimeSyncController {
public:
    void begin() override { beginCount++; }
    void cancel() override { cancelCount++; }
    void loopTick() override { loopTickCount++; }
    void reissue() override { reissueCount++; ssid = reissueSsid; psk = reissuePsk; }

    void getCredentials(std::string& outSsid, std::string& outPsk) const override { outSsid = ssid; outPsk = psk; }

    Status getStatus() const override { return status; }
    void getUrlPayload(std::string& outUrl) const override { outUrl = urlPayload; }
    const char* getErrorMessage() const override { return lastError.c_str(); }

    void setInitialCredentials(const std::string& s, const std::string& p) { ssid = s; psk = p; }
    void setReissueCredentials(const std::string& s, const std::string& p) { reissueSsid = s; reissuePsk = p; }

    int beginCount{0};
    int cancelCount{0};
    int loopTickCount{0};
    int reissueCount{0};

    std::string ssid;
    std::string psk;
    std::string reissueSsid;
    std::string reissuePsk;

    // Extended API backing fields (tests may ignore)
    Status status{Status::Idle};
    std::string urlPayload;
    std::string lastError;
};


