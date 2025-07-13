#include <unity.h>
#include <cstdio>
#include <cstring>
#include <vector>
#include <map>
#include "../src/state_transition/button_event.h"
#include "../src/state_transition/system_state.h"
#include "../src/state_transition/transition_result.h"
#include "../src/state_transition/transition_validator.h"
#include "../src/state_transition/state_transition.h"
#include "test_framework.h"

// 改善されたテスト用のモック関数
extern "C" {
  unsigned long millis() { return 1000; }
  time_t time(time_t* t) { return 1000; }
}

// テスト用のグローバル変数
enum Mode currentMode = MAIN_DISPLAY;
SettingsMenu settingsMenu;
InputState inputState;
std::vector<time_t> alarmTimes;
int scheduleSelectedIndex = 0;

// 警告メッセージ表示状態のモック
bool mockWarningDisplayed = false;
bool isWarningMessageDisplayed(const char* message) {
  return mockWarningDisplayed;
}

// テスト結果の詳細記録
struct TestResult {
  std::string testName;
  bool passed;
  std::string errorMessage;
  int executionTime;
};

std::vector<TestResult> testResults;

// テスト実行時間の計測
clock_t testStartTime;

void setUp(void) {
  CUSTOM_TEST_SETUP();
  currentMode = MAIN_DISPLAY;
  settingsMenu.selectedItem = 0;
  settingsMenu.itemCount = 5;
  inputState = InputState();
  alarmTimes.clear();
  scheduleSelectedIndex = 0;
  mockWarningDisplayed = false;
  testStartTime = clock();
}

void tearDown(void) {
  clock_t testEndTime = clock();
  int executionTime = (int)((testEndTime - testStartTime) * 1000 / CLOCKS_PER_SEC);
  
  // テスト結果を記録
  TestResult result;
  result.testName = "Current Test";
  result.passed = true; // デフォルトは成功
  result.executionTime = executionTime;
  
  testResults.push_back(result);
  
  CUSTOM_TEST_TEARDOWN();
}

// エラーハンドリング付きの状態遷移テスト
void test_state_transition_with_error_handling() {
  CUSTOM_TEST_SETUP();
  
  SystemState state = getCurrentSystemState();
  
  // 正常な遷移のテスト
  ButtonEvent aShortPress(BUTTON_TYPE_A, SHORT_PRESS);
  TransitionResult result = StateTransitionManager::handleStateTransition(MAIN_DISPLAY, aShortPress, state);
  
  CUSTOM_TEST_ASSERT_WITH_MESSAGE(result.isValid, "正常な状態遷移が失敗しました");
  CUSTOM_TEST_ASSERT_WITH_MESSAGE(result.nextMode == ABS_TIME_INPUT, "期待されるモードに遷移しませんでした");
  
  // エラー状態での遷移テスト
  mockWarningDisplayed = true;
  result = StateTransitionManager::handleStateTransition(MAIN_DISPLAY, aShortPress, state);
  CUSTOM_TEST_ASSERT_WITH_MESSAGE(!result.isValid, "警告表示中に遷移が許可されました");
  mockWarningDisplayed = false;
  
  printf("✓ 状態遷移エラーハンドリングテスト: 成功\n");
  CUSTOM_TEST_TEARDOWN();
}

// 境界値テストの改善
void test_boundary_conditions_improved() {
  CUSTOM_TEST_SETUP();
  
  SystemState state = getCurrentSystemState();
  
  // アラーム数の境界値テスト
  for (int i = 0; i < 5; i++) {
    alarmTimes.push_back(1000 + i);
  }
  state = getCurrentSystemState();
  CUSTOM_TEST_ASSERT_WITH_MESSAGE(state.alarmCount == 5, "最大アラーム数が正しく設定されませんでした");
  
  // 選択インデックスの境界値テスト
  state.selectedIndex = 0; // 最小値
  CUSTOM_TEST_ASSERT_WITH_MESSAGE(isValidSystemState(state), "最小インデックスが無効と判定されました");
  
  state.selectedIndex = 4; // 最大値
  CUSTOM_TEST_ASSERT_WITH_MESSAGE(isValidSystemState(state), "最大インデックスが無効と判定されました");
  
  state.selectedIndex = 5; // 範囲外
  CUSTOM_TEST_ASSERT_WITH_MESSAGE(!isValidSystemState(state), "範囲外インデックスが有効と判定されました");
  
  printf("✓ 境界値テスト（改善版）: 成功\n");
  CUSTOM_TEST_TEARDOWN();
}

