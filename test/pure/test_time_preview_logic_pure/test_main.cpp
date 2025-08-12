#include <unity.h>
#include "TimePreviewLogic.h"
#include "InputLogic.h"
#include "ITimeService.h"
#include <memory>
#include <string>

const time_t kFixedTestTime = 1700000000;
struct MockTimeService : public ITimeService {
    time_t n; uint32_t ms{0};
    explicit MockTimeService(time_t now): n(now) {}
    time_t now() const override { return n; }
    struct tm* localtime(time_t* t) const override { return ::localtime(t); }
    bool setSystemTime(time_t t) override { n = t; return true; }
    uint32_t monotonicMillis() const override { return ms; }
};
std::shared_ptr<MockTimeService> testTimeProvider = std::make_shared<MockTimeService>(kFixedTestTime);

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

// === AIM-11: 追加プレビュー検証 本体 ===
void test_preview_abs_00_00_from_23_59_is_plus1d_00_00() {
    // 現在時刻: 23:59
    struct tm base_tm = {};
    base_tm.tm_year = 124; base_tm.tm_mon = 0; base_tm.tm_mday = 1; 
    base_tm.tm_hour = 23; base_tm.tm_min = 59; base_tm.tm_sec = 0;
    time_t now = mktime(&base_tm);
    auto provider = std::make_shared<MockTimeService>(now);

    int digits[4] = {0,0,0,0};
    bool entered[4] = {true,true,true,true}; // 00:00 完全入力

    auto result = TimePreviewLogic::generatePreview(digits, entered, provider.get(), false);
    TEST_ASSERT_TRUE(result.isValid);
    TEST_ASSERT_EQUAL_STRING("+1d 00:00", result.preview.c_str());
}

void test_preview_abs_00_00_from_00_00_is_plus1d_00_00() {
    // 現在時刻: 00:00
    struct tm base_tm = {};
    base_tm.tm_year = 124; base_tm.tm_mon = 0; base_tm.tm_mday = 1; 
    base_tm.tm_hour = 0; base_tm.tm_min = 0; base_tm.tm_sec = 0;
    time_t now = mktime(&base_tm);
    auto provider = std::make_shared<MockTimeService>(now);

    int digits[4] = {0,0,0,0};
    bool entered[4] = {true,true,true,true};

    auto result = TimePreviewLogic::generatePreview(digits, entered, provider.get(), false);
    TEST_ASSERT_TRUE(result.isValid);
    TEST_ASSERT_EQUAL_STRING("+1d 00:00", result.preview.c_str());
}

void test_preview_abs___0_from_14_35_is_15_00() {
    // 現在時刻: 14:35、__:_0 入力
    struct tm base_tm = {};
    base_tm.tm_year = 124; base_tm.tm_mon = 0; base_tm.tm_mday = 1; 
    base_tm.tm_hour = 14; base_tm.tm_min = 35; base_tm.tm_sec = 0;
    time_t now = mktime(&base_tm);
    auto provider = std::make_shared<MockTimeService>(now);

    int digits[4] = {0,0,0,0};
    bool entered[4] = {false,false,false,true};

    auto result = TimePreviewLogic::generatePreview(digits, entered, provider.get(), false);
    TEST_ASSERT_TRUE(result.isValid);
    TEST_ASSERT_EQUAL_STRING("15:00", result.preview.c_str());
}

void test_preview_rel_plus_5_min_from_14_35_is_14_40() {
    // 相対: 5分
    struct tm base_tm = {};
    base_tm.tm_year = 124; base_tm.tm_mon = 0; base_tm.tm_mday = 1; 
    base_tm.tm_hour = 14; base_tm.tm_min = 35; base_tm.tm_sec = 0;
    time_t now = mktime(&base_tm);
    auto provider = std::make_shared<MockTimeService>(now);

    InputLogic logic(provider);
    logic.reset();
    logic.incrementInput(5); // 分一桁のみ 5
    time_t t = logic.getAbsoluteValue();
    auto result = TimePreviewLogic::generateRelativePreview(t, provider.get());
    TEST_ASSERT_TRUE(result.isValid);
    TEST_ASSERT_EQUAL_STRING("14:40", result.preview.c_str());
}

