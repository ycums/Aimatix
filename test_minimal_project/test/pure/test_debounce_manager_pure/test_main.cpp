#include <unity.h>

unsigned long mockMillis = 0;
unsigned long millis() { return mockMillis; }

void setUp() {}
void tearDown() {}

void test_dummy() {
    TEST_ASSERT_EQUAL_UINT32(0, mockMillis);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_dummy);
    return UNITY_END();
} 