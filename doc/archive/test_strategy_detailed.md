# テスト戦略詳細仕様書

## 概要

統一されたボタン管理システムの実装における包括的なテスト戦略を定義する。各Phaseに対応したテスト計画、テスト環境、テストケースを詳細に記載し、品質保証を確実にする。

## テスト戦略の基本方針

### 1. 段階的テスト
- 各Phaseの完了時にテストを実施
- 前のPhaseのテスト結果を基に次のPhaseを進める
- 回帰テストを継続的に実施

### 2. 多層テスト
- Unit Test（ロジック層）
- 統合テスト（ハードウェア連携）
- シナリオテスト（実際の使用フロー）

### 3. 自動化優先
- 可能な限り自動テストを実装
- CI/CDパイプラインでの継続的テスト
- 手動テストは実機での最終確認のみ

## テスト環境構成

### 1. Native環境（Unit Test）
```ini
[env:native]
platform = native
build_flags = 
    -Ilib
    -DUNITY_INCLUDE_DOUBLE
    -DUNITY_DOUBLE_PRECISION=1e-12
    -DMOCK_M5STACK
    -DTEST_MODE
    -DARDUINO=100
    -D__XTENSA__=0
    -std=c++11
lib_deps =
    throwtheswitch/Unity @ ^2.5.2
build_unflags = -std=gnu++11
```

### 2. ESP32環境（統合テスト）
```ini
[env:test-m5stack-fire]
platform = espressif32
board = m5stack-fire
framework = arduino
lib_deps =
    throwtheswitch/Unity @ ^2.5.2
    m5stack/M5Stack @ ^0.4.3
    arduino-libraries/NTPClient @ ^3.2.1
build_flags = 
    -DTEST_MODE
    -DCORE_DEBUG_LEVEL=3
    -DM5STACK_FIRE
    -DUNITY_INCLUDE_CONFIG_H
test_framework = unity
test_build_src = yes
build_src_filter = -<main.cpp> +<test/*>
```

### 3. 実機環境（シナリオテスト）
- M5Stack Fire実機
- 実際のボタン操作
- 実際の画面表示確認

## Phase別テスト計画

### Phase 1: ButtonManagerクラスの実装

#### テスト対象
- ButtonManagerクラスの基本機能
- ボタン状態管理
- デバウンス処理との連携

#### テストファイル構成
```
test/
├── test_button_manager_basic.cpp      # 基本機能テスト
├── test_button_manager_state.cpp      # 状態管理テスト
├── test_button_manager_debounce.cpp   # デバウンス連携テスト
└── mocks/
    ├── mock_button.h                  # ボタンモック
    └── mock_m5stack.h                 # M5Stackモック
```

#### テストケース例
```cpp
// test_button_manager_basic.cpp
void test_button_manager_initialization() {
  ButtonManager::initialize();
  TEST_ASSERT_TRUE(ButtonManager::isInitialized());
}

void test_short_press_detection() {
  MockButton button;
  button.simulatePress(100);
  
  bool result = ButtonManager::isShortPress(button, 1000);
  TEST_ASSERT_TRUE(result);
}

void test_long_press_detection() {
  MockButton button;
  button.simulatePress(1500);
  
  bool result = ButtonManager::isLongPress(button, 1000);
  TEST_ASSERT_TRUE(result);
}

void test_button_state_management() {
  MockButton button;
  ButtonState* state = ButtonManager::getButtonState(button);
  
  TEST_ASSERT_NOT_NULL(state);
  TEST_ASSERT_FALSE(state->isPressed);
  
  button.simulatePress(100);
  ButtonManager::updateButtonStates();
  
  TEST_ASSERT_TRUE(state->isPressed);
  TEST_ASSERT_TRUE(state->wasPressed);
}
```

#### 実行方法
```bash
# Native環境でのテスト実行
pio test -e native

# 特定のテストファイルのみ実行
pio test -e native -f test_button_manager_basic

# 詳細出力
pio test -e native --verbose
```

### Phase 2: 並行導入

#### テスト対象
- 既存機能との並行動作
- ButtonManagerの並行動作
- パフォーマンス影響

#### テストファイル構成
```
test/
├── test_integration_phase2.cpp        # 統合テスト
├── test_parallel_operation.cpp        # 並行動作テスト
├── test_performance_phase2.cpp        # パフォーマンステスト
└── test_memory_usage_phase2.cpp       # メモリ使用量テスト
```