void test_preview_rel_plus_0_min_from_14_35_is_14_35() {
    // 相対: 0分
    struct tm base_tm = {};
    base_tm.tm_year = 124; base_tm.tm_mon = 0; base_tm.tm_mday = 1; 
    base_tm.tm_hour = 14; base_tm.tm_min = 35; base_tm.tm_sec = 0;
    time_t now = mktime(&base_tm);
    auto provider = std::make_shared<MockTimeService>(now);

    InputLogic logic(provider);
    logic.reset();
    logic.incrementInput(0); // 分一桁のみ 0
    time_t t = logic.getAbsoluteValue();
    auto result = TimePreviewLogic::generateRelativePreview(t, provider.get());
    TEST_ASSERT_TRUE(result.isValid);
    TEST_ASSERT_EQUAL_STRING("14:35", result.preview.c_str());
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

// Issue #8 再現テストケース: 絶対値入力モードで4:45→5:00入力時の問題
void test_time_preview_logic_absolute_input_4_45_to_5_00() {
    // 現在時刻: 4:45 を設定
    struct tm base_tm = {};
    base_tm.tm_year = 124; base_tm.tm_mon = 0; base_tm.tm_mday = 1; 
    base_tm.tm_hour = 4; base_tm.tm_min = 45; base_tm.tm_sec = 0;
    time_t now = mktime(&base_tm);
    
    // MockTimeProviderで時刻を固定
    auto timeProvider = std::make_shared<MockTimeService>(now);
    
    // _5:00 を入力
    int digits[4] = {0, 5, 0, 0};
    bool entered[4] = {false, true, true, true};
    
    auto result = TimePreviewLogic::generatePreview(digits, entered, timeProvider.get(), false);
    TEST_ASSERT_TRUE(result.isValid);
    
    // 期待値: 同日の5:00（+1d 5:00ではなく、5:00）
    TEST_ASSERT_EQUAL_STRING("05:00", result.preview.c_str());
}

// 類似ケースのテスト: 3:45→4:00
void test_time_preview_logic_absolute_input_3_45_to_4_00() {
    // 現在時刻: 3:45 → 入力: _4:00 → 期待値: 同日4:00
    struct tm base_tm = {};
    base_tm.tm_year = 124; base_tm.tm_mon = 0; base_tm.tm_mday = 1; 
    base_tm.tm_hour = 3; base_tm.tm_min = 45; base_tm.tm_sec = 0;
    time_t now = mktime(&base_tm);
    
    auto timeProvider = std::make_shared<MockTimeService>(now);
    
    // _4:00 を入力
    int digits[4] = {0, 4, 0, 0};
    bool entered[4] = {false, true, true, true};
    
    auto result = TimePreviewLogic::generatePreview(digits, entered, timeProvider.get(), false);
    TEST_ASSERT_TRUE(result.isValid);
    
    // 期待値: 同日の4:00
    TEST_ASSERT_EQUAL_STRING("04:00", result.preview.c_str());
}

// 類似ケースのテスト: 23:45→0:00
void test_time_preview_logic_absolute_input_23_45_to_0_00() {
    // 現在時刻: 23:45 → 入力: _0:00 → 期待値: 翌日0:00
    struct tm base_tm = {};
    base_tm.tm_year = 124; base_tm.tm_mon = 0; base_tm.tm_mday = 1; 
    base_tm.tm_hour = 23; base_tm.tm_min = 45; base_tm.tm_sec = 0;
    time_t now = mktime(&base_tm);
    
    auto timeProvider = std::make_shared<MockTimeService>(now);
    
    // _0:00 を入力
    int digits[4] = {0, 0, 0, 0};
    bool entered[4] = {false, true, true, true};
    
    auto result = TimePreviewLogic::generatePreview(digits, entered, timeProvider.get(), false);
    TEST_ASSERT_TRUE(result.isValid);
    
    // 期待値: 翌日の0:00
    TEST_ASSERT_EQUAL_STRING("+1d 00:00", result.preview.c_str());
}

// 類似ケースのテスト: 23:45→23:50
void test_time_preview_logic_absolute_input_23_45_to_23_50() {
    // 現在時刻: 23:45 → 入力: _23:50 → 期待値: 同日23:50
    struct tm base_tm = {};
    base_tm.tm_year = 124; base_tm.tm_mon = 0; base_tm.tm_mday = 1; 
    base_tm.tm_hour = 23; base_tm.tm_min = 45; base_tm.tm_sec = 0;
    time_t now = mktime(&base_tm);
    
    auto timeProvider = std::make_shared<MockTimeService>(now);
    
    // _23:50 を入力（23は2桁なので、時十=2, 時一=3、両方入力済み）
    int digits[4] = {2, 3, 5, 0};
    bool entered[4] = {true, true, true, true};
    
    auto result = TimePreviewLogic::generatePreview(digits, entered, timeProvider.get(), false);
    TEST_ASSERT_TRUE(result.isValid);
    
    // 期待値: 同日の23:50
    TEST_ASSERT_EQUAL_STRING("23:50", result.preview.c_str());
}

// Issue #8 問題再現テスト: 実際の問題が発生する可能性のあるケース
void test_time_preview_logic_issue_8_reproduction() {
    // 現在時刻: 4:45 を設定
    struct tm base_tm = {};
    base_tm.tm_year = 124; base_tm.tm_mon = 0; base_tm.tm_mday = 1; 
    base_tm.tm_hour = 4; base_tm.tm_min = 45; base_tm.tm_sec = 0;
    time_t now = mktime(&base_tm);
    
    auto timeProvider = std::make_shared<MockTimeService>(now);
    
    // _5:00 を入力
    int digits[4] = {0, 5, 0, 0};
    bool entered[4] = {false, true, true, true};
    
    auto result = TimePreviewLogic::generatePreview(digits, entered, timeProvider.get(), false);
    TEST_ASSERT_TRUE(result.isValid);
    
    // 期待値: 同日の5:00
    TEST_ASSERT_EQUAL_STRING("05:00", result.preview.c_str());
    

}

// Issue #8 問題再現テスト: 実際の問題が発生する可能性のある条件
void test_time_preview_logic_issue_8_edge_case() {
    // 現在時刻: 4:59 を設定
    struct tm base_tm = {};
    base_tm.tm_year = 124; base_tm.tm_mon = 0; base_tm.tm_mday = 1; 
    base_tm.tm_hour = 4; base_tm.tm_min = 59; base_tm.tm_sec = 0;
    time_t now = mktime(&base_tm);
    
    auto timeProvider = std::make_shared<MockTimeService>(now);
    
    // _5:00 を入力
    int digits[4] = {0, 5, 0, 0};
    bool entered[4] = {false, true, true, true};
    
    auto result = TimePreviewLogic::generatePreview(digits, entered, timeProvider.get(), false);
    TEST_ASSERT_TRUE(result.isValid);
    
    // 期待値: 同日の5:00
    TEST_ASSERT_EQUAL_STRING("05:00", result.preview.c_str());
    

}

// Issue #8 問題再現テスト: 4:59→5:00の詳細テスト
void test_time_preview_logic_issue_8_4_59_to_5_00() {
    // 現在時刻: 4:59 を設定
    struct tm base_tm = {};
    base_tm.tm_year = 124; base_tm.tm_mon = 0; base_tm.tm_mday = 1; 
    base_tm.tm_hour = 4; base_tm.tm_min = 59; base_tm.tm_sec = 0;
    time_t now = mktime(&base_tm);
    
    auto timeProvider = std::make_shared<MockTimeService>(now);
    
    // 5:00 を入力（完全入力）
    int digits[4] = {0, 5, 0, 0};
    bool entered[4] = {true, true, true, true};
    
    auto result = TimePreviewLogic::generatePreview(digits, entered, timeProvider.get(), false);
    TEST_ASSERT_TRUE(result.isValid);
    
    // 期待値: 同日の5:00
    TEST_ASSERT_EQUAL_STRING("05:00", result.preview.c_str());
    

}

// Issue #8 問題再現テスト: 4:55→5:00の詳細テスト（更新されたタイトル）
void test_time_preview_logic_issue_8_4_55_to_5_00() {
    // 現在時刻: 4:55 を設定
    struct tm base_tm = {};
    base_tm.tm_year = 124; base_tm.tm_mon = 0; base_tm.tm_mday = 1; 
    base_tm.tm_hour = 4; base_tm.tm_min = 55; base_tm.tm_sec = 0;
    time_t now = mktime(&base_tm);
    
    auto timeProvider = std::make_shared<MockTimeService>(now);
    
    // _5:00 を入力（部分入力）
    int digits[4] = {0, 5, 0, 0};
    bool entered[4] = {false, true, true, true};
    
    auto result = TimePreviewLogic::generatePreview(digits, entered, timeProvider.get(), false);
    TEST_ASSERT_TRUE(result.isValid);
    
    // 期待値: 同日の5:00
    TEST_ASSERT_EQUAL_STRING("05:00", result.preview.c_str());
    

}

// Issue #8 問題再現テスト: 実際の問題が発生する可能性のある条件
void test_time_preview_logic_issue_8_actual_problem() {
    // 現在時刻: 4:55 を設定
    struct tm base_tm = {};
    base_tm.tm_year = 124; base_tm.tm_mon = 0; base_tm.tm_mday = 1; 
    base_tm.tm_hour = 4; base_tm.tm_min = 55; base_tm.tm_sec = 0;
    time_t now = mktime(&base_tm);
    
    auto timeProvider = std::make_shared<MockTimeService>(now);
    
    // _5:00 を入力（部分入力）
    int digits[4] = {0, 5, 0, 0};
    bool entered[4] = {false, true, true, true};
    
    auto result = TimePreviewLogic::generatePreview(digits, entered, timeProvider.get(), false);
    TEST_ASSERT_TRUE(result.isValid);
    
    // 期待値: 同日の5:00
    TEST_ASSERT_EQUAL_STRING("05:00", result.preview.c_str());
    

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
    RUN_TEST(test_time_preview_logic_absolute_input_4_45_to_5_00);
    RUN_TEST(test_time_preview_logic_absolute_input_3_45_to_4_00);
    RUN_TEST(test_time_preview_logic_absolute_input_23_45_to_0_00);
    RUN_TEST(test_time_preview_logic_absolute_input_23_45_to_23_50);
    RUN_TEST(test_time_preview_logic_issue_8_reproduction);
    RUN_TEST(test_time_preview_logic_issue_8_edge_case);
    RUN_TEST(test_time_preview_logic_issue_8_4_59_to_5_00);
    RUN_TEST(test_time_preview_logic_issue_8_4_55_to_5_00);
    RUN_TEST(test_time_preview_logic_issue_8_actual_problem);
    
    // === AIM-11: 追加プレビュー検証 ===
    void test_preview_abs_00_00_from_23_59_is_plus1d_00_00();
    void test_preview_abs_00_00_from_00_00_is_plus1d_00_00();
    void test_preview_abs___0_from_14_35_is_15_00();
    void test_preview_rel_plus_5_min_from_14_35_is_14_40();
    void test_preview_rel_plus_0_min_from_14_35_is_14_35();
    
    RUN_TEST(test_preview_abs_00_00_from_23_59_is_plus1d_00_00);
    RUN_TEST(test_preview_abs_00_00_from_00_00_is_plus1d_00_00);
    RUN_TEST(test_preview_abs___0_from_14_35_is_15_00);
    RUN_TEST(test_preview_rel_plus_5_min_from_14_35_is_14_40);
    RUN_TEST(test_preview_rel_plus_0_min_from_14_35_is_14_35);
    
    UNITY_END();
    return 0;
} 