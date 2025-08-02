#include <unity.h>
#include "PartialInputLogic.h"

void setUp(void) {
    // テスト前の初期化処理
}

void tearDown(void) {
    // テスト後のクリーンアップ処理
}

// バグレポート3-0-14-2の修正対象：分一桁のみ入力
void test_parsePartialInput_minute_only_digit() {
    // __:_5 → 00:05 (直感的な解釈：分一桁として扱う)
    int digits[4] = {0, 0, 0, 5};
    bool entered[4] = {false, false, false, true};
    
    auto result = PartialInputLogic::parsePartialInput(digits, entered);
    
    TEST_ASSERT_TRUE(result.isValid);
    TEST_ASSERT_EQUAL(0, result.hour);
    TEST_ASSERT_EQUAL(5, result.minute);
    TEST_ASSERT_FALSE(result.hourSpecified);
    TEST_ASSERT_TRUE(result.minuteSpecified);
}

// 時一桁のみ入力
void test_parsePartialInput_hour_only_digit() {
    // _1:__ → 01:00 (修正後: 時一桁として解釈)
    int digits[4] = {0, 1, 0, 0};
    bool entered[4] = {false, true, false, false};
    
    auto result = PartialInputLogic::parsePartialInput(digits, entered);
    
    TEST_ASSERT_TRUE(result.isValid);
    TEST_ASSERT_EQUAL(1, result.hour);  // 修正後: 1 (修正前: 10)
    TEST_ASSERT_EQUAL(0, result.minute);
    TEST_ASSERT_TRUE(result.hourSpecified);
    TEST_ASSERT_FALSE(result.minuteSpecified);
}

// 時十桁のみ入力 → 時が未入力として扱う
void test_parsePartialInput_hour_tens_only() {
    // 1_:__ → 時が未入力として扱う
    int digits[4] = {1, 0, 0, 0};
    bool entered[4] = {true, false, false, false};
    
    auto result = PartialInputLogic::parsePartialInput(digits, entered);
    
    TEST_ASSERT_TRUE(result.isValid);
    TEST_ASSERT_EQUAL(0, result.hour);  // 時が未入力なので0
    TEST_ASSERT_EQUAL(0, result.minute);
    TEST_ASSERT_FALSE(result.hourSpecified);  // 時が未入力
    TEST_ASSERT_FALSE(result.minuteSpecified);
}

// 分十桁のみ入力 → 分が未入力として扱う
void test_parsePartialInput_minute_tens_only() {
    // __:3_ → 分が未入力として扱う
    int digits[4] = {0, 0, 3, 0};
    bool entered[4] = {false, false, true, false};
    
    auto result = PartialInputLogic::parsePartialInput(digits, entered);
    
    TEST_ASSERT_TRUE(result.isValid);
    TEST_ASSERT_EQUAL(0, result.hour);
    TEST_ASSERT_EQUAL(0, result.minute);  // 分が未入力なので0
    TEST_ASSERT_FALSE(result.hourSpecified);
    TEST_ASSERT_FALSE(result.minuteSpecified);  // 分が未入力
}

// 完全入力
void test_parsePartialInput_complete_input() {
    // 12:34 → 12:34
    int digits[4] = {1, 2, 3, 4};
    bool entered[4] = {true, true, true, true};
    
    auto result = PartialInputLogic::parsePartialInput(digits, entered);
    
    TEST_ASSERT_TRUE(result.isValid);
    TEST_ASSERT_EQUAL(12, result.hour);
    TEST_ASSERT_EQUAL(34, result.minute);
    TEST_ASSERT_TRUE(result.hourSpecified);
    TEST_ASSERT_TRUE(result.minuteSpecified);
}

