#include <unity.h>
#include "AlarmFlashScheduler.h"

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
    return UNITY_END();
}