#### テストケース例
```cpp
// test_integration_phase2.cpp
void test_existing_functionality_preserved() {
  setupTestEnvironment();
  
  // 既存のボタン処理が正常に動作することを確認
  simulateButtonPress(M5.BtnA, 100);
  TEST_ASSERT_EQUAL(ABS_TIME_INPUT, currentMode);
  
  simulateButtonPress(M5.BtnB, 100);
  TEST_ASSERT_EQUAL(1, digitEditInput.cursor);
}

void test_button_manager_parallel_operation() {
  setupTestEnvironment();
  
  // ButtonManagerが並行して動作することを確認
  ButtonManager::updateButtonStates();
  
  // 既存のボタン処理も正常に動作することを確認
  simulateButtonPress(M5.BtnC, 100);
  TEST_ASSERT_EQUAL(ALARM_MANAGEMENT, currentMode);
}

void test_debounce_manager_integration() {
  setupTestEnvironment();
  
  // DebounceManagerとの連携を確認
  bool canProcess = ButtonManager::canProcessButton(M5.BtnA);
  TEST_ASSERT_TRUE(canProcess);
  
  // 連続押下でのデバウンス効果確認
  simulateButtonPress(M5.BtnA, 50);
  canProcess = ButtonManager::canProcessButton(M5.BtnA);
  TEST_ASSERT_FALSE(canProcess);
}
```

#### パフォーマンステスト
```cpp
// test_performance_phase2.cpp
void test_button_manager_performance() {
  unsigned long startTime = micros();
  
  // ButtonManagerの処理を実行
  ButtonManager::updateButtonStates();
  
  unsigned long endTime = micros();
  unsigned long processingTime = endTime - startTime;
  
  // 処理時間が許容範囲内であることを確認
  TEST_ASSERT_LESS_THAN(1000, processingTime); // 1ms以内
}

void test_memory_usage_impact() {
  int initialHeap = ESP.getFreeHeap();
  
  // ButtonManagerの初期化
  ButtonManager::initialize();
  
  int finalHeap = ESP.getFreeHeap();
  int memoryUsed = initialHeap - finalHeap;
  
  // メモリ使用量が許容範囲内であることを確認
  TEST_ASSERT_LESS_THAN(5000, memoryUsed); // 5KB以内
}
```

### Phase 3: 段階的移行

#### テスト対象
- 移行した処理の動作確認
- 未移行処理との混在動作
- エッジケースの処理

#### テストファイル構成
```
test/
├── test_phase3_migration.cpp          # 移行テスト
├── test_edge_cases_phase3.cpp         # エッジケーステスト
├── test_mixed_processing.cpp          # 混在処理テスト
└── test_warning_messages.cpp          # 警告メッセージテスト
```

#### テストケース例
```cpp
// test_phase3_migration.cpp
void test_c_button_long_press_migration() {
  setupTestEnvironment();
  currentMode = ALARM_MANAGEMENT;
  
  // Cボタン長押しをシミュレート
  simulateButtonPress(M5.BtnC, 1500);
  
  // メイン画面に戻ることを確認
  TEST_ASSERT_EQUAL(MAIN_DISPLAY, currentMode);
}

void test_a_button_short_press_migration() {
  setupTestEnvironment();
  
  // Aボタン短押しをシミュレート
  simulateButtonPress(M5.BtnA, 100);
  
  // 絶対時刻入力画面に遷移することを確認
  TEST_ASSERT_EQUAL(ABS_TIME_INPUT, currentMode);
}

void test_input_mode_c_button_migration() {
  setupTestEnvironment();
  currentMode = ABS_TIME_INPUT;
  
  // 有効な時刻を設定
  digitEditInput.hourTens = 1;
  digitEditInput.hourOnes = 0;
  digitEditInput.minTens = 3;
  digitEditInput.minOnes = 0;
  
  // Cボタン短押しをシミュレート
  simulateButtonPress(M5.BtnC, 100);
  
  // メイン画面に戻ることを確認
  TEST_ASSERT_EQUAL(MAIN_DISPLAY, currentMode);
  
  // アラームが追加されることを確認
  TEST_ASSERT_EQUAL(1, alarmTimes.size());
}
```