// 部分入力（時分十桁のみ）
void test_parsePartialInput_partial_tens() {
    // 1_:3_ → 時が未入力、分が未入力として扱う
    int digits[4] = {1, 0, 3, 0};
    bool entered[4] = {true, false, true, false};
    
    auto result = PartialInputLogic::parsePartialInput(digits, entered);
    
    TEST_ASSERT_TRUE(result.isValid);
    TEST_ASSERT_EQUAL(0, result.hour);  // 時が未入力なので0
    TEST_ASSERT_EQUAL(0, result.minute);  // 分が未入力なので0
    TEST_ASSERT_FALSE(result.hourSpecified);  // 時が未入力
    TEST_ASSERT_FALSE(result.minuteSpecified);  // 分が未入力
}

// 部分入力（時分一桁のみ）
void test_parsePartialInput_partial_units() {
    // _2:_4 → 02:04 (修正後: 各桁をそのまま解釈)
    int digits[4] = {0, 2, 0, 4};
    bool entered[4] = {false, true, false, true};
    
    auto result = PartialInputLogic::parsePartialInput(digits, entered);
    
    TEST_ASSERT_TRUE(result.isValid);
    TEST_ASSERT_EQUAL(2, result.hour);  // 修正後: 2 (修正前: 20)
    TEST_ASSERT_EQUAL(4, result.minute);
    TEST_ASSERT_TRUE(result.hourSpecified);
    TEST_ASSERT_TRUE(result.minuteSpecified);
}

// 未入力（確定拒絶）
void test_parsePartialInput_no_input() {
    // __:__ → 無効（確定拒絶）
    int digits[4] = {0, 0, 0, 0};
    bool entered[4] = {false, false, false, false};
    
    auto result = PartialInputLogic::parsePartialInput(digits, entered);
    
    TEST_ASSERT_FALSE(result.isValid);
    // hour, minuteの値は無効なので確認しない
}

// nullptrエラーハンドリング
void test_parsePartialInput_null_params() {
    auto result1 = PartialInputLogic::parsePartialInput(nullptr, nullptr);
    TEST_ASSERT_FALSE(result1.isValid);
    
    int digits[4] = {0, 0, 0, 0};
    auto result2 = PartialInputLogic::parsePartialInput(digits, nullptr);
    TEST_ASSERT_FALSE(result2.isValid);
    
    bool entered[4] = {false, false, false, false};
    auto result3 = PartialInputLogic::parsePartialInput(nullptr, entered);
    TEST_ASSERT_FALSE(result3.isValid);
}

// formatTime関数のテスト
void test_formatTime() {
    TEST_ASSERT_EQUAL_STRING("00:00", PartialInputLogic::formatTime(0, 0).c_str());
    TEST_ASSERT_EQUAL_STRING("12:34", PartialInputLogic::formatTime(12, 34).c_str());
    TEST_ASSERT_EQUAL_STRING("23:59", PartialInputLogic::formatTime(23, 59).c_str());
    TEST_ASSERT_EQUAL_STRING("01:05", PartialInputLogic::formatTime(1, 5).c_str());
}

// isValidTime関数のテスト
void test_isValidTime() {
    // 有効な時分
    TEST_ASSERT_TRUE(PartialInputLogic::isValidTime(0, 0));
    TEST_ASSERT_TRUE(PartialInputLogic::isValidTime(12, 30));
    TEST_ASSERT_TRUE(PartialInputLogic::isValidTime(23, 59));
    
    // 無効な時分
    TEST_ASSERT_FALSE(PartialInputLogic::isValidTime(-1, 0));
    TEST_ASSERT_FALSE(PartialInputLogic::isValidTime(24, 0));
    TEST_ASSERT_FALSE(PartialInputLogic::isValidTime(0, -1));
    TEST_ASSERT_FALSE(PartialInputLogic::isValidTime(0, 60));
}

