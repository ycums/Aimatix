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
    mockBtn.setPressed(BUTTON_TYPE_A, true);
    TEST_ASSERT_TRUE(mockBtn.isPressed(BUTTON_TYPE_A));
    mockBtn.setLongPressed(BUTTON_TYPE_A, true);
    TEST_ASSERT_TRUE(mockBtn.isLongPressed(BUTTON_TYPE_A));
    mockBtn.reset();
    TEST_ASSERT_FALSE(mockBtn.isPressed(BUTTON_TYPE_A));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_mock_button_manager_basic);
    return UNITY_END();
} 