#### エッジケーステスト
```cpp
// test_edge_cases_phase3.cpp
void test_rapid_button_presses() {
  setupTestEnvironment();
  
  // 連続でAボタンを押下
  for (int i = 0; i < 10; i++) {
    simulateButtonPress(M5.BtnA, 50);
    delay(10);
  }
  
  // 最後の1回だけが処理されることを確認
  TEST_ASSERT_EQUAL(ABS_TIME_INPUT, currentMode);
}

void test_invalid_time_input() {
  setupTestEnvironment();
  currentMode = ABS_TIME_INPUT;
  
  // 無効な時刻を設定（25:00）
  digitEditInput.hourTens = 2;
  digitEditInput.hourOnes = 5;
  digitEditInput.minTens = 0;
  digitEditInput.minOnes = 0;
  
  simulateButtonPress(M5.BtnC, 100);
  
  // 画面が変わらないことを確認
  TEST_ASSERT_EQUAL(ABS_TIME_INPUT, currentMode);
}

void test_duplicate_alarm_time() {
  setupTestEnvironment();
  
  // 既存のアラームを追加
  time_t existingAlarm = time(NULL) + 3600;
  alarmTimes.push_back(existingAlarm);
  
  currentMode = ABS_TIME_INPUT;
  
  // 同じ時刻を設定
  struct tm tminfo;
  localtime_r(&existingAlarm, &tminfo);
  digitEditInput.hourTens = tminfo.tm_hour / 10;
  digitEditInput.hourOnes = tminfo.tm_hour % 10;
  digitEditInput.minTens = tminfo.tm_min / 10;
  digitEditInput.minOnes = tminfo.tm_min % 10;
  
  simulateButtonPress(M5.BtnC, 100);
  
  // アラーム数が増加しないことを確認
  TEST_ASSERT_EQUAL(1, alarmTimes.size());
}
```

### Phase 4: 完全移行と最適化

#### テスト対象
- 完全移行されたシステム
- 新機能（警告メッセージ、同時押し）
- パフォーマンス最適化

#### テストファイル構成
```
test/
├── test_complete_migration.cpp        # 完全移行テスト
├── test_warning_messages.cpp          # 警告メッセージテスト
├── test_simultaneous_press.cpp        # 同時押しテスト
├── test_performance_optimization.cpp  # 最適化テスト
└── test_system_integration.cpp        # システム統合テスト
```

#### テストケース例
```cpp
// test_complete_migration.cpp
void test_all_button_functions_migrated() {
  setupTestEnvironment();
  
  // 全てのボタン処理がButtonManagerを使用していることを確認
  TEST_ASSERT_TRUE(isButtonManagerUsed());
  
  // 全ての機能が正常に動作することを確認
  testAllButtonOperations();
}

void test_main_display_buttons() {
  setupTestEnvironment();
  
  // Aボタン短押し
  simulateButtonPress(M5.BtnA, 100);
  TEST_ASSERT_EQUAL(ABS_TIME_INPUT, currentMode);
  
  // Bボタン短押し
  resetTestEnvironment();
  simulateButtonPress(M5.BtnB, 100);
  TEST_ASSERT_EQUAL(REL_PLUS_TIME_INPUT, currentMode);
  
  // Bボタン長押し
  resetTestEnvironment();
  simulateButtonPress(M5.BtnB, 1500);
  TEST_ASSERT_EQUAL(REL_MINUS_TIME_INPUT, currentMode);
  
  // Cボタン短押し
  resetTestEnvironment();
  simulateButtonPress(M5.BtnC, 100);
  TEST_ASSERT_EQUAL(ALARM_MANAGEMENT, currentMode);
  
  // Cボタン長押し
  resetTestEnvironment();
  simulateButtonPress(M5.BtnC, 1500);
  TEST_ASSERT_EQUAL(SETTINGS_MENU, currentMode);
}
```

#### 警告メッセージテスト
```cpp
// test_warning_messages.cpp
void test_warning_message_display() {
  setupTestEnvironment();
  
  // 警告メッセージを表示
  showWarningMessage("テスト警告メッセージ");
  
  // メッセージが表示されることを確認
  TEST_ASSERT_TRUE(isWarningMessageDisplayed("テスト警告メッセージ"));
}

void test_maximum_alarm_warning() {
  setupTestEnvironment();
  
  // 最大アラーム数まで追加
  for (int i = 0; i < 5; i++) {
    time_t alarmTime = time(NULL) + (i + 1) * 3600;
    alarmTimes.push_back(alarmTime);
  }
  TEST_ASSERT_EQUAL(5, alarmTimes.size());
  
  // 6件目を追加しようとする
  currentMode = ABS_TIME_INPUT;
  digitEditInput.hourTens = 1;
  digitEditInput.hourOnes = 5;
  digitEditInput.minTens = 0;
  digitEditInput.minOnes = 0;
  
  simulateButtonPress(M5.BtnC, 100);
  
  // 警告メッセージが表示されることを確認
  TEST_ASSERT_TRUE(isWarningMessageDisplayed("アラームは最大5件までです"));
  
  // アラーム数が増加しないことを確認
  TEST_ASSERT_EQUAL(5, alarmTimes.size());
}
```

