#include <unity.h>
#include <vector>
#include <ctime>
#include "AlarmRolloverDetector.h"

void setUp(void) {}
void tearDown(void) {}

static void test_rollover_true_when_front_removed_at_now() {
    AlarmRolloverDetector d;
    // frame n-1: alarms [100, 200], now=99 => prevNext=100, no trigger
    {
        std::vector<time_t> alarms = {100, 200};
        TEST_ASSERT_FALSE(d.onFrame(alarms, 99));
    }
    // frame n: removePastAlarms後を想定 → [200], now=100 => trigger true
    {
        std::vector<time_t> alarms = {200};
        TEST_ASSERT_TRUE(d.onFrame(alarms, 100));
    }
}

static void test_no_trigger_when_all_future_deleted() {
    AlarmRolloverDetector d;
    // frame n-1: alarms [150], now=100 => set prevNext=150
    {
        std::vector<time_t> alarms = {150};
        TEST_ASSERT_FALSE(d.onFrame(alarms, 100));
    }
    // user deletes all future alarms → empty, now=100 (< prevNext)
    {
        std::vector<time_t> alarms;
        TEST_ASSERT_FALSE(d.onFrame(alarms, 100));
    }
}

static void test_trigger_only_once() {
    AlarmRolloverDetector d;
    // frame 1: [100, 200], now=99 -> no trigger
    TEST_ASSERT_FALSE(d.onFrame(std::vector<time_t>{100,200}, 99));
    // frame 2: [200], now=100 -> trigger
    TEST_ASSERT_TRUE(d.onFrame(std::vector<time_t>{200}, 100));
    // frame 3: [200], now=101 -> no trigger (already rolled over)
    TEST_ASSERT_FALSE(d.onFrame(std::vector<time_t>{200}, 101));
}

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_rollover_true_when_front_removed_at_now);
    RUN_TEST(test_no_trigger_when_all_future_deleted);
    RUN_TEST(test_trigger_only_once);
    return UNITY_END();
}


