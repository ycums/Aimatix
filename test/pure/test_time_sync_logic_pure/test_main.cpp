#include <unity.h>
#include <string>
#include "TimeSyncLogic.h"
#include "IRandomProvider.h"
#include "ITimeService.h"

void setUp() {}
void tearDown() {}

class FixedRandomProvider : public IRandomProvider {
public:
    explicit FixedRandomProvider(uint64_t seed) : next(seed) {}
    uint64_t getRandom64() override {
        // simple deterministic sequence
        uint64_t v = next;
        next = next * 6364136223846793005ULL + 1ULL;
        return v;
    }
private:
    uint64_t next;
};

class TestTimeService : public ITimeService {
public:
    TestTimeService() : nowMs(0), nowSec(0), setOk(true) {}
    uint32_t monotonicMillis() const override { return nowMs; }
    time_t now() const override { return nowSec; }
    struct tm* localtime(time_t* t) const override { return ::localtime(t); }
    bool setSystemTime(time_t t) override { nowSec = t; return setOk; }
    void setMillis(uint32_t v) { nowMs = v; }
    void setTime(time_t v) { nowSec = v; }
    void setSucceed(bool v) { setOk = v; }
private:
    uint32_t nowMs;
    time_t nowSec;
    bool setOk;
};

// helpers
static void begin_session(TimeSyncLogic &logic, FixedRandomProvider &rnd, TestTimeService &ts, uint32_t windowMs = 60000) {
    logic.begin(&rnd, &ts, windowMs);
}

// 1) begin => Step1
void test_begin_sets_step1() {
    TimeSyncLogic logic; FixedRandomProvider rnd(123); TestTimeService ts; ts.setMillis(1000);
    begin_session(logic, rnd, ts);
    TEST_ASSERT_EQUAL_INT((int)TimeSyncLogic::Status::Step1, (int)logic.getStatus());
}

// 2) success path => AppliedOk
void test_success_applies_ok_status() {
    TimeSyncLogic logic; FixedRandomProvider rnd(123); TestTimeService ts; ts.setMillis(1000);
    begin_session(logic, rnd, ts);
    const auto creds = logic.getCredentials();
    // within window, valid token, valid ranges
    const int64_t validEpoch = 1735689600000LL + 1000; // min + 1s
    const int tz = 0;
    ts.setMillis(1000 + 1000);
    (void)logic.handleTimeSetRequest(validEpoch, tz, creds.token, &ts);
    TEST_ASSERT_EQUAL_INT((int)TimeSyncLogic::Status::AppliedOk, (int)logic.getStatus());
}

// 3) window expired => Error status
void test_window_expired_status_error() {
    TimeSyncLogic logic; FixedRandomProvider rnd(1); TestTimeService ts; ts.setMillis(1000);
    begin_session(logic, rnd, ts, 60000);
    const auto creds = logic.getCredentials();
    ts.setMillis(1000 + 60001);
    (void)logic.handleTimeSetRequest(1735689600000LL + 1000, 0, creds.token, &ts);
    TEST_ASSERT_EQUAL_INT((int)TimeSyncLogic::Status::Error, (int)logic.getStatus());
}

// 4) window expired => reason code
void test_window_expired_reason() {
    TimeSyncLogic logic; FixedRandomProvider rnd(1); TestTimeService ts; ts.setMillis(1000);
    begin_session(logic, rnd, ts, 60000);
    const auto creds = logic.getCredentials();
    ts.setMillis(1000 + 60001);
    (void)logic.handleTimeSetRequest(1735689600000LL + 1000, 0, creds.token, &ts);
    TEST_ASSERT_EQUAL_STRING("window_expired", logic.getErrorMessage());
}

// 5) token mismatch => Error status
void test_token_mismatch_status_error() {
    TimeSyncLogic logic; FixedRandomProvider rnd(2); TestTimeService ts; ts.setMillis(1000);
    begin_session(logic, rnd, ts, 60000);
    ts.setMillis(1500);
    (void)logic.handleTimeSetRequest(1735689600000LL + 1000, 0, std::string("WRONG"), &ts);
    TEST_ASSERT_EQUAL_INT((int)TimeSyncLogic::Status::Error, (int)logic.getStatus());
}

// 6) token mismatch => reason code
void test_token_mismatch_reason() {
    TimeSyncLogic logic; FixedRandomProvider rnd(2); TestTimeService ts; ts.setMillis(1000);
    begin_session(logic, rnd, ts, 60000);
    ts.setMillis(1500);
    (void)logic.handleTimeSetRequest(1735689600000LL + 1000, 0, std::string("WRONG"), &ts);
    TEST_ASSERT_EQUAL_STRING("invalid_token", logic.getErrorMessage());
}

// 7) rate limit: second request => Error status
void test_rate_limited_status_error() {
    TimeSyncLogic logic; FixedRandomProvider rnd(3); TestTimeService ts; ts.setMillis(1000);
    begin_session(logic, rnd, ts, 60000);
    const auto creds = logic.getCredentials();
    // first request consumes the window allowance (success)
    ts.setMillis(1500);
    (void)logic.handleTimeSetRequest(1735689600000LL + 1000, 0, creds.token, &ts);
    // second request in the same window must be rate limited regardless of success of first
    ts.setMillis(2000);
    (void)logic.handleTimeSetRequest(1735689600000LL + 2000, 0, creds.token, &ts);
    TEST_ASSERT_EQUAL_INT((int)TimeSyncLogic::Status::Error, (int)logic.getStatus());
}

