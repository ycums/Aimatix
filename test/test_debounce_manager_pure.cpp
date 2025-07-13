#include <unity.h>
#include <cstdio>
#include <cstring>
#include <map>
#include <string>

// DebounceManagerの純粋ロジックテスト
// M5Stack依存を排除し、標準C++のみでテスト

// モックButtonクラス
class MockButton {
public:
  bool pressed;
  unsigned long lastChangeTime;
  
  MockButton() : pressed(false), lastChangeTime(0) {}
  
  bool isPressed() const { return pressed; }
  void setPressed(bool state) { pressed = state; }
  void setLastChangeTime(unsigned long time) { lastChangeTime = time; }
};

// モック時間管理
static unsigned long mockMillis = 0;
unsigned long millis() { return mockMillis; }
void setMockTime(unsigned long time) { mockMillis = time; }

// 純粋ロジックDebounceManager
class PureDebounceManager {
private:
  static std::map<std::string, unsigned long> lastOperationTimes;
  static unsigned long lastModeChangeTime;
  static unsigned long lastButtonChangeTime;
  
  // デフォルトのデバウンス時間
  static const unsigned long DEFAULT_HARDWARE_DEBOUNCE = 50;
  static const unsigned long DEFAULT_OPERATION_DEBOUNCE = 200;
  static const unsigned long DEFAULT_MODE_CHANGE_DEBOUNCE = 300;
  
public:
  static void initialize() {
    lastOperationTimes.clear();
    lastModeChangeTime = 0;
    lastButtonChangeTime = 0;
  }
  
  // ハードウェアレベルのデバウンス判定
  static bool canProcessHardware(MockButton& button) {
    unsigned long currentTime = mockMillis;
    
    // 前回の処理から一定時間経過しているかチェック
    if (currentTime - lastButtonChangeTime >= DEFAULT_HARDWARE_DEBOUNCE) {
      lastButtonChangeTime = currentTime;
      return true;
    }
    
    return false;
  }
  
  // 操作レベルのデバウンス判定
  static bool canProcessOperation(const std::string& operationType) {
    unsigned long currentTime = mockMillis;
    
    // 指定された操作タイプの最後の処理時刻を取得
    auto it = lastOperationTimes.find(operationType);
    if (it == lastOperationTimes.end()) {
      // 初回の場合は処理可能
      lastOperationTimes[operationType] = currentTime;
      return true;
    }
    
    // 前回の処理から一定時間経過しているかチェック
    if (currentTime - it->second >= DEFAULT_OPERATION_DEBOUNCE) {
      it->second = currentTime;
      return true;
    }
    
    return false;
  }
  
  // 画面遷移レベルのデバウンス判定
  static bool canProcessModeChange() {
    unsigned long currentTime = mockMillis;
    
    // 前回のモード変更から一定時間経過しているかチェック
    if (currentTime - lastModeChangeTime >= DEFAULT_MODE_CHANGE_DEBOUNCE) {
      lastModeChangeTime = currentTime;
      return true;
    }
    
    return false;
  }
  
  static void reset() {
    initialize();
  }
};

// 静的メンバ変数の定義
std::map<std::string, unsigned long> PureDebounceManager::lastOperationTimes;
unsigned long PureDebounceManager::lastModeChangeTime = 0;
unsigned long PureDebounceManager::lastButtonChangeTime = 0;

// ハードウェアデバウンステスト
void test_hardware_debounce() {
  MockButton button;
  PureDebounceManager::initialize();
  
  setMockTime(0);
  
  // 初回は処理可能
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessHardware(button));
  
  // 50ms以内は処理不可
  setMockTime(25);
  TEST_ASSERT_FALSE(PureDebounceManager::canProcessHardware(button));
  
  // 50ms経過後は処理可能
  setMockTime(50);
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessHardware(button));
  
  printf("✓ ハードウェアデバウンステスト: 成功\n");
}

