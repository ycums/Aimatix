#include <unity.h>
#include "StateManager.h"
#include "MainDisplayState.h"
#include "InputDisplayState.h"
#include "InputLogic.h"
#include <cstdio>
#include <cstring>
#include <vector>
#include <ctime>
#include "../mock/MockTimeProvider.h"
#include <memory>

// グローバル変数の宣言（定義はtest_globals.cppにある）
extern std::vector<time_t> alarm_times;

// テスト用の固定時刻
const time_t kFixedTestTime = 1700000000; // 任意の固定値
std::shared_ptr<MockTimeProvider> testTimeProvider = std::make_shared<MockTimeProvider>(kFixedTestTime);

void setUp(void) {}
void tearDown(void) {}

// 基本的なテストのみ
void test_basic_input_logic() {
    InputLogic logic(testTimeProvider);
    logic.reset();
    
    // 初期状態確認（reset()後の値）
    TEST_ASSERT_EQUAL(-1, logic.getValue());
    
    // 基本的な入力テスト
    logic.incrementInput(5);
    TEST_ASSERT_EQUAL(5, logic.getDigit(3));
}

void test_basic_state_manager() {
    StateManager sm;
    TEST_ASSERT_NOT_NULL(&sm);
}

// 未テスト関数のテストケース追加
void test_input_logic_shift_digits() {
    InputLogic logic(testTimeProvider);
    logic.reset();
    
    // 初期状態では桁送りできない
    TEST_ASSERT_FALSE(logic.shiftDigits());
    
    // 値を入力してから桁送り
    logic.incrementInput(5); // 分一の位に5を入力
    TEST_ASSERT_TRUE(logic.shiftDigits());
    
    // 桁送り後の状態確認
    TEST_ASSERT_EQUAL(5, logic.getDigit(2)); // 分十の位に移動
    TEST_ASSERT_TRUE(logic.isEntered(3));    // 分一の位は0で入力済み
    TEST_ASSERT_EQUAL(0, logic.getDigit(3)); // 分一の位は0
    TEST_ASSERT_TRUE(logic.isEntered(2));    // 分十の位は入力済み
}

void test_input_logic_get_absolute_value() {
    InputLogic logic(testTimeProvider);
    logic.reset();
    
    // 未入力状態では-1を返す
    time_t result = logic.getAbsoluteValue();
    TEST_ASSERT_EQUAL(-1, result);
    
    // 部分入力（05分）
    logic.incrementInput(0); // 分十の位
    logic.shiftDigits();
    logic.incrementInput(5); // 分一の位
    
    result = logic.getAbsoluteValue();
    TEST_ASSERT_NOT_EQUAL(-1, result);
    
    // 完全入力（12:34）
    logic.reset();
    logic.incrementInput(1); // 時十の位
    logic.shiftDigits();
    logic.incrementInput(2); // 時一の位
    logic.shiftDigits();
    logic.incrementInput(3); // 分十の位
    logic.shiftDigits();
    logic.incrementInput(4); // 分一の位
    
    result = logic.getAbsoluteValue();
    TEST_ASSERT_NOT_EQUAL(-1, result);
}

void test_input_logic_complete_input_value() {
    InputLogic logic(testTimeProvider);
    logic.reset();
    
    // 完全入力（12:34）
    logic.incrementInput(1); // 時十の位
    logic.shiftDigits();
    logic.incrementInput(2); // 時一の位
    logic.shiftDigits();
    logic.incrementInput(3); // 分十の位
    logic.shiftDigits();
    logic.incrementInput(4); // 分一の位
    
    // 完全入力時の値確認
    int value = logic.getValue();
    TEST_ASSERT_EQUAL(1234, value);
    
    // 各桁の値確認
    TEST_ASSERT_EQUAL(1, logic.getDigit(0)); // 時十の位
    TEST_ASSERT_EQUAL(2, logic.getDigit(1)); // 時一の位
    TEST_ASSERT_EQUAL(3, logic.getDigit(2)); // 分十の位
    TEST_ASSERT_EQUAL(4, logic.getDigit(3)); // 分一の位
    
    // 各桁の入力済み状態確認
    TEST_ASSERT_TRUE(logic.isEntered(0));
    TEST_ASSERT_TRUE(logic.isEntered(1));
    TEST_ASSERT_TRUE(logic.isEntered(2));
    TEST_ASSERT_TRUE(logic.isEntered(3));
}

