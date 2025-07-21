#include <unity.h>
#include "DebounceManager.h"

void test_debounce_basic() {
    DebounceManager dm(50);
    uint32_t t = 1000;
    // 最初はfalse
    TEST_ASSERT_FALSE(dm.getStable());
    // 10msだけON→OFF（デバウンス未満）
    dm.update(true, t+10);
    TEST_ASSERT_FALSE(dm.getStable());
    dm.update(false, t+20);
    TEST_ASSERT_FALSE(dm.getStable());
    // 60ms間ON→安定
    dm.update(true, t+100);
    dm.update(true, t+160);
    TEST_ASSERT_TRUE(dm.getStable());
}

void test_debounce_edge() {
    DebounceManager dm(30);
    uint32_t t = 2000;
    dm.update(true, t+10);
    dm.update(true, t+40); // 30ms経過
    TEST_ASSERT_TRUE(dm.getStable());
    dm.update(false, t+50);
    dm.update(false, t+90); // 40ms経過
    TEST_ASSERT_FALSE(dm.getStable());
}

void setUp(void) {}
void tearDown(void) {}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_debounce_basic);
    RUN_TEST(test_debounce_edge);
    UNITY_END();
    return 0;
} 