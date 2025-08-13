#include <unity.h>
#include "ButtonManager.h"

void test_short_press() {
    ButtonManager bm;
    uint32_t t = 1000;
    bm.update(ButtonManager::BtnA, true, t);      // 押下
    bm.update(ButtonManager::BtnA, false, t+100); // 100ms後に離す
    TEST_ASSERT_TRUE(bm.isShortPress(ButtonManager::BtnA));
    bm.reset(ButtonManager::BtnA);
}

void test_long_press() {
    ButtonManager bm;
    uint32_t t = 2000;
    bm.update(ButtonManager::BtnB, true, t);      // 押下
    bm.update(ButtonManager::BtnB, true, t+900);  // 900ms後も押下中
    TEST_ASSERT_TRUE(bm.isLongPress(ButtonManager::BtnB));
    bm.update(ButtonManager::BtnB, false, t+950); // 離す
    bm.reset(ButtonManager::BtnB);
}

void test_debounce() {
    ButtonManager bm;
    uint32_t t = 3000;
    bm.update(ButtonManager::BtnC, true, t);      // 押下
    bm.update(ButtonManager::BtnC, false, t+10);  // 10ms後に離す（デバウンス）
    TEST_ASSERT_FALSE(bm.isShortPress(ButtonManager::BtnC));
    bm.update(ButtonManager::BtnC, false, t+100); // 100ms後に離す
    bm.update(ButtonManager::BtnC, true, t+200);  // 再度押下
    bm.update(ButtonManager::BtnC, false, t+300); // 100ms後に離す
    TEST_ASSERT_TRUE(bm.isShortPress(ButtonManager::BtnC));
    bm.reset(ButtonManager::BtnC);
}

void test_debounce_edge_and_reset() {
    ButtonManager bm;
    uint32_t t = 4000;
    // DEBOUNCE_MS未満の連打
    bm.update(ButtonManager::BtnA, true, t);
    bm.update(ButtonManager::BtnA, false, t+5); // 5ms後に離す（デバウンス）
    TEST_ASSERT_FALSE(bm.isShortPress(ButtonManager::BtnA));
    // DEBOUNCE_MS超えた後の短押し
    bm.update(ButtonManager::BtnA, false, t+100); // 100ms後に離す
    TEST_ASSERT_TRUE(bm.isShortPress(ButtonManager::BtnA));
    // 長押し後の離し
    bm.update(ButtonManager::BtnB, true, t);
    bm.update(ButtonManager::BtnB, true, t+1000); // 1000ms後も押下中
    TEST_ASSERT_TRUE(bm.isLongPress(ButtonManager::BtnB));
    bm.update(ButtonManager::BtnB, false, t+1100); // 離す
    TEST_ASSERT_FALSE(bm.isLongPress(ButtonManager::BtnB));
    // reset直後の状態
    bm.reset(ButtonManager::BtnB);
    TEST_ASSERT_FALSE(bm.isShortPress(ButtonManager::BtnB));
    TEST_ASSERT_FALSE(bm.isLongPress(ButtonManager::BtnB));
    // 未使用ボタンの初期値
    ButtonManager bm2;
    TEST_ASSERT_FALSE(bm2.isShortPress(ButtonManager::BtnC));
    TEST_ASSERT_FALSE(bm2.isLongPress(ButtonManager::BtnC));
}

void test_press_down_edge_single_shot_and_debounce() {
    ButtonManager bm;
    uint32_t t = 5000;
    // 立ち上がりで1回だけtrue、その後押しっぱなしではfalse
    bm.update(ButtonManager::BtnA, true, t);      // 押下（エッジ）
    TEST_ASSERT_TRUE(bm.isPressDown(ButtonManager::BtnA));
    TEST_ASSERT_FALSE(bm.isPressDown(ButtonManager::BtnA));
    // デバウンス未満の反転は無視
    bm.update(ButtonManager::BtnA, false, t+10);  // 10msで離す（無視）
    bm.update(ButtonManager::BtnA, true, t+15);   // さらに押下（無視）
    TEST_ASSERT_FALSE(bm.isPressDown(ButtonManager::BtnA));
    // デバウンスを超えた後の再押下で再度true
    bm.update(ButtonManager::BtnA, false, t+200); // 安定した離し
    bm.update(ButtonManager::BtnA, true, t+260);  // 再押下（エッジ）
    TEST_ASSERT_TRUE(bm.isPressDown(ButtonManager::BtnA));
}

void setUp(void) {}
void tearDown(void) {}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_short_press);
    RUN_TEST(test_long_press);
    RUN_TEST(test_debounce);
    RUN_TEST(test_debounce_edge_and_reset);
    RUN_TEST(test_press_down_edge_single_shot_and_debounce);
    UNITY_END();
    return 0;
}
