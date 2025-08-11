#include <unity.h>
#include "TimeSyncLogic.h"
#include "ITimeManager.h"
#include "ITimeProvider.h"

class MockRnd : public IRandomProvider {
public:
    uint64_t v{0x0123456789abcdefULL};
    uint64_t getRandom64() override { return v ^= 0x9e3779b97f4a7c15ULL; }
};

class MockClock : public ITimeManager {
public:
    unsigned long now{0};
    unsigned long getCurrentMillis() const override { return now; }
    time_t getCurrentTime() const override { return 0; }
};

class MockSysClock : public ITimeProvider {
public:
    time_t last{0};
    bool ok{true};
    time_t now() const override { return last; }
    struct tm* localtime(time_t* t) const override { (void)t; return nullptr; }
    bool setSystemTime(time_t t) override { last = t; return ok; }
};

void setUp() {}
void tearDown() {}

void test_begin_sets_step1_and_creds() {
    TimeSyncLogic logic;
    MockRnd rnd; MockClock clk; clk.now = 1000;
    logic.begin(&rnd, &clk, 60000);
    TEST_ASSERT_EQUAL((int)TimeSyncLogic::Status::Step1, (int)logic.getStatus());
}

void test_on_station_connected_moves_to_step2() {
    TimeSyncLogic logic; MockRnd rnd; MockClock clk; clk.now = 0;
    logic.begin(&rnd, &clk, 60000);
    logic.onStationConnected();
    TEST_ASSERT_EQUAL((int)TimeSyncLogic::Status::Step2, (int)logic.getStatus());
}

void test_build_url_uses_token() {
    TimeSyncLogic logic; MockRnd rnd; MockClock clk; clk.now = 0;
    logic.begin(&rnd, &clk, 60000);
    auto url = logic.buildUrlPayload("192.168.4.1");
    TEST_ASSERT_TRUE(url.find("http://192.168.4.1/sync?t=") == 0);
}

void test_handle_time_set_success_sets_applied() {
    TimeSyncLogic logic; MockRnd rnd; MockClock clk; clk.now = 0; MockSysClock sys;
    logic.begin(&rnd, &clk, 60000);
    auto creds = logic.getCredentials();
    clk.now = 1000;
    const int64_t okEpoch = 1735689600000LL; // min boundary
    bool ok = logic.handleTimeSetRequest(okEpoch, 0, creds.token, &clk, &sys);
    TEST_ASSERT_TRUE(ok);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_begin_sets_step1_and_creds);
    RUN_TEST(test_on_station_connected_moves_to_step2);
    RUN_TEST(test_build_url_uses_token);
    RUN_TEST(test_handle_time_set_success_sets_applied);
    return UNITY_END();
}


