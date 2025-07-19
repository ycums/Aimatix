#include <unity.h>
#include <debounce_manager.h>

// 各テストで独立した時間管理
unsigned long testTime = 0;
unsigned long testMillis() { return testTime; }
void setTestTime(unsigned long time) { testTime = time; }
void resetTestTime() { testTime = 0; }

void setUp(void) {
    // 各テスト前に時間をリセット
    resetTestTime();
}

void tearDown(void) {
    // テスト後のクリーンアップ
}

// ハードウェアデバウンスのテスト
void test_hardware_debounce() {
    // 各テストで独立したインスタンスを作成
    DebounceManager debounceManager;
    
    // 初回呼び出しは常にtrue
    TEST_ASSERT_TRUE(debounceManager.canProcessHardware(BUTTON_TYPE_A, testMillis));
    
    // 50ms未満ではfalse
    setTestTime(25);
    TEST_ASSERT_FALSE(debounceManager.canProcessHardware(BUTTON_TYPE_A, testMillis));
    
    // 50ms経過後はtrue
    setTestTime(75);
    TEST_ASSERT_TRUE(debounceManager.canProcessHardware(BUTTON_TYPE_A, testMillis));
}

// 操作デバウンスのテスト
void test_operation_debounce() {
    // 各テストで独立したインスタンスを作成
    DebounceManager debounceManager;
    
    // 初回呼び出しは常にtrue
    TEST_ASSERT_TRUE(debounceManager.canProcessOperation("test_operation", testMillis));
    
    // 200ms未満ではfalse
    setTestTime(100);
    TEST_ASSERT_FALSE(debounceManager.canProcessOperation("test_operation", testMillis));
    
    // 200ms経過後はtrue
    setTestTime(250);
    TEST_ASSERT_TRUE(debounceManager.canProcessOperation("test_operation", testMillis));
}

// 複数ボタンの独立したデバウンステスト
void test_multiple_buttons_independent() {
    // 各テストで独立したインスタンスを作成
    DebounceManager debounceManager;
    
    // 各ボタンで初回呼び出し
    TEST_ASSERT_TRUE(debounceManager.canProcessHardware(BUTTON_TYPE_A, testMillis));
    
    // Aボタンは50ms未満でfalse
    setTestTime(25);
    TEST_ASSERT_FALSE(debounceManager.canProcessHardware(BUTTON_TYPE_A, testMillis));
    
    // 他のボタンは独立して動作
    TEST_ASSERT_TRUE(debounceManager.canProcessHardware(BUTTON_TYPE_B, testMillis));
    TEST_ASSERT_TRUE(debounceManager.canProcessHardware(BUTTON_TYPE_C, testMillis));
}

// 複数操作の独立したデバウンステスト
void test_multiple_operations_independent() {
    // 各テストで独立したインスタンスを作成
    DebounceManager debounceManager;
    
    // 各操作で初回呼び出し
    TEST_ASSERT_TRUE(debounceManager.canProcessOperation("operation1", testMillis));
    TEST_ASSERT_TRUE(debounceManager.canProcessOperation("operation2", testMillis));
    
    // 200ms未満では両方ともfalse
    setTestTime(100);
    TEST_ASSERT_FALSE(debounceManager.canProcessOperation("operation1", testMillis));
    TEST_ASSERT_FALSE(debounceManager.canProcessOperation("operation2", testMillis));
    
    // 200ms経過後は両方ともtrue
    setTestTime(250);
    TEST_ASSERT_TRUE(debounceManager.canProcessOperation("operation1", testMillis));
    TEST_ASSERT_TRUE(debounceManager.canProcessOperation("operation2", testMillis));
}

// ハードウェアと操作の独立したデバウンステスト
void test_hardware_and_operation_independent() {
    // 各テストで独立したインスタンスを作成
    DebounceManager debounceManager;
    
    // ハードウェアと操作で初回呼び出し
    TEST_ASSERT_TRUE(debounceManager.canProcessHardware(BUTTON_TYPE_A, testMillis));
    TEST_ASSERT_TRUE(debounceManager.canProcessOperation("test", testMillis));
    
    // 50ms未満ではハードウェアのみfalse
    setTestTime(25);
    TEST_ASSERT_FALSE(debounceManager.canProcessHardware(BUTTON_TYPE_A, testMillis));
    TEST_ASSERT_FALSE(debounceManager.canProcessOperation("test", testMillis));
}

