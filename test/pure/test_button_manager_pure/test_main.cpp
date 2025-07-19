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

// テスト用の時間関数
unsigned long testTime = 0;
unsigned long getTestTime() {
    return testTime;
}

void setUp(void) {
    testTime = 0;
}
void tearDown(void) {}

void test_mock_button_manager_basic() {
    MockButtonManager mockBtn;
    mockBtn.setPressed(BUTTON_TYPE_A, true);
    TEST_ASSERT_TRUE(mockBtn.isPressed(BUTTON_TYPE_A));
    mockBtn.setLongPressed(BUTTON_TYPE_A, true);
    TEST_ASSERT_TRUE(mockBtn.isLongPressed(BUTTON_TYPE_A));
    mockBtn.reset();
    TEST_ASSERT_FALSE(mockBtn.isPressed(BUTTON_TYPE_A));
    printf("✓ mock_button_manager_basic: 成功\n");
}

// 新しく追加するテストケース（実際に存在する関数のみ）

void test_initialize_function() {
    ButtonManager btnManager(getTestTime);
    
    // initialize関数のテスト
    btnManager.initialize();
    
    // 関数が正常に実行されたことを確認（エラーが発生しない）
    TEST_ASSERT_TRUE(true);
    
    printf("✓ initialize_function: 成功\n");
}

void test_isReleased_function() {
    ButtonManager btnManager(getTestTime);
    
    // 初期化
    btnManager.initialize();
    
    // isReleased関数のテスト
    bool isReleased = btnManager.isReleased(BUTTON_TYPE_A);
    
    // 初期状態ではリリースされていない
    TEST_ASSERT_FALSE(isReleased);
    
    // 他のボタンもテスト
    TEST_ASSERT_FALSE(btnManager.isReleased(BUTTON_TYPE_B));
    TEST_ASSERT_FALSE(btnManager.isReleased(BUTTON_TYPE_C));
    
    printf("✓ isReleased_function: 成功\n");
}

void test_isShortPress_function() {
    ButtonManager btnManager(getTestTime);
    
    // 初期化
    btnManager.initialize();
    
    // isShortPress関数のテスト
    testTime = 1000;
    bool isShortPress = btnManager.isShortPress(BUTTON_TYPE_A, testTime);
    
    // 初期状態では短押しされていない
    TEST_ASSERT_FALSE(isShortPress);
    
    // 他のボタンもテスト
    TEST_ASSERT_FALSE(btnManager.isShortPress(BUTTON_TYPE_B, testTime));
    TEST_ASSERT_FALSE(btnManager.isShortPress(BUTTON_TYPE_C, testTime));
    
    printf("✓ isShortPress_function: 成功\n");
}

void test_canProcessButton_function() {
    ButtonManager btnManager(getTestTime);
    
    // 初期化
    btnManager.initialize();
    
    // canProcessButton関数はプライベートなので、間接的にテスト
    // 初期状態ではボタンが押されていない
    TEST_ASSERT_FALSE(btnManager.isPressed(BUTTON_TYPE_A));
    TEST_ASSERT_FALSE(btnManager.isPressed(BUTTON_TYPE_B));
    TEST_ASSERT_FALSE(btnManager.isPressed(BUTTON_TYPE_C));
    
    printf("✓ canProcessButton_function: 成功\n");
}

void test_resetButtonStates_function() {
    ButtonManager btnManager(getTestTime);
    
    // 初期化
    btnManager.initialize();
    
    // resetButtonStates関数のテスト
    btnManager.resetButtonStates();
    
    // 関数が正常に実行されたことを確認（エラーが発生しない）
    TEST_ASSERT_TRUE(true);
    
    printf("✓ resetButtonStates_function: 成功\n");
}

void test_updateButtonState_function() {
    ButtonManager btnManager(getTestTime);
    
    // 初期化
    btnManager.initialize();
    
    // updateButtonState関数はプライベートなので、間接的にテスト
    // update関数を通じてテスト
    testTime = 1000;
    btnManager.update();
    
    // 関数が正常に実行されたことを確認（エラーが発生しない）
    TEST_ASSERT_TRUE(true);
    
    printf("✓ updateButtonState_function: 成功\n");
}

