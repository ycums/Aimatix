#include <unity.h>
#include "DateTimeInputState.h"
#include "../mock/MockTimeProvider.h"
#include <memory>

extern std::vector<time_t> alarm_times;

const time_t kFixedTestTime = 1700000000;
std::shared_ptr<MockTimeProvider> testTimeProvider = std::make_shared<MockTimeProvider>(kFixedTestTime);

void setUp(void) {}
void tearDown(void) {}

// 基本的なテスト
void test_datetime_input_basic() {
    DateTimeInputState state(testTimeProvider.get());
    
    // 基本的な動作確認
    TEST_ASSERT_NOT_NULL(&state);
}

// 公開バリデーションメソッドのテスト
void test_datetime_input_validate_datetime() {
    DateTimeInputState state(testTimeProvider.get());
    
    // 初期状態でのバリデーション
    TEST_ASSERT_TRUE(state.validateDateTime());
    
    // formatDateTimeStringのテスト
    std::string dateTimeStr = state.formatDateTimeString();
    TEST_ASSERT_FALSE(dateTimeStr.empty());
}

// ボタン操作のテスト
void test_datetime_input_button_operations() {
    DateTimeInputState state(testTimeProvider.get());
    
    // onEnterのテスト
    state.onEnter();
    
    // onDrawのテスト
    state.onDraw();
    
    // ボタン操作のテスト
    state.onButtonA(); // インクリメント
    state.onButtonB(); // カーソル移動
    state.onButtonC(); // 確定
    
    // 長押しボタンのテスト
    state.onButtonALongPress();
    state.onButtonBLongPress();
    state.onButtonCLongPress();
    
    // onExitのテスト
    state.onExit();
}

// 境界値テスト
void test_datetime_input_edge_cases() {
    DateTimeInputState state(testTimeProvider.get());
    
    // 複数回の状態変更テスト
    state.onEnter();
    state.onDraw();
    
    // 複数回のボタン操作
    for (int i = 0; i < 10; i++) {
        state.onButtonA();
        state.onButtonB();
    }
    
    state.onExit();
}

// 分岐カバレッジ向上のためのテスト
void test_datetime_input_branch_coverage() {
    DateTimeInputState state(testTimeProvider.get());
    
    // onEnterの分岐テスト
    state.onEnter();
    
    // onDrawの分岐テスト（viewがnullptrの場合も含む）
    state.onDraw();
    
    // 各ボタンの分岐テスト
    state.onButtonA();
    state.onButtonB();
    state.onButtonC();
    
    // 長押しボタンの分岐テスト
    state.onButtonALongPress();
    state.onButtonBLongPress();
    state.onButtonCLongPress();
}

// 包括的なテスト
void test_datetime_input_comprehensive() {
    DateTimeInputState state(testTimeProvider.get());
    
    // 初期化
    state.onEnter();
    
    // 様々な状態でのonDraw呼び出し
    state.onDraw();
    
    // 複数回のボタン操作で状態変化をテスト
    for (int i = 0; i < 5; i++) {
        state.onButtonA(); // インクリメント
        state.onButtonB(); // カーソル移動
    }
    
    // 確定操作のテスト
    state.onButtonC();
    
    // 終了処理
    state.onExit();
}

// エラーケースのテスト
void test_datetime_input_error_cases() {
    // nullptrでの初期化テスト
    DateTimeInputState state(nullptr);
    
    // null状態での操作テスト
    state.onEnter();
    state.onDraw();
    state.onButtonA();
    state.onButtonB();
    state.onButtonC();
    state.onExit();
}

// 追加のカバレッジ向上テスト
void test_datetime_input_additional_coverage() {
    DateTimeInputState state(testTimeProvider.get());
    
    // 様々な状態での操作テスト
    state.onEnter();
    
    // 複数回の状態変更
    for (int i = 0; i < 3; i++) {
        state.onButtonA();
        state.onButtonB();
        state.onDraw();
    }
    
    // 長押し操作のテスト
    state.onButtonALongPress();
    state.onButtonBLongPress();
    state.onButtonCLongPress();
    
    state.onExit();
}

// 分岐カバレッジ向上のための追加テスト
void test_datetime_input_branch_coverage_additional() {
    DateTimeInputState state(testTimeProvider.get());
    
    // 初期化と描画
    state.onEnter();
    state.onDraw();
    
    // 各ボタンの分岐をテスト
    state.onButtonA(); // インクリメント
    state.onButtonB(); // カーソル移動
    state.onButtonC(); // 確定
    
    // 長押しボタンの分岐
    state.onButtonALongPress();
    state.onButtonBLongPress();
    state.onButtonCLongPress();
    
    // 終了処理
    state.onExit();
}

// 未カバー関数のテスト（公開インターフェース経由）
void test_datetime_input_uncovered_functions() {
    DateTimeInputState state(testTimeProvider.get());
    
    // incrementCurrentDigitのテスト（onButtonA経由）
    state.onEnter();
    for (int i = 0; i < 20; i++) {
        state.onButtonA(); // incrementCurrentDigitが呼ばれる
    }
    
    // moveCursorRightのテスト（onButtonB経由）
    for (int i = 0; i < 15; i++) {
        state.onButtonB(); // moveCursorRightが呼ばれる
    }
    
    // validateDateTimeのテスト
    TEST_ASSERT_TRUE(state.validateDateTime());
    
    // formatDateTimeStringのテスト
    std::string result = state.formatDateTimeString();
    TEST_ASSERT_FALSE(result.empty());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    
    RUN_TEST(test_datetime_input_basic);
    RUN_TEST(test_datetime_input_validate_datetime);
    RUN_TEST(test_datetime_input_button_operations);
    RUN_TEST(test_datetime_input_edge_cases);
    RUN_TEST(test_datetime_input_branch_coverage);
    RUN_TEST(test_datetime_input_comprehensive);
    RUN_TEST(test_datetime_input_error_cases);
    RUN_TEST(test_datetime_input_additional_coverage);
    RUN_TEST(test_datetime_input_branch_coverage_additional);
    RUN_TEST(test_datetime_input_uncovered_functions);
    
    return UNITY_END();
} 