// 操作デバウンステスト
void test_operation_debounce() {
  PureDebounceManager::initialize();
  
  setMockTime(0);
  
  // 初回の操作は処理可能
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessOperation("test_operation"));
  
  // 200ms以内は処理不可
  setMockTime(100);
  TEST_ASSERT_FALSE(PureDebounceManager::canProcessOperation("test_operation"));
  
  // 200ms経過後は処理可能
  setMockTime(200);
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessOperation("test_operation"));
  
  printf("✓ 操作デバウンステスト: 成功\n");
}

// 複数操作タイプのデバウンステスト
void test_multiple_operation_debounce() {
  PureDebounceManager::initialize();
  
  setMockTime(0);
  
  // 異なる操作タイプは独立して管理される
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessOperation("operation1"));
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessOperation("operation2"));
  
  setMockTime(100);
  
  // 両方とも200ms以内なので処理不可
  TEST_ASSERT_FALSE(PureDebounceManager::canProcessOperation("operation1"));
  TEST_ASSERT_FALSE(PureDebounceManager::canProcessOperation("operation2"));
  
  setMockTime(250);
  
  // 両方とも200ms経過後なので処理可能
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessOperation("operation1"));
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessOperation("operation2"));
  
  printf("✓ 複数操作タイプのデバウンステスト: 成功\n");
}

// モード変更デバウンステスト
void test_mode_change_debounce() {
  PureDebounceManager::initialize();
  
  setMockTime(0);
  
  // 初回のモード変更は処理可能
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessModeChange());
  
  // 300ms以内は処理不可
  setMockTime(150);
  TEST_ASSERT_FALSE(PureDebounceManager::canProcessModeChange());
  
  // 300ms経過後は処理可能
  setMockTime(300);
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessModeChange());
  
  printf("✓ モード変更デバウンステスト: 成功\n");
}

// 階層化デバウンステスト
void test_hierarchical_debounce() {
  MockButton button;
  PureDebounceManager::initialize();
  
  setMockTime(0);
  
  // 全てのレベルで処理可能
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessHardware(button));
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessOperation("test"));
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessModeChange());
  
  setMockTime(25);
  
  // ハードウェアレベルは処理不可（50ms未満）
  TEST_ASSERT_FALSE(PureDebounceManager::canProcessHardware(button));
  
  setMockTime(100);
  
  // 操作レベルも処理不可（200ms未満）
  TEST_ASSERT_FALSE(PureDebounceManager::canProcessOperation("test"));
  
  setMockTime(150);
  
  // モード変更レベルも処理不可（300ms未満）
  TEST_ASSERT_FALSE(PureDebounceManager::canProcessModeChange());
  
  printf("✓ 階層化デバウンステスト: 成功\n");
}

// デバウンス時間境界値テスト
void test_debounce_time_boundaries() {
  MockButton button;
  PureDebounceManager::initialize();
  
  setMockTime(0);
  
  // 初回処理
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessHardware(button));
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessOperation("test"));
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessModeChange());
  
  // 境界値テスト（ハードウェア）
  setMockTime(49);
  TEST_ASSERT_FALSE(PureDebounceManager::canProcessHardware(button));
  
  setMockTime(50);
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessHardware(button));
  
  // 境界値テスト（操作）
  setMockTime(199);
  TEST_ASSERT_FALSE(PureDebounceManager::canProcessOperation("test"));
  
  setMockTime(200);
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessOperation("test"));
  
  // 境界値テスト（モード変更）
  setMockTime(299);
  TEST_ASSERT_FALSE(PureDebounceManager::canProcessModeChange());
  
  setMockTime(300);
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessModeChange());
  
  printf("✓ デバウンス時間境界値テスト: 成功\n");
}

// 連続処理テスト
void test_consecutive_processing() {
  MockButton button;
  PureDebounceManager::initialize();
  
  setMockTime(0);
  
  // 連続処理のテスト
  for (int i = 0; i < 5; i++) {
    setMockTime(i * 100);
    
    // ハードウェアレベル（50ms間隔）
    if (i % 2 == 0) {
      TEST_ASSERT_TRUE(PureDebounceManager::canProcessHardware(button));
    } else {
      TEST_ASSERT_FALSE(PureDebounceManager::canProcessHardware(button));
    }
    
    // 操作レベル（200ms間隔）
    if (i % 4 == 0) {
      TEST_ASSERT_TRUE(PureDebounceManager::canProcessOperation("test"));
    } else {
      TEST_ASSERT_FALSE(PureDebounceManager::canProcessOperation("test"));
    }
  }
  
  printf("✓ 連続処理テスト: 成功\n");
}

