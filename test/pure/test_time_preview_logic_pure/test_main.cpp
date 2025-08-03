#include <unity.h>
#include "TimePreviewLogic.h"
#include "../mock/MockTimeProvider.h"
#include <memory>
#include <string>

const time_t kFixedTestTime = 1700000000;
std::shared_ptr<MockTimeProvider> testTimeProvider = std::make_shared<MockTimeProvider>(kFixedTestTime);

void setUp(void) {}
void tearDown(void) {}

// 基本的なプレビュー生成テスト
void test_time_preview_logic_basic_preview() {
    int digits[4] = {1, 2, 3, 4};
    bool entered[4] = {true, true, true, true};
    
    auto result = TimePreviewLogic::generatePreview(digits, entered, testTimeProvider.get(), false);
    
    TEST_ASSERT_TRUE(result.isValid);
    TEST_ASSERT_FALSE(result.preview.empty());
}

// 相対値プレビュー生成テスト
void test_time_preview_logic_relative_preview() {
    time_t relativeTime = kFixedTestTime + 3600; // 1時間後
    
    auto result = TimePreviewLogic::generateRelativePreview(relativeTime, testTimeProvider.get());
    
    TEST_ASSERT_TRUE(result.isValid);
    TEST_ASSERT_FALSE(result.preview.empty());
}

// 部分入力のプレビュー生成テスト
void test_time_preview_logic_partial_input() {
    int digits[4] = {1, 2, -1, -1};
    bool entered[4] = {true, true, false, false};
    
    auto result = TimePreviewLogic::generatePreview(digits, entered, testTimeProvider.get(), false);
    
    TEST_ASSERT_TRUE(result.isValid);
    TEST_ASSERT_FALSE(result.preview.empty());
}

// 無効入力のプレビュー生成テスト
void test_time_preview_logic_invalid_input() {
    int digits[4] = {-1, -1, -1, -1};
    bool entered[4] = {false, false, false, false};
    
    auto result = TimePreviewLogic::generatePreview(digits, entered, testTimeProvider.get(), false);
    
    TEST_ASSERT_FALSE(result.isValid);
}

// nullptr入力のプレビュー生成テスト
void test_time_preview_logic_null_input() {
    auto result = TimePreviewLogic::generatePreview(nullptr, nullptr, testTimeProvider.get(), false);
    
    TEST_ASSERT_FALSE(result.isValid);
}

// 相対値モードのプレビュー生成テスト
void test_time_preview_logic_relative_mode() {
    int digits[4] = {1, 2, 3, 4};
    bool entered[4] = {true, true, true, true};
    
    auto result = TimePreviewLogic::generatePreview(digits, entered, testTimeProvider.get(), true);
    
    // 相対値モードでは相対値の計算は呼び出し側で行うため、isValidはfalse
    TEST_ASSERT_FALSE(result.isValid);
}

// 無効な相対時刻のプレビュー生成テスト
void test_time_preview_logic_invalid_relative_time() {
    time_t invalidTime = -1;
    
    auto result = TimePreviewLogic::generateRelativePreview(invalidTime, testTimeProvider.get());
    
    TEST_ASSERT_FALSE(result.isValid);
}

// 過去時刻のプレビュー生成テスト
void test_time_preview_logic_past_time() {
    int digits[4] = {0, 0, 0, 0}; // 00:00
    bool entered[4] = {true, true, true, true};
    
    auto result = TimePreviewLogic::generatePreview(digits, entered, testTimeProvider.get(), false);
    
    TEST_ASSERT_TRUE(result.isValid);
    TEST_ASSERT_FALSE(result.preview.empty());
}

// 日付跨ぎのプレビュー生成テスト
void test_time_preview_logic_next_day() {
    int digits[4] = {2, 3, 5, 9}; // 23:59
    bool entered[4] = {true, true, true, true};
    
    auto result = TimePreviewLogic::generatePreview(digits, entered, testTimeProvider.get(), false);
    
    TEST_ASSERT_TRUE(result.isValid);
    TEST_ASSERT_FALSE(result.preview.empty());
}

// エッジケースのプレビュー生成テスト
void test_time_preview_logic_edge_cases() {
    // 最大時刻
    int digits1[4] = {2, 3, 5, 9};
    bool entered1[4] = {true, true, true, true};
    auto result1 = TimePreviewLogic::generatePreview(digits1, entered1, testTimeProvider.get(), false);
    TEST_ASSERT_TRUE(result1.isValid);
    
    // 最小時刻
    int digits2[4] = {0, 0, 0, 0};
    bool entered2[4] = {true, true, true, true};
    auto result2 = TimePreviewLogic::generatePreview(digits2, entered2, testTimeProvider.get(), false);
    TEST_ASSERT_TRUE(result2.isValid);
}

// 分岐カバレッジ向上のためのテスト
void test_time_preview_logic_branch_coverage() {
    // nullptr timeProvider
    int digits[4] = {1, 2, 3, 4};
    bool entered[4] = {true, true, true, true};
    
    auto result = TimePreviewLogic::generatePreview(digits, entered, nullptr, false);
    TEST_ASSERT_FALSE(result.isValid);
    
    // 相対値プレビューでnullptr timeProvider
    time_t relativeTime = kFixedTestTime + 3600;
    auto relativeResult = TimePreviewLogic::generateRelativePreview(relativeTime, nullptr);
    TEST_ASSERT_FALSE(relativeResult.isValid);
}

// 包括的なテスト
void test_time_preview_logic_comprehensive() {
    // 様々な入力パターンをテスト
    for (int hour = 0; hour < 24; hour += 6) {
        for (int minute = 0; minute < 60; minute += 15) {
            int digits[4] = {hour / 10, hour % 10, minute / 10, minute % 10};
            bool entered[4] = {true, true, true, true};
            
            auto result = TimePreviewLogic::generatePreview(digits, entered, testTimeProvider.get(), false);
            TEST_ASSERT_TRUE(result.isValid);
        }
    }
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    
    RUN_TEST(test_time_preview_logic_basic_preview);
    RUN_TEST(test_time_preview_logic_relative_preview);
    RUN_TEST(test_time_preview_logic_partial_input);
    RUN_TEST(test_time_preview_logic_invalid_input);
    RUN_TEST(test_time_preview_logic_null_input);
    RUN_TEST(test_time_preview_logic_relative_mode);
    RUN_TEST(test_time_preview_logic_invalid_relative_time);
    RUN_TEST(test_time_preview_logic_past_time);
    RUN_TEST(test_time_preview_logic_next_day);
    RUN_TEST(test_time_preview_logic_edge_cases);
    RUN_TEST(test_time_preview_logic_branch_coverage);
    RUN_TEST(test_time_preview_logic_comprehensive);
    
    UNITY_END();
    return 0;
} 