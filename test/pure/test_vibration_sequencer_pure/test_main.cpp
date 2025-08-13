#include <unity.h>
#include "VibrationSequencer.h"
#include "../mock/MockVibrationOutput.h"

void setUp(void) {}
void tearDown(void) {}

int main(int, char**) {
    UNITY_BEGIN();

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

    return UNITY_END();
}