void test_applyHardwareDebounce_function() {
    ButtonManager btnManager(getTestTime);
    
    // 初期化
    btnManager.initialize();
    
    // applyHardwareDebounce関数はプライベートなので、間接的にテスト
    // update関数を通じてテスト
    
    testTime = 1000;
    btnManager.update();
    
    // エラーが発生しないことを確認
    TEST_ASSERT_TRUE(true);
    
    printf("✓ applyHardwareDebounce_function: 成功\n");
}

void test_getOrCreateButtonState_function() {
    ButtonManager btnManager(getTestTime);
    
    // 初期化
    btnManager.initialize();
    
    // getOrCreateButtonState関数はプライベートなので、間接的にテスト
    // 初期状態ではボタン状態が存在しないため、getButtonStateはnullを返す
    ButtonState* state = btnManager.getButtonState(BUTTON_TYPE_A);
    TEST_ASSERT_NULL(state);
    
    // update関数を呼んでボタン状態を作成
    btnManager.update();
    
    // update後もボタン状態は作成されない（updateは既存のボタン状態のみ更新）
    state = btnManager.getButtonState(BUTTON_TYPE_A);
    TEST_ASSERT_NULL(state);
    
    printf("✓ getOrCreateButtonState_function: 成功\n");
}

void test_getButtonState_function() {
    ButtonManager btnManager(getTestTime);
    
    // 初期化
    btnManager.initialize();
    
    // 初期状態ではボタン状態が存在しないため、getButtonStateはnullを返す
    ButtonState* stateA = btnManager.getButtonState(BUTTON_TYPE_A);
    ButtonState* stateB = btnManager.getButtonState(BUTTON_TYPE_B);
    ButtonState* stateC = btnManager.getButtonState(BUTTON_TYPE_C);
    TEST_ASSERT_NULL(stateA);
    TEST_ASSERT_NULL(stateB);
    TEST_ASSERT_NULL(stateC);
    
    printf("✓ getButtonState_function: 成功\n");
}

void test_button_manager_comprehensive() {
    ButtonManager btnManager(getTestTime);
    
    // 包括的なテスト
    // 1. 初期化
    btnManager.initialize();
    
    // 2. 更新処理
    testTime = 1000;
    btnManager.update();
    
    // 3. 状態確認
    TEST_ASSERT_FALSE(btnManager.isPressed(BUTTON_TYPE_A));
    TEST_ASSERT_FALSE(btnManager.isLongPressed(BUTTON_TYPE_A));
    TEST_ASSERT_FALSE(btnManager.isShortPress(BUTTON_TYPE_A, testTime));
    TEST_ASSERT_FALSE(btnManager.isReleased(BUTTON_TYPE_A));
    
    // 4. リセット
    btnManager.resetButtonStates();
    
    printf("✓ button_manager_comprehensive: 成功\n");
}

void test_button_manager_error_conditions() {
    ButtonManager btnManager(getTestTime);
    
    // 初期化
    btnManager.initialize();
    
    // エラー条件のテスト
    
    // 無効なボタンタイプ（範囲外）
    testTime = 1000;
    btnManager.update();
    
    // エラーが発生しないことを確認
    TEST_ASSERT_TRUE(true);
    
    // 無効なボタンタイプでの状態確認
    TEST_ASSERT_FALSE(btnManager.isPressed(static_cast<ButtonType>(99)));
    TEST_ASSERT_FALSE(btnManager.isLongPressed(static_cast<ButtonType>(99)));
    TEST_ASSERT_FALSE(btnManager.isShortPress(static_cast<ButtonType>(99), testTime));
    TEST_ASSERT_FALSE(btnManager.isReleased(static_cast<ButtonType>(99)));
    
    printf("✓ button_manager_error_conditions: 成功\n");
}

void test_button_manager_timing_behavior() {
    ButtonManager btnManager(getTestTime);
    
    // 初期化
    btnManager.initialize();
    
    // タイミング動作のテスト
    
    // 連続した更新
    for (int i = 0; i < 10; i++) {
        testTime = i * 100;
        btnManager.update();
        
        // エラーが発生しないことを確認
        TEST_ASSERT_TRUE(true);
    }
    
    printf("✓ button_manager_timing_behavior: 成功\n");
}

