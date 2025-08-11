#include <unity.h>
#include "TimeZoneUtil.h"

void setUp() {}
void tearDown() {}

static void test_tz_zero(void) {
    std::string tz = TimeZoneUtil::buildPosixTzFromOffsetMinutes(0);
    TEST_ASSERT_EQUAL_STRING("GMT0", tz.c_str());
}

static void test_tz_jst_plus_9h(void) {
    std::string tz = TimeZoneUtil::buildPosixTzFromOffsetMinutes(540); // +9:00
    TEST_ASSERT_EQUAL_STRING("GMT-9", tz.c_str());
}

static void test_tz_minus_4h(void) {
    std::string tz = TimeZoneUtil::buildPosixTzFromOffsetMinutes(-240); // -4:00
    TEST_ASSERT_EQUAL_STRING("GMT+4", tz.c_str());
}

static void test_tz_plus_5h30(void) {
    std::string tz = TimeZoneUtil::buildPosixTzFromOffsetMinutes(330); // +5:30
    TEST_ASSERT_EQUAL_STRING("GMT-5:30", tz.c_str());
}

static void test_tz_minus_9h30(void) {
    std::string tz = TimeZoneUtil::buildPosixTzFromOffsetMinutes(-570); // -9:30
    TEST_ASSERT_EQUAL_STRING("GMT+9:30", tz.c_str());
}

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_tz_zero);
    RUN_TEST(test_tz_jst_plus_9h);
    RUN_TEST(test_tz_minus_4h);
    RUN_TEST(test_tz_plus_5h30);
    RUN_TEST(test_tz_minus_9h30);
    return UNITY_END();
}