// 全ボタンのデバウンステスト
void test_all_buttons_debounce() {
    // 各テストで独立したインスタンスを作成
    DebounceManager debounceManager;
    
    // 全ボタンで初回呼び出し
    TEST_ASSERT_TRUE(debounceManager.canProcessHardware(BUTTON_TYPE_A, testMillis));
    TEST_ASSERT_TRUE(debounceManager.canProcessHardware(BUTTON_TYPE_B, testMillis));
    TEST_ASSERT_TRUE(debounceManager.canProcessHardware(BUTTON_TYPE_C, testMillis));
    
    // 50ms未満では全ボタンfalse
    setTestTime(25);
    TEST_ASSERT_FALSE(debounceManager.canProcessHardware(BUTTON_TYPE_A, testMillis));
    TEST_ASSERT_FALSE(debounceManager.canProcessHardware(BUTTON_TYPE_B, testMillis));
    TEST_ASSERT_FALSE(debounceManager.canProcessHardware(BUTTON_TYPE_C, testMillis));
}

// ハードウェアデバウンスの連続呼び出しテスト
void test_hardware_debounce_continuous() {
    DebounceManager debounceManager;
    TEST_ASSERT_TRUE(debounceManager.canProcessHardware(BUTTON_TYPE_A, testMillis));
    setTestTime(10);
    TEST_ASSERT_FALSE(debounceManager.canProcessHardware(BUTTON_TYPE_A, testMillis));
    // 50ms後（true）
    setTestTime(50);
    TEST_ASSERT_TRUE(debounceManager.canProcessHardware(BUTTON_TYPE_A, testMillis));
}

// 操作デバウンスの連続呼び出しテスト
void test_operation_debounce_continuous() {
    DebounceManager debounceManager;
    TEST_ASSERT_TRUE(debounceManager.canProcessOperation("test_op", testMillis));
    setTestTime(100);
    TEST_ASSERT_FALSE(debounceManager.canProcessOperation("test_op", testMillis));
    // 200ms後（true）
    setTestTime(200);
    TEST_ASSERT_TRUE(debounceManager.canProcessOperation("test_op", testMillis));
}

// ハードウェアデバウンスの長時間経過テスト
void test_hardware_debounce_long_time() {
    // 各テストで独立したインスタンスを作成
    DebounceManager debounceManager;
    
    // 初回呼び出し
    TEST_ASSERT_TRUE(debounceManager.canProcessHardware(BUTTON_TYPE_A, testMillis));
    
    // 長時間経過後
    setTestTime(1000);
    TEST_ASSERT_TRUE(debounceManager.canProcessHardware(BUTTON_TYPE_A, testMillis));
}

// 操作デバウンスの長時間経過テスト
void test_operation_debounce_long_time() {
    // 各テストで独立したインスタンスを作成
    DebounceManager debounceManager;
    
    // 初回呼び出し
    TEST_ASSERT_TRUE(debounceManager.canProcessOperation("test_op", testMillis));
    
    // 長時間経過後
    setTestTime(1000);
    TEST_ASSERT_TRUE(debounceManager.canProcessOperation("test_op", testMillis));
}

// ハードウェアと操作の組み合わせテスト
void test_hardware_and_operation_combination() {
    DebounceManager debounceManager;
    TEST_ASSERT_TRUE(debounceManager.canProcessHardware(BUTTON_TYPE_A, testMillis));
    TEST_ASSERT_TRUE(debounceManager.canProcessOperation("test_op", testMillis));
    setTestTime(25);
    TEST_ASSERT_FALSE(debounceManager.canProcessHardware(BUTTON_TYPE_A, testMillis));
    TEST_ASSERT_FALSE(debounceManager.canProcessOperation("test_op", testMillis));
    // 50ms後（true）
    setTestTime(50);
    TEST_ASSERT_TRUE(debounceManager.canProcessHardware(BUTTON_TYPE_A, testMillis));
    // 200ms後（true）
    setTestTime(200);
    TEST_ASSERT_TRUE(debounceManager.canProcessOperation("test_op", testMillis));
}

// ストレステスト
void test_stress_test() {
    DebounceManager debounceManager;
    setTestTime(0);
    TEST_ASSERT_TRUE(debounceManager.canProcessHardware(BUTTON_TYPE_A, testMillis));
    TEST_ASSERT_TRUE(debounceManager.canProcessOperation("stress_test", testMillis));
    // 多数の操作を連続実行
    for (int i = 1; i < 100; i++) {
        setTestTime(i * 10);
        debounceManager.canProcessHardware(BUTTON_TYPE_A, testMillis);
        debounceManager.canProcessOperation("stress_test", testMillis);
    }
    // 1000ms後（true）
    setTestTime(1000);
    TEST_ASSERT_TRUE(debounceManager.canProcessHardware(BUTTON_TYPE_A, testMillis));
    TEST_ASSERT_TRUE(debounceManager.canProcessOperation("stress_test", testMillis));
}

// 空文字列の操作テスト
void test_empty_operation() {
    DebounceManager debounceManager;
    TEST_ASSERT_TRUE(debounceManager.canProcessOperation("", testMillis));
    setTestTime(100);
    TEST_ASSERT_FALSE(debounceManager.canProcessOperation("", testMillis));
    // 200ms後（true）
    setTestTime(200);
    TEST_ASSERT_TRUE(debounceManager.canProcessOperation("", testMillis));
}