// リセット機能テスト
void test_reset_functionality() {
  MockButton button;
  PureDebounceManager::initialize();
  
  setMockTime(0);
  
  // 初回処理
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessHardware(button));
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessOperation("test"));
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessModeChange());
  
  setMockTime(25);
  
  // 処理不可状態
  TEST_ASSERT_FALSE(PureDebounceManager::canProcessHardware(button));
  TEST_ASSERT_FALSE(PureDebounceManager::canProcessOperation("test"));
  TEST_ASSERT_FALSE(PureDebounceManager::canProcessModeChange());
  
  // リセット
  PureDebounceManager::reset();
  
  // リセット後は再び処理可能
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessHardware(button));
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessOperation("test"));
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessModeChange());
  
  printf("✓ リセット機能テスト: 成功\n");
}

// 長時間動作テスト
void test_long_duration_operation() {
  MockButton button;
  PureDebounceManager::initialize();
  
  setMockTime(0);
  
  // 初回処理
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessHardware(button));
  
  // 長時間経過後
  setMockTime(10000);
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessHardware(button));
  
  // 連続処理
  setMockTime(10050);
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessHardware(button));
  
  setMockTime(10099);
  TEST_ASSERT_FALSE(PureDebounceManager::canProcessHardware(button));
  
  printf("✓ 長時間動作テスト: 成功\n");
}

// 複合デバウンスシナリオテスト
void test_composite_debounce_scenario() {
  MockButton button;
  PureDebounceManager::initialize();
  
  setMockTime(0);
  
  // 複数の操作を同時に実行
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessHardware(button));
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessOperation("input_mode"));
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessOperation("alarm_setting"));
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessModeChange());
  
  // 時間経過
  setMockTime(100);
  
  // 各レベルでの処理可否確認
  TEST_ASSERT_FALSE(PureDebounceManager::canProcessHardware(button)); // 50ms未満
  TEST_ASSERT_FALSE(PureDebounceManager::canProcessOperation("input_mode")); // 200ms未満
  TEST_ASSERT_FALSE(PureDebounceManager::canProcessOperation("alarm_setting")); // 200ms未満
  TEST_ASSERT_FALSE(PureDebounceManager::canProcessModeChange()); // 300ms未満
  
  // 段階的に処理可能になる
  setMockTime(100);
  TEST_ASSERT_FALSE(PureDebounceManager::canProcessHardware(button));
  
  setMockTime(250);
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessHardware(button));
  TEST_ASSERT_FALSE(PureDebounceManager::canProcessOperation("input_mode"));
  
  setMockTime(350);
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessHardware(button));
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessOperation("input_mode"));
  TEST_ASSERT_FALSE(PureDebounceManager::canProcessModeChange());
  
  setMockTime(450);
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessHardware(button));
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessOperation("input_mode"));
  TEST_ASSERT_TRUE(PureDebounceManager::canProcessModeChange());
  
  printf("✓ 複合デバウンスシナリオテスト: 成功\n");
}

void setUp(void) {
  PureDebounceManager::initialize();
  setMockTime(0);
}

void tearDown(void) {
  PureDebounceManager::reset();
}

// メイン関数
int main() {
  UNITY_BEGIN();
  
  printf("=== DebounceManager 純粋ロジックテスト ===\n");
  
  RUN_TEST(test_hardware_debounce);
  RUN_TEST(test_operation_debounce);
  RUN_TEST(test_multiple_operation_debounce);
  RUN_TEST(test_mode_change_debounce);
  RUN_TEST(test_hierarchical_debounce);
  RUN_TEST(test_debounce_time_boundaries);
  RUN_TEST(test_consecutive_processing);
  RUN_TEST(test_reset_functionality);
  RUN_TEST(test_long_duration_operation);
  RUN_TEST(test_composite_debounce_scenario);
  
  printf("=== 全テスト完了 ===\n");
  
  return UNITY_END();
} 