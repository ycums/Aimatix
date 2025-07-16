#include <unity.h>
#include <cstdio>
#include <cstring>
#include <map>
#include <vector>
#include "mock_button_manager.h"
#include "mock_time.h"
#include <button_manager.h>

// ButtonManagerの純粋ロジックテスト
// 新mock（MockButtonManager）を利用

void setUp(void) {}
void tearDown(void) {}

void test_mock_button_manager_basic() {
    MockButtonManager mockBtn;
    mockBtn.setPressed(0, true);
    TEST_ASSERT_TRUE(mockBtn.isPressed(0));
    mockBtn.setLongPressed(0, true);
    TEST_ASSERT_TRUE(mockBtn.isLongPressed(0));
    mockBtn.reset();
    TEST_ASSERT_FALSE(mockBtn.isPressed(0));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_mock_button_manager_basic);
    return UNITY_END();
} 