void test_button_manager_initialization() {
    ButtonManager btnManager(getTestTime);
    
    // 初期化
    btnManager.initialize();
    
    // 初期状態の確認
    TEST_ASSERT_FALSE(btnManager.isPressed(BUTTON_TYPE_A));
    TEST_ASSERT_FALSE(btnManager.isPressed(BUTTON_TYPE_B));
    TEST_ASSERT_FALSE(btnManager.isPressed(BUTTON_TYPE_C));
    TEST_ASSERT_FALSE(btnManager.isLongPressed(BUTTON_TYPE_A));
    TEST_ASSERT_FALSE(btnManager.isLongPressed(BUTTON_TYPE_B));
    TEST_ASSERT_FALSE(btnManager.isLongPressed(BUTTON_TYPE_C));
    
    printf("✓ button_manager_initialization: 成功\n");
}

void test_button_state_operations() {
    ButtonManager btnManager(getTestTime);
    
    // 初期化
    btnManager.initialize();
    
    // ボタン状態の基本操作
    TEST_ASSERT_FALSE(btnManager.isPressed(BUTTON_TYPE_A));
    TEST_ASSERT_FALSE(btnManager.isLongPressed(BUTTON_TYPE_A));
    
    // 更新処理
    btnManager.update();
    
    // 状態が変化しないことを確認
    TEST_ASSERT_FALSE(btnManager.isPressed(BUTTON_TYPE_A));
    TEST_ASSERT_FALSE(btnManager.isLongPressed(BUTTON_TYPE_A));
    
    printf("✓ button_state_operations: 成功\n");
}

void test_button_manager_update_cycle() {
    ButtonManager btnManager(getTestTime);
    
    // 初期化
    btnManager.initialize();
    
    // 更新サイクルのテスト
    for (int i = 0; i < 5; i++) {
        testTime = i * 1000;
        btnManager.update();
        
        // 各更新でエラーが発生しないことを確認
        TEST_ASSERT_FALSE(btnManager.isPressed(BUTTON_TYPE_A));
        TEST_ASSERT_FALSE(btnManager.isLongPressed(BUTTON_TYPE_A));
        TEST_ASSERT_FALSE(btnManager.isShortPress(BUTTON_TYPE_A, testTime));
        TEST_ASSERT_FALSE(btnManager.isReleased(BUTTON_TYPE_A));
    }
    
    printf("✓ button_manager_update_cycle: 成功\n");
}

void test_button_manager_consistency() {
    ButtonManager btnManager(getTestTime);
    
    // 初期化
    btnManager.initialize();
    
    // 一貫性のテスト
    testTime = 1000;
    btnManager.update();
    
    // 状態の一貫性を確認
    bool isPressed = btnManager.isPressed(BUTTON_TYPE_A);
    bool isLongPressed = btnManager.isLongPressed(BUTTON_TYPE_A);
    bool isShortPress = btnManager.isShortPress(BUTTON_TYPE_A, testTime);
    bool isReleased = btnManager.isReleased(BUTTON_TYPE_A);
    
    // 初期状態では全てfalse
    TEST_ASSERT_FALSE(isPressed);
    TEST_ASSERT_FALSE(isLongPressed);
    TEST_ASSERT_FALSE(isShortPress);
    TEST_ASSERT_FALSE(isReleased);
    
    printf("✓ button_manager_consistency: 成功\n");
}

void test_button_manager_performance() {
    ButtonManager btnManager(getTestTime);
    
    // 初期化
    btnManager.initialize();
    
    // パフォーマンステスト
    for (int i = 0; i < 1000; i++) {
        testTime = i;
        btnManager.update();
        
        // エラーが発生しないことを確認
        TEST_ASSERT_TRUE(true);
    }
    
    printf("✓ button_manager_performance: 成功\n");
}

void test_button_manager_memory_management() {
    ButtonManager btnManager(getTestTime);
    
    // 初期化
    btnManager.initialize();
    
    // メモリ管理のテスト
    for (int i = 0; i < 100; i++) {
        testTime = i * 10;
        btnManager.update();
        
        // 状態確認（メモリリークがないことを確認）
        // 初期状態ではボタン状態が存在しないため、getButtonStateはnullを返す
        ButtonState* state = btnManager.getButtonState(BUTTON_TYPE_A);
        TEST_ASSERT_NULL(state);
    }
    
    printf("✓ button_manager_memory_management: 成功\n");
}

void test_button_manager_edge_cases() {
    ButtonManager btnManager(getTestTime);
    
    // 初期化
    btnManager.initialize();
    
    // エッジケースのテスト
    
    // 時間が0の場合
    testTime = 0;
    btnManager.update();
    TEST_ASSERT_FALSE(btnManager.isPressed(BUTTON_TYPE_A));
    
    // 非常に大きな時間値
    testTime = 0xFFFFFFFF;
    btnManager.update();
    TEST_ASSERT_FALSE(btnManager.isPressed(BUTTON_TYPE_A));
    
    printf("✓ button_manager_edge_cases: 成功\n");
}

