#include <unity.h>
#include "TimeLogic.h"

void test_time_logic_stub() {
    TEST_ASSERT_EQUAL(12, TimeLogic::getHour());
    TEST_ASSERT_EQUAL(0, TimeLogic::getMinute());
}

void setUp(void) {}
void tearDown(void) {}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_time_logic_stub);
    UNITY_END();
    return 0;
} 