// 特殊文字を含む操作テスト
void test_special_characters_operation() {
    // 各テストで独立したインスタンスを作成
    DebounceManager debounceManager;
    
    // 特殊文字を含む操作名でも動作する
    TEST_ASSERT_TRUE(debounceManager.canProcessOperation("test", testMillis));
    
    // 200ms未満ではfalse
    setTestTime(100);
    TEST_ASSERT_FALSE(debounceManager.canProcessOperation("test", testMillis));
}

// 全機能の組み合わせテスト
void test_all_functions_combination() {
    // 各テストで独立したインスタンスを作成
    DebounceManager debounceManager;
    
    // 全機能で初回呼び出し
    TEST_ASSERT_TRUE(debounceManager.canProcessHardware(BUTTON_TYPE_A, testMillis));
    TEST_ASSERT_TRUE(debounceManager.canProcessOperation("test_op", testMillis));
    
    // 50ms未満ではハードウェアのみfalse
    setTestTime(25);
    TEST_ASSERT_FALSE(debounceManager.canProcessHardware(BUTTON_TYPE_A, testMillis));
    TEST_ASSERT_FALSE(debounceManager.canProcessOperation("test_op", testMillis));
}

// 複数回呼び出しの戻り値テスト
void test_multiple_calls_return_values() {
    // 各テストで独立したインスタンスを作成
    DebounceManager debounceManager;
    
    // 初回呼び出しの戻り値を確認
    bool result1 = debounceManager.canProcessHardware(BUTTON_TYPE_A, testMillis);
    TEST_ASSERT_TRUE(result1);
    
    // 2回目の呼び出し（50ms未満）
    setTestTime(25);
    bool result2 = debounceManager.canProcessHardware(BUTTON_TYPE_A, testMillis);
    TEST_ASSERT_FALSE(result2);
    
    // 操作の初回呼び出し
    bool result3 = debounceManager.canProcessOperation("test_op", testMillis);
    TEST_ASSERT_TRUE(result3);
    
    // 操作の2回目の呼び出し（200ms未満）
    setTestTime(100);
    bool result4 = debounceManager.canProcessOperation("test_op", testMillis);
    TEST_ASSERT_FALSE(result4);
}

// 異なる時間間隔でのテスト
void test_different_time_intervals() {
    // 各テストで独立したインスタンスを作成
    DebounceManager debounceManager;
    
    // 初回呼び出し
    TEST_ASSERT_TRUE(debounceManager.canProcessHardware(BUTTON_TYPE_A, testMillis));
    TEST_ASSERT_TRUE(debounceManager.canProcessOperation("test_op", testMillis));
    
    // 異なる時間間隔でテスト
    setTestTime(10);
    TEST_ASSERT_FALSE(debounceManager.canProcessHardware(BUTTON_TYPE_A, testMillis));
    TEST_ASSERT_FALSE(debounceManager.canProcessOperation("test_op", testMillis));
    
    setTestTime(50);
    TEST_ASSERT_TRUE(debounceManager.canProcessHardware(BUTTON_TYPE_A, testMillis));
    TEST_ASSERT_FALSE(debounceManager.canProcessOperation("test_op", testMillis));
    
    setTestTime(200);
    TEST_ASSERT_TRUE(debounceManager.canProcessHardware(BUTTON_TYPE_A, testMillis));
    TEST_ASSERT_TRUE(debounceManager.canProcessOperation("test_op", testMillis));
}

int main() {
    UNITY_BEGIN();
    
    // ハードウェアデバウンスのテスト
    RUN_TEST(test_hardware_debounce);
    RUN_TEST(test_multiple_buttons_independent);
    RUN_TEST(test_hardware_debounce_continuous);
    RUN_TEST(test_hardware_debounce_long_time);
    RUN_TEST(test_all_buttons_debounce);
    
    // 操作デバウンスのテスト
    RUN_TEST(test_operation_debounce);
    RUN_TEST(test_multiple_operations_independent);
    RUN_TEST(test_operation_debounce_continuous);
    RUN_TEST(test_operation_debounce_long_time);
    
    // 組み合わせテスト
    RUN_TEST(test_hardware_and_operation_independent);
    RUN_TEST(test_hardware_and_operation_combination);
    RUN_TEST(test_all_functions_combination);
    
    // その他のテスト
    RUN_TEST(test_stress_test);
    RUN_TEST(test_empty_operation);
    RUN_TEST(test_special_characters_operation);
    RUN_TEST(test_multiple_calls_return_values);
    RUN_TEST(test_different_time_intervals);
    
    return UNITY_END();
} 