#### 同時押しテスト
```cpp
// test_simultaneous_press.cpp
void test_ab_simultaneous_press() {
  setupTestEnvironment();
  
  // AB同時押しをシミュレート
  simulateSimultaneousPress(M5.BtnA, M5.BtnB, 100);
  
  // 全アラーム削除確認ダイアログが表示されることを確認
  TEST_ASSERT_TRUE(isYesNoDialogDisplayed("DELETE ALL ALARMS?"));
}

void test_abc_simultaneous_press() {
  setupTestEnvironment();
  
  // ABC同時押しをシミュレート
  simulateAllButtonsPress();
  
  // システムリセット確認ダイアログが表示されることを確認
  TEST_ASSERT_TRUE(isYesNoDialogDisplayed("SYSTEM RESET?"));
}

void test_ac_simultaneous_press() {
  setupTestEnvironment();
  
  // AC同時押しをシミュレート
  simulateSimultaneousPress(M5.BtnA, M5.BtnC, 100);
  
  // デバッグ情報が表示されることを確認
  TEST_ASSERT_TRUE(isDebugInfoDisplayed());
}
```

#### パフォーマンス最適化テスト
```cpp
// test_performance_optimization.cpp
void test_optimized_button_processing() {
  setupTestEnvironment();
  
  unsigned long startTime = micros();
  
  // 最適化されたButtonManagerの処理を実行
  ButtonManager::updateButtonStatesOptimized();
  
  unsigned long endTime = micros();
  unsigned long processingTime = endTime - startTime;
  
  // 処理時間が改善されていることを確認
  TEST_ASSERT_LESS_THAN(500, processingTime); // 0.5ms以内
}

void test_memory_optimization() {
  setupTestEnvironment();
  
  int initialHeap = ESP.getFreeHeap();
  
  // 大量のボタン操作を実行
  for (int i = 0; i < 1000; i++) {
    simulateButtonPress(M5.BtnA, 100);
    ButtonManager::updateButtonStatesOptimized();
  }
  
  int finalHeap = ESP.getFreeHeap();
  int memoryLoss = initialHeap - finalHeap;
  
  // メモリリークがないことを確認
  TEST_ASSERT_LESS_THAN(1000, memoryLoss); // 1KB以内
}

void test_memory_monitor() {
  setupTestEnvironment();
  
  // メモリ使用量を監視
  MemoryMonitor::printMemoryUsage();
  
  // メモリ健全性をチェック
  bool memoryHealth = MemoryMonitor::checkMemoryHealth();
  TEST_ASSERT_TRUE(memoryHealth);
}
```

## シナリオテスト（実機環境）

### テストシナリオ例

#### シナリオ1: 基本的なアラーム作成フロー
```cpp
void test_basic_alarm_creation_flow() {
  // 1. メイン画面から時刻入力画面へ
  TEST_ASSERT_EQUAL(MAIN_DISPLAY, currentMode);
  simulateButtonPress(M5.BtnA, 100);
  TEST_ASSERT_EQUAL(ABS_TIME_INPUT, currentMode);
  
  // 2. 時刻入力
  simulateDigitInput(10, 30);
  TEST_ASSERT_EQUAL(10, digitEditInput.hourTens * 10 + digitEditInput.hourOnes);
  TEST_ASSERT_EQUAL(30, digitEditInput.minTens * 10 + digitEditInput.minOnes);
  
  // 3. 時刻確定
  simulateButtonPress(M5.BtnC, 100);
  TEST_ASSERT_EQUAL(MAIN_DISPLAY, currentMode);
  TEST_ASSERT_EQUAL(1, alarmTimes.size());
}
```

#### シナリオ2: アラーム管理フロー
```cpp
void test_alarm_management_flow() {
  // 1. アラーム管理画面へ
  simulateButtonPress(M5.BtnC, 100);
  TEST_ASSERT_EQUAL(ALARM_MANAGEMENT, currentMode);
  
  // 2. アラーム削除
  simulateButtonPress(M5.BtnC, 100);
  TEST_ASSERT_TRUE(showYesNoDialog("DELETE ALARM?", "10:30"));
  
  // 3. メイン画面に戻る
  simulateButtonPress(M5.BtnC, 1500); // 長押し
  TEST_ASSERT_EQUAL(MAIN_DISPLAY, currentMode);
}
```