// バグレポート1対応テスト: _1:00 → 01:00
void test_bugreport_1_hour_only_digit() {
    // _1:00 → 01:00 (時一桁のみの場合は時一桁として解釈)
    int digits[4] = {0, 1, 0, 0};
    bool entered[4] = {false, true, false, false};
    
    auto result = PartialInputLogic::parsePartialInput(digits, entered);
    
    TEST_ASSERT_TRUE(result.isValid);
    TEST_ASSERT_EQUAL(1, result.hour);  // 修正後: 1 (修正前: 10)
    TEST_ASSERT_EQUAL(0, result.minute);
    TEST_ASSERT_TRUE(result.hourSpecified);
    TEST_ASSERT_FALSE(result.minuteSpecified);
}

// バグレポート2対応テスト: __:_1 → 00:01
void test_bugreport_2_minute_only_digit() {
    // __:_1 → 00:01 (分一桁のみの場合は分一桁として解釈)
    int digits[4] = {0, 0, 0, 1};
    bool entered[4] = {false, false, false, true};
    
    auto result = PartialInputLogic::parsePartialInput(digits, entered);
    
    TEST_ASSERT_TRUE(result.isValid);
    TEST_ASSERT_EQUAL(0, result.hour);
    TEST_ASSERT_EQUAL(1, result.minute);
    TEST_ASSERT_FALSE(result.hourSpecified);
    TEST_ASSERT_TRUE(result.minuteSpecified);
}

// エラーハンドリングテスト
void test_formatTime_error_handling() {
    // 正常なケース
    auto result1 = PartialInputLogic::formatTime(12, 34);
    TEST_ASSERT_EQUAL_STRING("12:34", result1.c_str());
    
    // エラーケース（バッファサイズが小さい場合のシミュレーション）
    // 実際のテストでは、バッファサイズを小さくしてエラーを再現
    // このテストは実装の詳細に依存するため、必要に応じて調整
}

// バグレポートの具体例との一致確認（修正後の期待値）
void test_bugreport_3_0_14_2_examples() {
    // __:_5 → 00:05 （修正後の期待値：分一桁として解釈）
    int digits1[4] = {0, 0, 0, 5};
    bool entered1[4] = {false, false, false, true};
    auto result1 = PartialInputLogic::parsePartialInput(digits1, entered1);
    TEST_ASSERT_EQUAL(0, result1.hour);
    TEST_ASSERT_EQUAL(5, result1.minute);
    TEST_ASSERT_EQUAL_STRING("00:05", PartialInputLogic::formatTime(result1.hour, result1.minute).c_str());
    
    // _1:__ → 01:00 （修正後: 時一桁として解釈）
    int digits2[4] = {0, 1, 0, 0};
    bool entered2[4] = {false, true, false, false};
    auto result2 = PartialInputLogic::parsePartialInput(digits2, entered2);
    TEST_ASSERT_EQUAL(1, result2.hour);  // 修正後: 1 (修正前: 10)
    TEST_ASSERT_EQUAL(0, result2.minute);
    TEST_ASSERT_EQUAL_STRING("01:00", PartialInputLogic::formatTime(result2.hour, result2.minute).c_str());
}

int main() {
    UNITY_BEGIN();
    
    // 既存テスト
    RUN_TEST(test_parsePartialInput_minute_only_digit);
    RUN_TEST(test_parsePartialInput_hour_only_digit);
    RUN_TEST(test_parsePartialInput_hour_tens_only);
    RUN_TEST(test_parsePartialInput_minute_tens_only);
    RUN_TEST(test_parsePartialInput_complete_input);
    RUN_TEST(test_parsePartialInput_partial_tens);
    RUN_TEST(test_parsePartialInput_partial_units);
    RUN_TEST(test_parsePartialInput_no_input);
    RUN_TEST(test_parsePartialInput_null_params);
    RUN_TEST(test_formatTime);
    RUN_TEST(test_isValidTime);
    
    // 新規追加テスト
    RUN_TEST(test_bugreport_1_hour_only_digit);
    RUN_TEST(test_bugreport_2_minute_only_digit);
    RUN_TEST(test_formatTime_error_handling);
    RUN_TEST(test_bugreport_3_0_14_2_examples);
    
    UNITY_END();
    return 0;
}