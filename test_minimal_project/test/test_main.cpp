#include <unity.h>
#include <your_lib.h>

void setUp(void) {}
void tearDown(void) {}

void test_add() {
    TEST_ASSERT_EQUAL(5, add(2, 3));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_add);
    return UNITY_END();
} 