#### シナリオ3: エラー処理フロー
```cpp
void test_error_handling_flow() {
  // 1. 最大アラーム数（5件）まで追加
  for (int i = 0; i < 5; i++) {
    addTestAlarm(10 + i, 0);
  }
  TEST_ASSERT_EQUAL(5, alarmTimes.size());
  
  // 2. 6件目を追加しようとする
  simulateAlarmCreation(15, 0);
  
  // 3. 警告メッセージが表示されることを確認
  TEST_ASSERT_TRUE(isWarningMessageDisplayed("アラームは最大5件までです"));
  TEST_ASSERT_EQUAL(5, alarmTimes.size()); // 追加されていないことを確認
}
```

## 継続的テスト（CI/CD）

### GitHub Actions設定例
```yaml
# .github/workflows/test.yml
name: Button Manager Tests
on: [push, pull_request]

jobs:
  unit-tests:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - uses: actions/setup-python@v2
        with:
          python-version: '3.9'
      - run: pip install platformio
      - run: pio test -e native
      - run: pio test -e native --verbose

  integration-tests:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - uses: actions/setup-python@v2
        with:
          python-version: '3.9'
      - run: pip install platformio
      - run: pio test -e test-m5stack-fire
      - run: pio test -e test-m5stack-fire --verbose

  performance-tests:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - uses: actions/setup-python@v2
        with:
          python-version: '3.9'
      - run: pip install platformio
      - run: pio test -e test-m5stack-fire -f test_performance
      - run: pio test -e test-m5stack-fire -f test_memory_usage
```

### テスト結果レポート
```cpp
// テスト結果の集計とレポート生成
void generateTestReport() {
  Serial.println("=== Test Report ===");
  Serial.printf("Total tests: %d\n", UNITY_TEST_COUNT);
  Serial.printf("Passed: %d\n", UNITY_TEST_PASS);
  Serial.printf("Failed: %d\n", UNITY_TEST_FAIL);
  Serial.printf("Success rate: %.1f%%\n", 
                (float)UNITY_TEST_PASS / UNITY_TEST_COUNT * 100);
  
  // パフォーマンス指標
  Serial.printf("Average processing time: %lu microseconds\n", 
                getAverageProcessingTime());
  Serial.printf("Memory usage: %d bytes\n", getMemoryUsage());
  
  Serial.println("==================");
}
```

## テスト実行手順

### 1. 開発時のテスト実行
```bash
# 特定のPhaseのテストを実行
pio test -e native -f test_phase1
pio test -e native -f test_phase2
pio test -e native -f test_phase3
pio test -e native -f test_phase4

# 全テストを実行
pio test -e native
pio test -e test-m5stack-fire
```

### 2. 実機でのシナリオテスト
```bash
# 実機にアップロード
pio run -e test-m5stack-fire -t upload

# シリアルモニタでテスト結果を確認
pio device monitor
```

### 3. 継続的テスト
- GitHub Actionsで自動実行
- プルリクエスト時に自動テスト
- テスト結果をGitHubにレポート

## 成功基準

### Phase 1
- [x] 全Unit Testが通過
- [x] ButtonManagerの基本機能が正常動作
- [x] コンパイルエラー0件

### Phase 2
- [x] 統合テストが通過
- [x] 既存機能が正常動作
- [x] パフォーマンス影響が許容範囲内

### Phase 3
- [x] 移行テストが通過
- [x] エッジケーステストが通過
- [x] 新旧処理の混在が正常動作

### Phase 4
- [x] 完全移行テストが通過
- [x] 新機能テストが通過
- [x] パフォーマンス最適化が確認

### 全体
- [x] 全テストケースの通過率90%以上
- [x] パフォーマンス要件の達成
- [x] メモリ使用量の最適化
- [x] 安定性の向上確認

## トラブルシューティング

### よくある問題と解決方法

#### 1. テストの失敗
**問題**: テストが失敗する
**解決**: テストケースを確認し、実装を修正

#### 2. パフォーマンスの低下
**問題**: テストでパフォーマンスが悪い
**解決**: 実装を最適化し、ボトルネックを特定

#### 3. メモリリーク
**問題**: メモリ使用量が増加し続ける
**解決**: メモリリークを特定し、修正

#### 4. 実機での動作不良
**問題**: テストは通るが実機で動作しない
**解決**: 実機固有の問題を特定し、修正

このテスト戦略により、統一されたボタン管理システムの品質を確実に保証します。 