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

void test_debounce_edge_and_error_cases() {
    // 初期値
    DebounceManager dm1(50);
    TEST_ASSERT_FALSE(dm1.getStable());
    // debounceMs=0
    DebounceManager dm2(0);
    uint32_t t = 1000;
    dm2.update(true, t);
    TEST_ASSERT_TRUE(dm2.getStable());
    dm2.update(false, t+1);
    TEST_ASSERT_FALSE(dm2.getStable());
    // 同じraw連続
    DebounceManager dm3(20);
    dm3.update(true, t);
    dm3.update(true, t+5);
    dm3.update(true, t+10);
    TEST_ASSERT_FALSE(dm3.getStable());
    dm3.update(true, t+25); // 25ms経過
    TEST_ASSERT_TRUE(dm3.getStable());
    // lastChangeが大きく進む（オーバーフロー対策）
    DebounceManager dm4(10);
    dm4.update(true, 0xFFFFFFF0);
    dm4.update(true, 0xFFFFFFF5);
    dm4.update(true, 0x100000000); // 16進数でオーバーフロー
    TEST_ASSERT_TRUE(dm4.getStable());
    // raw値が頻繁に変わる
    DebounceManager dm5(30);
    dm5.update(true, t);
    dm5.update(false, t+10);
    dm5.update(true, t+20);
    dm5.update(false, t+30);
    dm5.update(true, t+40);
    TEST_ASSERT_FALSE(dm5.getStable());
    dm5.update(true, t+80); // 40ms安定
    TEST_ASSERT_TRUE(dm5.getStable());
}

// getLastChange()のテスト
void test_debounce_get_last_change() {
    DebounceManager dm(50);
    uint32_t t = 1000;
    
    // 初期状態ではlastChangeは0
    TEST_ASSERT_EQUAL(0, dm.getLastChange());
    
    // 最初のupdateでlastChangeが設定される
    dm.update(true, t);
    TEST_ASSERT_EQUAL(t, dm.getLastChange());
    
    // 値が変わるとlastChangeが更新される
    dm.update(false, t+10);
    TEST_ASSERT_EQUAL(t+10, dm.getLastChange());
    
    // 同じ値が続いてもlastChangeは更新されない
    dm.update(false, t+20);
    TEST_ASSERT_EQUAL(t+10, dm.getLastChange());
    
    // 値が変わるとlastChangeが更新される
    dm.update(true, t+30);
    TEST_ASSERT_EQUAL(t+30, dm.getLastChange());
}

void setUp(void) {}
void tearDown(void) {}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_debounce_basic);
    RUN_TEST(test_debounce_edge);
    RUN_TEST(test_debounce_edge_and_error_cases);
    RUN_TEST(test_debounce_get_last_change);
    UNITY_END();
    return 0;
} 