#pragma once

#include <string>
#include <cstdint>
#include <ctime>
#include "IRandomProvider.h"
#include "ITimeService.h"
#include "TimeSyncCore.h"

// Pure logic: manages a single time sync session lifecycle
class TimeSyncLogic {
public:
    enum class Status { Idle, Step1, Step2, AppliedOk, Error };

    struct Credentials { std::string ssid; std::string psk; std::string token; };

    void begin(IRandomProvider* rnd, ITimeService* timeService, uint32_t windowMs = 60000);
    void reissue(IRandomProvider* rnd);
    void onStationConnected();
    // Override the internally generated token for the session (e.g., when token is issued by adapter)
    void setExpectedToken(const std::string& token) { creds_.token = token; }

    // Returns current URL for Step2 QR (requires IP provided by caller)
    std::string buildUrlPayload(const std::string& ip) const;

    // Handle POST /time/set
    // Returns true if applied and status becomes AppliedOk
    bool handleTimeSetRequest(int64_t epochMs, int tzOffsetMin, const std::string& token,
                              ITimeService* timeService);

    // Window helpers for controllers/adapters
    bool isWindowExpired(uint32_t nowMs) const {
        return !TimeSyncCore::isWithinWindow(startMs_, nowMs, windowMs_);
    }
    uint32_t getWindowRemainingMs(uint32_t nowMs) const {
        if (nowMs >= startMs_ && (nowMs - startMs_) <= windowMs_) {
            return windowMs_ - (nowMs - startMs_);
        }
        // handle wraparound and expired cases conservatively
        const uint32_t elapsed = nowMs - startMs_;
        return (elapsed <= windowMs_) ? (windowMs_ - elapsed) : 0u;
    }

    Status getStatus() const { return status_; }
    const char* getErrorMessage() const { return lastError_.c_str(); }
    const Credentials& getCredentials() const { return creds_; }

private:
    static std::string makeSsid(uint64_t r);
    static std::string makePsk(uint64_t r);
    static std::string makeToken(uint64_t r);

    Credentials creds_{};
    Status status_{Status::Idle};
    std::string lastError_{};
    uint32_t startMs_{0};
    uint32_t windowMs_{60000};
    bool rateConsumed_{false};
};