void test_input_logic_shift_digits_edge_cases() {
    InputLogic logic(testTimeProvider);
    logic.reset();
    
    // 全桁入力済み状態で桁送りを試行
    logic.incrementInput(1); // 時十の位
    logic.shiftDigits();
    logic.incrementInput(2); // 時一の位
    logic.shiftDigits();
    logic.incrementInput(3); // 分十の位
    logic.shiftDigits();
    logic.incrementInput(4); // 分一の位
    
    // 全桁入力済み状態では桁送りできない
    TEST_ASSERT_FALSE(logic.shiftDigits());
    
    // 左端が入力済みの状態で桁送りを試行
    logic.reset();
    logic.incrementInput(1); // 時十の位
    logic.shiftDigits();
    logic.incrementInput(2); // 時一の位
    logic.shiftDigits();
    logic.incrementInput(3); // 分十の位
    logic.shiftDigits();
    logic.incrementInput(4); // 分一の位
    logic.shiftDigits(); // 左端まで移動
    
    // 左端が入力済み状態では桁送りできない
    TEST_ASSERT_FALSE(logic.shiftDigits());
}

// バグレポートのBA操作問題を再現・修正確認するテストケース
void test_ba_operation_bug_reproduction() {
    InputLogic logic(testTimeProvider);
    logic.reset();
    
    // バグレポートの状況を再現
    // 前提: 絶対値入力画面 初期状態（__:__）
    // 操作: BA
    // 0. 初期状態: __:__
    // 1. 無効な桁送りを試行 (Button B)
    bool shiftResult = logic.shiftDigits();
    TEST_ASSERT_FALSE(shiftResult); // 初期状態では桁送りは失敗すべき
    
    // 2. 数値を+1 (Button A)
    logic.incrementInput(1);
    
    // Expected: 00:01が表示されるべき
    // つまり、digits[3] = 1, その他は0のはず
    TEST_ASSERT_EQUAL(0, logic.getDigit(0)); // 時十の位
    TEST_ASSERT_EQUAL(0, logic.getDigit(1)); // 時一の位
    TEST_ASSERT_EQUAL(0, logic.getDigit(2)); // 分十の位
    TEST_ASSERT_EQUAL(1, logic.getDigit(3)); // 分一の位 - ここが1であるべき
    
    // 入力済み状態の確認
    TEST_ASSERT_FALSE(logic.isEntered(0)); // 時十の位
    TEST_ASSERT_FALSE(logic.isEntered(1)); // 時一の位
    TEST_ASSERT_FALSE(logic.isEntered(2)); // 分十の位
    TEST_ASSERT_TRUE(logic.isEntered(3));  // 分一の位のみ入力済み
    
    // PartialInputLogicによるプレビュー表示の確認（修正後の期待値）
    const int* digits = logic.getDigits();
    const bool* entered = logic.getEntered();
    auto parsedTime = PartialInputLogic::parsePartialInput(digits, entered);
    
    TEST_ASSERT_TRUE(parsedTime.isValid);
    TEST_ASSERT_EQUAL(0, parsedTime.hour);    // 時: 0
    TEST_ASSERT_EQUAL(1, parsedTime.minute);  // 分: 1 (修正後は00:01)
    
    std::string timeStr = PartialInputLogic::formatTime(parsedTime.hour, parsedTime.minute);
    TEST_ASSERT_EQUAL_STRING("00:01", timeStr.c_str()); // Expected: 00:01
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_basic_input_logic);
    RUN_TEST(test_basic_state_manager);
    RUN_TEST(test_input_logic_shift_digits);
    RUN_TEST(test_input_logic_get_absolute_value);
    RUN_TEST(test_input_logic_complete_input_value);
    RUN_TEST(test_input_logic_shift_digits_edge_cases);
    RUN_TEST(test_ba_operation_bug_reproduction);
    UNITY_END();
    return 0;
} 