#include <unity.h>
#include <cstdio>
#include <ctime>
#include <debounce_manager.h>

// lib/your_lib/src/debounce_manager.hのDebounceManagerクラスを直接テスト

// テスト用のmockMillis関数
unsigned long mockMillis = 0;
unsigned long millis() { return mockMillis; }
void setMockTime(unsigned long time) { mockMillis = time; }

void setUp(void) {
    setMockTime(0);
    DebounceManager::reset();
}
void tearDown(void) {}

// 基本的なデバウンス機能テスト
void test_canProcessHardware() {
    // 初回は処理可能
    TEST_ASSERT_TRUE(DebounceManager::canProcessHardware(1, millis));
    
    // 50ms以内は処理不可
    setMockTime(25);
    TEST_ASSERT_FALSE(DebounceManager::canProcessHardware(1, millis));
    
    // 50ms経過後は処理可能
    setMockTime(50);
    TEST_ASSERT_TRUE(DebounceManager::canProcessHardware(1, millis));
    
    printf("✓ canProcessHardware: 成功\n");
}

void test_canProcessOperation() {
    // 初回の操作は処理可能
    TEST_ASSERT_TRUE(DebounceManager::canProcessOperation("test_operation", millis));
    
    // 200ms以内は処理不可
    setMockTime(100);
    TEST_ASSERT_FALSE(DebounceManager::canProcessOperation("test_operation", millis));
    
    // 200ms経過後は処理可能
    setMockTime(200);
    TEST_ASSERT_TRUE(DebounceManager::canProcessOperation("test_operation", millis));
    
    printf("✓ canProcessOperation: 成功\n");
}

// 連打防止テスト（現実的なシナリオ）
void test_rapid_button_press_prevention() {
    // 初回呼び出し
    TEST_ASSERT_TRUE(DebounceManager::canProcessHardware(1, millis));
    
    // 連打シナリオ（25ms後に再度押下）
    setMockTime(25);
    TEST_ASSERT_FALSE(DebounceManager::canProcessHardware(1, millis));
    
    // 適切な間隔での操作
    setMockTime(60);
    TEST_ASSERT_TRUE(DebounceManager::canProcessHardware(1, millis));
    
    printf("✓ 連打防止テスト: 成功\n");
}

// 異なる操作の独立管理テスト
void test_multiple_operations() {
    // 異なる操作タイプは独立して管理される
    TEST_ASSERT_TRUE(DebounceManager::canProcessOperation("operation1", millis));
    TEST_ASSERT_TRUE(DebounceManager::canProcessOperation("operation2", millis));
    
    setMockTime(100);
    
    // 両方とも200ms以内なので処理不可
    TEST_ASSERT_FALSE(DebounceManager::canProcessOperation("operation1", millis));
    TEST_ASSERT_FALSE(DebounceManager::canProcessOperation("operation2", millis));
    
    setMockTime(250);
    
    // 両方とも200ms経過後なので処理可能
    TEST_ASSERT_TRUE(DebounceManager::canProcessOperation("operation1", millis));
    TEST_ASSERT_TRUE(DebounceManager::canProcessOperation("operation2", millis));
    
    printf("✓ multiple_operations: 成功\n");
}

// メイン関数
int main() {
    UNITY_BEGIN();
  
    printf("=== DebounceManager 純粋ロジックテスト ===\n");
  
    RUN_TEST(test_canProcessHardware);
    RUN_TEST(test_canProcessOperation);
    RUN_TEST(test_rapid_button_press_prevention);
    RUN_TEST(test_multiple_operations);
  
    printf("=== 全テスト完了 ===\n");
  
    return UNITY_END();
} 