// 8) rate limit: reason code
void test_rate_limited_reason() {
    TimeSyncLogic logic; FixedRandomProvider rnd(3); TestTimeService ts; ts.setMillis(1000);
    begin_session(logic, rnd, ts, 60000);
    const auto creds = logic.getCredentials();
    ts.setMillis(1500);
    (void)logic.handleTimeSetRequest(1735689600000LL + 1000, 0, creds.token, &ts);
    ts.setMillis(2000);
    (void)logic.handleTimeSetRequest(1735689600000LL + 2000, 0, creds.token, &ts);
    TEST_ASSERT_EQUAL_STRING("rate_limited", logic.getErrorMessage());
}

// 9) epoch out of range => Error status
void test_epoch_out_of_range_status_error() {
    TimeSyncLogic logic; FixedRandomProvider rnd(4); TestTimeService ts; ts.setMillis(1000);
    begin_session(logic, rnd, ts, 60000);
    const auto creds = logic.getCredentials();
    ts.setMillis(1500);
    (void)logic.handleTimeSetRequest(1735689600000LL - 1, 0, creds.token, &ts);
    TEST_ASSERT_EQUAL_INT((int)TimeSyncLogic::Status::Error, (int)logic.getStatus());
}

// 10) epoch out of range => reason
void test_epoch_out_of_range_reason() {
    TimeSyncLogic logic; FixedRandomProvider rnd(4); TestTimeService ts; ts.setMillis(1000);
    begin_session(logic, rnd, ts, 60000);
    const auto creds = logic.getCredentials();
    ts.setMillis(1500);
    (void)logic.handleTimeSetRequest(1735689600000LL - 1, 0, creds.token, &ts);
    TEST_ASSERT_EQUAL_STRING("time_out_of_range", logic.getErrorMessage());
}

// 11) tz out of range => Error status
void test_tz_out_of_range_status_error() {
    TimeSyncLogic logic; FixedRandomProvider rnd(5); TestTimeService ts; ts.setMillis(1000);
    begin_session(logic, rnd, ts, 60000);
    const auto creds = logic.getCredentials();
    ts.setMillis(1500);
    (void)logic.handleTimeSetRequest(1735689600000LL + 1000, 841, creds.token, &ts);
    TEST_ASSERT_EQUAL_INT((int)TimeSyncLogic::Status::Error, (int)logic.getStatus());
}

// 12) tz out of range => reason
void test_tz_out_of_range_reason() {
    TimeSyncLogic logic; FixedRandomProvider rnd(5); TestTimeService ts; ts.setMillis(1000);
    begin_session(logic, rnd, ts, 60000);
    const auto creds = logic.getCredentials();
    ts.setMillis(1500);
    (void)logic.handleTimeSetRequest(1735689600000LL + 1000, 841, creds.token, &ts);
    TEST_ASSERT_EQUAL_STRING("tz_offset_out_of_range", logic.getErrorMessage());
}

// 13) apply failed => Error status
void test_apply_failed_status_error() {
    TimeSyncLogic logic; FixedRandomProvider rnd(6); TestTimeService ts; ts.setMillis(1000); ts.setSucceed(false);
    begin_session(logic, rnd, ts, 60000);
    const auto creds = logic.getCredentials();
    ts.setMillis(1500);
    (void)logic.handleTimeSetRequest(1735689600000LL + 1000, 0, creds.token, &ts);
    TEST_ASSERT_EQUAL_INT((int)TimeSyncLogic::Status::Error, (int)logic.getStatus());
}

// 14) apply failed => reason
void test_apply_failed_reason() {
    TimeSyncLogic logic; FixedRandomProvider rnd(6); TestTimeService ts; ts.setMillis(1000); ts.setSucceed(false);
    begin_session(logic, rnd, ts, 60000);
    const auto creds = logic.getCredentials();
    ts.setMillis(1500);
    (void)logic.handleTimeSetRequest(1735689600000LL + 1000, 0, creds.token, &ts);
    TEST_ASSERT_EQUAL_STRING("apply_failed", logic.getErrorMessage());
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_begin_sets_step1);
    RUN_TEST(test_success_applies_ok_status);
    RUN_TEST(test_window_expired_status_error);
    RUN_TEST(test_window_expired_reason);
    RUN_TEST(test_token_mismatch_status_error);
    RUN_TEST(test_token_mismatch_reason);
    RUN_TEST(test_rate_limited_status_error);
    RUN_TEST(test_rate_limited_reason);
    RUN_TEST(test_epoch_out_of_range_status_error);
    RUN_TEST(test_epoch_out_of_range_reason);
    RUN_TEST(test_tz_out_of_range_status_error);
    RUN_TEST(test_tz_out_of_range_reason);
    RUN_TEST(test_apply_failed_status_error);
    RUN_TEST(test_apply_failed_reason);
    return UNITY_END();
}


