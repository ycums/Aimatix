#include "TimeSyncLogic.h"

namespace {
std::string toHexN(uint64_t v, int n) {
    static const char* hex = "0123456789abcdef";
    auto s = std::string(n, '0');
    for (int i = n - 1; i >= 0; --i) {
        s[i] = hex[v & 0xF];
        v >>= 4;
    }
    return s;
}
}

auto TimeSyncLogic::makeSsid(uint64_t r) -> std::string {
    return std::string("AIM-TS-") + toHexN(r, 8);
}

auto TimeSyncLogic::makePsk(uint64_t r) -> std::string {
    return toHexN(r, 16);
}

auto TimeSyncLogic::makeToken(uint64_t r) -> std::string {
    return toHexN(r, 16);
}

void TimeSyncLogic::begin(IRandomProvider* rnd, ITimeManager* timeManager, uint32_t windowMs) {
    if (rnd == nullptr || timeManager == nullptr) {
        status_ = Status::Error;
        lastError_ = "bad_ports";
        return;
    }
    creds_.ssid = makeSsid(rnd->getRandom64());
    creds_.psk = makePsk(rnd->getRandom64());
    creds_.token = makeToken(rnd->getRandom64());
    startMs_ = static_cast<uint32_t>(timeManager->getCurrentMillis());
    windowMs_ = windowMs;
    rateConsumed_ = false;
    status_ = Status::Step1;
    lastError_.clear();
}

void TimeSyncLogic::reissue(IRandomProvider* rnd) {
    if (rnd == nullptr) {
        status_ = Status::Error;
        lastError_ = "bad_ports";
        return;
    }
    creds_.ssid = makeSsid(rnd->getRandom64());
    creds_.psk = makePsk(rnd->getRandom64());
    creds_.token = makeToken(rnd->getRandom64());
}

void TimeSyncLogic::onStationConnected() {
    if (status_ == Status::Step1) {
        status_ = Status::Step2;
    }
}

auto TimeSyncLogic::buildUrlPayload(const std::string& ip) const -> std::string {
    return TimeSyncCore::buildUrl(ip, creds_.token);
}

bool TimeSyncLogic::handleTimeSetRequest(int64_t epochMs, int tzOffsetMin, const std::string& token,
                                         ITimeManager* timeManager, ITimeProvider* timeProvider) {
    if (timeManager == nullptr || timeProvider == nullptr) {
        status_ = Status::Error;
        lastError_ = "bad_ports";
        return false;
    }
    const auto nowMs = static_cast<uint32_t>(timeManager->getCurrentMillis());
    if (!TimeSyncCore::isWithinWindow(startMs_, nowMs, windowMs_)) {
        status_ = Status::Error;
        lastError_ = "window_expired";
        return false;
    }
    if (!TimeSyncCore::verifyToken(creds_.token, token)) {
        status_ = Status::Error;
        lastError_ = "invalid_token";
        return false;
    }
    // Rate limit: first attempt consumes the allowance regardless of success
    if (rateConsumed_) {
        status_ = Status::Error;
        lastError_ = "rate_limited";
        return false;
    }
    rateConsumed_ = true;
    // Validate epoch range per spec
    const int64_t minEpoch = 1735689600000LL; // 2025-01-01 UTC in ms
    const int64_t maxEpoch = 4102444800000LL; // 2100-01-01 UTC in ms (time_t width dependent in practice)
    if (epochMs < minEpoch || epochMs > maxEpoch) {
        status_ = Status::Error;
        lastError_ = "time_out_of_range";
        return false;
    }
    if (tzOffsetMin < -14 * 60 || tzOffsetMin > 14 * 60) {
        status_ = Status::Error;
        lastError_ = "tz_offset_out_of_range";
        return false;
    }

    const time_t sec = static_cast<time_t>(epochMs / 1000);
    if (!timeProvider->setSystemTime(sec)) {
        status_ = Status::Error;
        lastError_ = "apply_failed";
        return false;
    }
    status_ = Status::AppliedOk;
    lastError_.clear();
    return true;
}


