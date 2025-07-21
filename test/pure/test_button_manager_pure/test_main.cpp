#include <unity.h>
#include "ButtonManager.h"

void test_short_press() {
    ButtonManager bm;
    uint32_t t = 1000;
    bm.update(ButtonManager::BUTTON_A, true, t);      // 押下
    bm.update(ButtonManager::BUTTON_A, false, t+100); // 100ms後に離す
    TEST_ASSERT_TRUE(bm.isShortPress(ButtonManager::BUTTON_A));
    bm.reset(ButtonManager::BUTTON_A);
}

void test_long_press() {
    ButtonManager bm;
    uint32_t t = 2000;
    bm.update(ButtonManager::BUTTON_B, true, t);      // 押下
    bm.update(ButtonManager::BUTTON_B, true, t+900);  // 900ms後も押下中
    TEST_ASSERT_TRUE(bm.isLongPress(ButtonManager::BUTTON_B));
    bm.update(ButtonManager::BUTTON_B, false, t+950); // 離す
    bm.reset(ButtonManager::BUTTON_B);
}

void test_debounce() {
    ButtonManager bm;
    uint32_t t = 3000;
    bm.update(ButtonManager::BUTTON_C, true, t);      // 押下
    bm.update(ButtonManager::BUTTON_C, false, t+10);  // 10ms後に離す（デバウンス）
    TEST_ASSERT_FALSE(bm.isShortPress(ButtonManager::BUTTON_C));
    bm.update(ButtonManager::BUTTON_C, false, t+100); // 100ms後に離す
    bm.update(ButtonManager::BUTTON_C, true, t+200);  // 再度押下
    bm.update(ButtonManager::BUTTON_C, false, t+300); // 100ms後に離す
    TEST_ASSERT_TRUE(bm.isShortPress(ButtonManager::BUTTON_C));
    bm.reset(ButtonManager::BUTTON_C);
}

void test_debounce_edge_and_reset() {
    ButtonManager bm;
    uint32_t t = 4000;
    // DEBOUNCE_MS未満の連打
    bm.update(ButtonManager::BUTTON_A, true, t);
    bm.update(ButtonManager::BUTTON_A, false, t+5); // 5ms後に離す（デバウンス）
    TEST_ASSERT_FALSE(bm.isShortPress(ButtonManager::BUTTON_A));
    // DEBOUNCE_MS超えた後の短押し
    bm.update(ButtonManager::BUTTON_A, false, t+100); // 100ms後に離す
    TEST_ASSERT_TRUE(bm.isShortPress(ButtonManager::BUTTON_A));
    // 長押し後の離し
    bm.update(ButtonManager::BUTTON_B, true, t);
    bm.update(ButtonManager::BUTTON_B, true, t+1000); // 1000ms後も押下中
    TEST_ASSERT_TRUE(bm.isLongPress(ButtonManager::BUTTON_B));
    bm.update(ButtonManager::BUTTON_B, false, t+1100); // 離す
    TEST_ASSERT_FALSE(bm.isLongPress(ButtonManager::BUTTON_B));
    // reset直後の状態
    bm.reset(ButtonManager::BUTTON_B);
    TEST_ASSERT_FALSE(bm.isShortPress(ButtonManager::BUTTON_B));
    TEST_ASSERT_FALSE(bm.isLongPress(ButtonManager::BUTTON_B));
    // 未使用ボタンの初期値
    ButtonManager bm2;
    TEST_ASSERT_FALSE(bm2.isShortPress(ButtonManager::BUTTON_C));
    TEST_ASSERT_FALSE(bm2.isLongPress(ButtonManager::BUTTON_C));
}

void setUp(void) {}
void tearDown(void) {}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_short_press);
    RUN_TEST(test_long_press);
    RUN_TEST(test_debounce);
    RUN_TEST(test_debounce_edge_and_reset);
    UNITY_END();
    return 0;
} 