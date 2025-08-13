#include <unity.h>
#include "AlarmFlashScheduler.h"
#include "VibrationSequencer.h"
#include "IVibration.h"

void setUp(void) {}
void tearDown(void) {}

static void test_begin_then_on_at_0ms() {
    AlarmFlashScheduler s; s.begin(1000);
    TEST_ASSERT_TRUE(s.update(1000));
}

static void test_125ms_is_off() {
    AlarmFlashScheduler s; s.begin(0);
    TEST_ASSERT_FALSE(s.update(125));
}

static void test_250ms_is_on() {
    AlarmFlashScheduler s; s.begin(0);
    TEST_ASSERT_TRUE(s.update(250));
}

static void test_375ms_is_off() {
    AlarmFlashScheduler s; s.begin(0);
    TEST_ASSERT_FALSE(s.update(375));
}

static void test_750ms_is_off() {
    AlarmFlashScheduler s; s.begin(0);
    TEST_ASSERT_FALSE(s.update(750));
}

static void test_4000ms_is_finished() {
    AlarmFlashScheduler s; s.begin(0);
    (void)s.update(4000);
    TEST_ASSERT_TRUE(s.isFinished());
}

static void test_after_finished_is_always_off() {
    AlarmFlashScheduler s; s.begin(0);
    (void)s.update(4001);
    TEST_ASSERT_FALSE(s.update(4500));
}

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_begin_then_on_at_0ms);
    RUN_TEST(test_125ms_is_off);
    RUN_TEST(test_250ms_is_on);
    RUN_TEST(test_375ms_is_off);
    RUN_TEST(test_750ms_is_off);
    RUN_TEST(test_4000ms_is_finished);
    RUN_TEST(test_after_finished_is_always_off);
    // --- VibrationSequencer tests ---
    {
        // local mock
        class MockVibrationOut : public IVibrationOutput {
        public:
            uint8_t lastDuty = 0;
            void setDutyPercent(uint8_t dutyPercent) override { lastDuty = dutyPercent; }
        };

        auto test_starts_active = [](){
            VibrationSequencer seq;
            seq.loadPattern({ {100, 50}, {100, 0}, {100, 50} }, false);
            seq.start(0);
            TEST_ASSERT_TRUE(seq.isActive());
        };
        auto test_duty_in_first_segment = [](){
            VibrationSequencer seq; MockVibrationOut out;
            seq.loadPattern({ {100, 50}, {100, 0}, {100, 50} }, false);
            seq.start(0);
            seq.update(50, &out);
            TEST_ASSERT_EQUAL_UINT8(50, out.lastDuty);
        };
        auto test_duty_in_second_segment = [](){
            VibrationSequencer seq; MockVibrationOut out;
            seq.loadPattern({ {100, 50}, {100, 0}, {100, 50} }, false);
            seq.start(0);
            seq.update(150, &out);
            TEST_ASSERT_EQUAL_UINT8(0, out.lastDuty);
        };
        auto test_stops_after_oneshot = [](){
            VibrationSequencer seq; MockVibrationOut out;
            seq.loadPattern({ {100, 50}, {100, 0}, {100, 50} }, false);
            seq.start(0);
            seq.update(350, &out);
            TEST_ASSERT_FALSE(seq.isActive());
        };

        RUN_TEST(test_starts_active);
        RUN_TEST(test_duty_in_first_segment);
        RUN_TEST(test_duty_in_second_segment);
        RUN_TEST(test_stops_after_oneshot);
    }
    return UNITY_END();
}