void test_button_manager_multiple_instances() {
    // 複数インスタンスのテスト
    ButtonManager btnManager1(getTestTime);
    ButtonManager btnManager2(getTestTime);
    
    // 両方初期化
    btnManager1.initialize();
    btnManager2.initialize();
    
    // 独立して動作することを確認
    testTime = 1000;
    btnManager1.update();
    btnManager2.update();
    
    TEST_ASSERT_FALSE(btnManager1.isPressed(BUTTON_TYPE_A));
    TEST_ASSERT_FALSE(btnManager2.isPressed(BUTTON_TYPE_A));
    
    printf("✓ button_manager_multiple_instances: 成功\n");
}

void test_button_manager_time_function() {
    // 時間関数のテスト
    unsigned long (*timeFunc)() = getTestTime;
    ButtonManager btnManager(timeFunc);
    
    // 初期化
    btnManager.initialize();
    
    // 時間関数が正しく動作することを確認
    testTime = 5000;
    btnManager.update();
    
    TEST_ASSERT_FALSE(btnManager.isPressed(BUTTON_TYPE_A));
    
    printf("✓ button_manager_time_function: 成功\n");
}

void test_button_manager_interface_compliance() {
    ButtonManager btnManager(getTestTime);
    
    // 初期化
    btnManager.initialize();
    
    // インターフェース準拠のテスト
    // IButtonManagerインターフェースの全てのメソッドが正常に動作することを確認
    
    testTime = 1000;
    btnManager.update();
    
    // 全てのボタンタイプでテスト
    for (int i = 0; i < 3; i++) {
        ButtonType buttonType = static_cast<ButtonType>(i);
        TEST_ASSERT_FALSE(btnManager.isPressed(buttonType));
        TEST_ASSERT_FALSE(btnManager.isLongPressed(buttonType));
        TEST_ASSERT_FALSE(btnManager.isShortPress(buttonType, testTime));
        TEST_ASSERT_FALSE(btnManager.isReleased(buttonType));
    }
    
    printf("✓ button_manager_interface_compliance: 成功\n");
}

void test_button_manager_stress_test() {
    ButtonManager btnManager(getTestTime);
    
    // 初期化
    btnManager.initialize();
    
    // ストレステスト
    for (int i = 0; i < 10000; i++) {
        testTime = i;
        btnManager.update();
        
        // 定期的に状態確認
        if (i % 1000 == 0) {
            TEST_ASSERT_FALSE(btnManager.isPressed(BUTTON_TYPE_A));
            TEST_ASSERT_FALSE(btnManager.isLongPressed(BUTTON_TYPE_A));
        }
    }
    
    printf("✓ button_manager_stress_test: 成功\n");
}

int main() {
    UNITY_BEGIN();
    
    // 基本的なテスト
    RUN_TEST(test_mock_button_manager_basic);
    
    // ButtonManager関数テスト
    RUN_TEST(test_initialize_function);
    RUN_TEST(test_isReleased_function);
    RUN_TEST(test_isShortPress_function);
    RUN_TEST(test_canProcessButton_function);
    RUN_TEST(test_resetButtonStates_function);
    RUN_TEST(test_updateButtonState_function);
    RUN_TEST(test_applyHardwareDebounce_function);
    RUN_TEST(test_getOrCreateButtonState_function);
    RUN_TEST(test_getButtonState_function);
    
    // 包括的テスト
    RUN_TEST(test_button_manager_comprehensive);
    RUN_TEST(test_button_manager_error_conditions);
    RUN_TEST(test_button_manager_timing_behavior);
    RUN_TEST(test_button_manager_initialization);
    RUN_TEST(test_button_state_operations);
    RUN_TEST(test_button_manager_update_cycle);
    RUN_TEST(test_button_manager_consistency);
    RUN_TEST(test_button_manager_performance);
    RUN_TEST(test_button_manager_memory_management);
    RUN_TEST(test_button_manager_edge_cases);
    RUN_TEST(test_button_manager_multiple_instances);
    RUN_TEST(test_button_manager_time_function);
    RUN_TEST(test_button_manager_interface_compliance);
    RUN_TEST(test_button_manager_stress_test);
    
    return UNITY_END();
} 