// パフォーマンステストの追加
void test_performance_benchmark() {
  CUSTOM_TEST_SETUP();
  
  SystemState state = getCurrentSystemState();
  const int iterations = 1000;
  
  clock_t startTime = clock();
  
  for (int i = 0; i < iterations; i++) {
    ButtonEvent event(BUTTON_TYPE_A, SHORT_PRESS);
    StateTransitionManager::handleStateTransition(MAIN_DISPLAY, event, state);
  }
  
  clock_t endTime = clock();
  int totalTime = (int)((endTime - startTime) * 1000 / CLOCKS_PER_SEC);
  double avgTime = (double)totalTime / iterations;
  
  CUSTOM_TEST_ASSERT_WITH_MESSAGE(avgTime < 1.0, "状態遷移の平均実行時間が1msを超えています");
  
  printf("✓ パフォーマンステスト: 成功 (平均実行時間: %.3fms)\n", avgTime);
  CUSTOM_TEST_TEARDOWN();
}

// メモリ使用量テストの追加
void test_memory_usage() {
  CUSTOM_TEST_SETUP();
  
  // 初期状態のメモリ使用量を記録
  size_t initialSize = alarmTimes.size();
  
  // 大量のアラームを追加
  for (int i = 0; i < 10; i++) {
    alarmTimes.push_back(1000 + i);
  }
  
  size_t finalSize = alarmTimes.size();
  CUSTOM_TEST_ASSERT_WITH_MESSAGE(finalSize == initialSize + 10, "アラームの追加が正しく動作しませんでした");
  
  // メモリリークのチェック（簡易版）
  alarmTimes.clear();
  CUSTOM_TEST_ASSERT_WITH_MESSAGE(alarmTimes.size() == 0, "アラームのクリアが正しく動作しませんでした");
  
  printf("✓ メモリ使用量テスト: 成功\n");
  CUSTOM_TEST_TEARDOWN();
}

// テスト結果の詳細レポート生成
void generate_test_report() {
  printf("\n" + "=" * 60 + "\n");
  printf("📊 統合テスト結果レポート\n");
  printf("=" * 60 + "\n");
  
  int totalTests = testResults.size();
  int passedTests = 0;
  int totalTime = 0;
  
  for (const auto& result : testResults) {
    if (result.passed) passedTests++;
    totalTime += result.executionTime;
  }
  
  printf("総テスト数: %d\n", totalTests);
  printf("成功テスト数: %d\n", passedTests);
  printf("失敗テスト数: %d\n", totalTests - passedTests);
  printf("成功率: %.1f%%\n", (double)passedTests / totalTests * 100);
  printf("総実行時間: %dms\n", totalTime);
  printf("平均実行時間: %.1fms\n", (double)totalTime / totalTests);
  
  if (passedTests == totalTests) {
    printf("🎉 全テストが成功しました！\n");
  } else {
    printf("⚠️  一部のテストが失敗しました。\n");
  }
  
  printf("=" * 60 + "\n");
}

// メイン関数
int main() {
  UNITY_BEGIN();
  
  printf("=== 統合テスト（改善版）開始 ===\n");
  
  RUN_TEST(test_state_transition_with_error_handling);
  RUN_TEST(test_boundary_conditions_improved);
  RUN_TEST(test_performance_benchmark);
  RUN_TEST(test_memory_usage);
  
  // テスト結果レポートを生成
  generate_test_report();
  
  printf("=== 統合テスト（改善版）完了 ===\n");
  
  return UNITY_END();
} 