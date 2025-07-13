# テスト戦略文書（Phase 0.2確定版）

## 1. 概要

Phase 0.2で確定したテスト戦略の詳細を定義する。Unit Test、統合テスト、実機テストの各段階でのテスト方針と実行方法を明確化する。

## 2. テスト戦略の基本方針

### 2.1 テスト駆動開発（TDD）
- **テストファースト**: 実装前にテストコード作成
- **継続的テスト**: 各コミットでの自動テスト実行
- **品質保証**: テストカバレッジによる品質指標

### 2.2 段階的テスト
- **Unit Test**: 機能単位での純粋ロジックテスト
- **統合テスト**: モジュール間の連携テスト
- **実機テスト**: ハードウェア連携の最終確認

### 2.3 効率性重視
- **自動化**: テスト実行の完全自動化
- **高速化**: ローカル環境での高速実行
- **最小化**: 実機テストの最小限化

## 3. Unit Test戦略

### 3.1 テスト環境

#### 実行環境
- **プラットフォーム**: Windows native環境
- **コンパイラ**: g++ (MinGW-w64)
- **フレームワーク**: Unity 2.5.2
- **モック**: カスタムM5Stackモック

#### ビルド設定
```ini
[env:native]
platform = native
build_flags = 
    -Ilib
    -Itest/mocks
    -Isrc
    -DUNITY_INCLUDE_DOUBLE
    -DUNITY_DOUBLE_PRECISION=1e-12
    -DMOCK_M5STACK
    -DTEST_MODE
    -DARDUINO=100
    -D__XTENSA__=0
    -D_WIN32
    -std=c++11
lib_deps =
    throwtheswitch/Unity @ ^2.5.2
build_unflags = -std=gnu++11
test_build_src = no
build_src_filter = +<time_logic.cpp>
```

### 3.2 テスト対象

#### 純粋ロジックテスト
- **TimeLogicクラス**: 時刻計算、バリデーション
- **AlarmLogicクラス**: アラーム管理、重複チェック
- **InputLogicクラス**: 入力処理、境界値チェック
- **SettingsLogicクラス**: 設定管理、永続化

#### テストファイル構成
```
test/
├── test_time_logic_simple.cpp      # 時刻計算テスト
├── test_alarm_logic_simple.cpp     # アラーム管理テスト
├── test_input_logic_simple.cpp     # 入力処理テスト
├── test_settings_logic_simple.cpp  # 設定管理テスト
├── test_warning_messages_simple.cpp # 警告メッセージテスト
└── mocks/
    ├── mock_m5stack.h              # M5Stackモック
    └── mock_m5stack.cpp            # モック実装
```

### 3.3 テスト実行

#### 自動実行スクリプト
```python
# scripts/run_simple_tests.py
def run_all_tests():
    test_files = [
        ("test_time_logic_simple.cpp", "time_logic"),
        ("test_alarm_logic_simple.cpp", "alarm_logic"),
        ("test_input_logic_simple.cpp", "input_logic"),
        ("test_settings_logic_simple.cpp", "settings_logic"),
        ("test_warning_messages_simple.cpp", "warning_messages")
    ]
    
    for test_file, test_name in test_files:
        success = compile_and_run_test(test_file, test_name)
        if not success:
            return False
    return True
```

#### カバレッジ測定
- **測定ツール**: gcov
- **目標カバレッジ**: 80%以上
- **測定対象**: 純粋ロジック関数のみ
- **除外対象**: M5Stack依存関数、UI描画関数

### 3.4 テストケース設計

#### TimeLogicテストケース
```cpp
void test_time_validation() {
    // 正常な時刻
    TEST_ASSERT_TRUE(TimeLogic::isValidTime(0, 0));
    TEST_ASSERT_TRUE(TimeLogic::isValidTime(12, 30));
    TEST_ASSERT_TRUE(TimeLogic::isValidTime(23, 59));
    
    // 無効な時刻
    TEST_ASSERT_FALSE(TimeLogic::isValidTime(-1, 0));
    TEST_ASSERT_FALSE(TimeLogic::isValidTime(24, 0));
    TEST_ASSERT_FALSE(TimeLogic::isValidTime(12, -1));
    TEST_ASSERT_FALSE(TimeLogic::isValidTime(12, 60));
}

void test_absolute_time_calculation() {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    
    // 現在の日付で12:30の時刻を計算
    time_t calculated = TimeLogic::calculateAbsoluteTime(12, 30);
    struct tm* calc_info = localtime(&calculated);
    
    TEST_ASSERT_EQUAL(12, calc_info->tm_hour);
    TEST_ASSERT_EQUAL(30, calc_info->tm_min);
}
```

#### AlarmLogicテストケース
```cpp
void test_alarm_addition() {
    std::vector<time_t> alarms;
    time_t alarm1 = 1000000000;
    time_t alarm2 = 1000003600;
    
    // 正常な追加
    TEST_ASSERT_TRUE(AlarmLogic::addAlarm(alarms, alarm1));
    TEST_ASSERT_EQUAL(1, alarms.size());
    
    TEST_ASSERT_TRUE(AlarmLogic::addAlarm(alarms, alarm2));
    TEST_ASSERT_EQUAL(2, alarms.size());
    
    // 重複追加の拒否
    TEST_ASSERT_FALSE(AlarmLogic::addAlarm(alarms, alarm1));
    TEST_ASSERT_EQUAL(2, alarms.size());
}

void test_maximum_alarm_limit() {
    std::vector<time_t> alarms;
    time_t base_time = 1000000000;
    
    // 5個まで追加可能
    for (int i = 0; i < 5; i++) {
        TEST_ASSERT_TRUE(AlarmLogic::addAlarm(alarms, base_time + i * 3600));
    }
    TEST_ASSERT_EQUAL(5, alarms.size());
    
    // 6個目は拒否
    TEST_ASSERT_FALSE(AlarmLogic::addAlarm(alarms, base_time + 18000));
    TEST_ASSERT_EQUAL(5, alarms.size());
}
```

## 4. 統合テスト戦略

### 4.1 テスト対象

#### モード間遷移テスト
- **全モード間遷移**: 8つのモード間の全遷移パターン
- **状態保持**: 遷移前後の状態の正確性
- **エラー処理**: 無効な遷移の適切な処理

#### エラーケース統合テスト
- **入力エラー**: 無効な入力値の処理
- **システムエラー**: メモリ不足、時刻異常
- **ハードウェアエラー**: ボタン異常、表示異常

#### パフォーマンステスト
- **動作周波数**: 100Hz維持の確認
- **メモリ使用量**: RAM・Flash使用量の監視
- **応答性**: ボタン操作の応答時間

### 4.2 テストファイル構成

```
test/
├── test_integration.cpp           # 統合テスト
├── test_state_transition.cpp      # 状態遷移テスト
├── test_performance.cpp           # パフォーマンステスト
└── test_error_handling.cpp        # エラーハンドリングテスト
```

### 4.3 統合テストケース

#### 状態遷移テスト
```cpp
void test_mode_transitions() {
    // メイン画面から各モードへの遷移
    TEST_ASSERT_EQUAL(ABS_TIME_INPUT, 
        StateTransitionManager::handleStateTransition(
            MAIN_DISPLAY, 
            ButtonEvent(BUTTON_TYPE_A, SHORT_PRESS), 
            getCurrentSystemState()
        ).nextMode);
    
    TEST_ASSERT_EQUAL(REL_PLUS_TIME_INPUT, 
        StateTransitionManager::handleStateTransition(
            MAIN_DISPLAY, 
            ButtonEvent(BUTTON_TYPE_B, SHORT_PRESS), 
            getCurrentSystemState()
        ).nextMode);
    
    TEST_ASSERT_EQUAL(ALARM_MANAGEMENT, 
        StateTransitionManager::handleStateTransition(
            MAIN_DISPLAY, 
            ButtonEvent(BUTTON_TYPE_C, SHORT_PRESS), 
            getCurrentSystemState()
        ).nextMode);
}
```

#### パフォーマンステスト
```cpp
void test_loop_performance() {
    unsigned long start_time = millis();
    int loop_count = 0;
    
    // 1秒間のループ回数を測定
    while (millis() - start_time < 1000) {
        handleButtons();
        loop_count++;
        delay(10); // 10ms間隔
    }
    
    // 100Hz（100回/秒）以上であることを確認
    TEST_ASSERT_GREATER_THAN(95, loop_count);
}
```

## 5. 実機テスト戦略

### 5.1 テスト条件

#### 実行タイミング
- **統合テスト通過後**: 全ての統合テストが成功した後
- **最終確認**: 実装完了後の最終品質確認
- **最小限実行**: 必要最小限のテスト項目のみ

#### テスト環境
- **ハードウェア**: M5Stack Fire v2.7
- **電源**: バッテリー動作、USB給電
- **環境**: 実使用環境に近い条件

### 5.2 テスト項目

#### 基本動作確認
- **画面表示**: 全モードの画面表示確認
- **ボタン操作**: 全ボタンの動作確認
- **音声出力**: アラーム音の確認
- **LED表示**: RGB LEDの動作確認

#### ハードウェア連携確認
- **ボタン応答性**: ボタン押下の即座な反応
- **表示品質**: 文字の視認性、色の正確性
- **音質**: アラーム音の音量・音質
- **振動**: 振動機能の動作（実装時）

#### 長時間動作確認
- **安定性**: 8時間以上の連続動作
- **メモリリーク**: 長時間動作後のメモリ使用量
- **バッテリー消費**: バッテリー持続時間の確認
- **温度上昇**: 長時間動作時の温度変化

### 5.3 テスト手順

#### 基本動作確認手順
1. **電源投入**: バッテリーで起動
2. **初期画面確認**: メイン画面の表示確認
3. **モード遷移確認**: 各ボタンでの画面遷移
4. **入力機能確認**: 時刻入力の動作確認
5. **アラーム機能確認**: アラーム設定・鳴動確認
6. **設定機能確認**: 設定メニューの動作確認

#### 長時間動作確認手順
1. **初期状態記録**: メモリ使用量、バッテリー残量
2. **連続動作**: 8時間の連続動作
3. **定期的な確認**: 1時間ごとの動作確認
4. **最終状態記録**: 動作後の状態確認
5. **結果分析**: 性能劣化の有無確認

## 6. テスト実行の自動化

### 6.1 自動実行スクリプト

#### Unit Test自動実行
```python
# scripts/run_tests.py
def main():
    print("=== Unit Test実行 ===")
    if not run_unit_tests():
        print("✗ Unit Test失敗")
        return False
    
    print("=== 統合テスト実行 ===")
    if not run_integration_tests():
        print("✗ 統合テスト失敗")
        return False
    
    print("=== カバレッジ測定 ===")
    coverage = measure_coverage()
    print(f"カバレッジ: {coverage:.1f}%")
    
    if coverage < 80.0:
        print("✗ カバレッジ不足")
        return False
    
    print("✓ 全テスト成功")
    return True
```

#### カバレッジ測定
```python
def measure_coverage():
    # gcovを使用したカバレッジ測定
    run_command("gcov src/time_logic.cpp")
    run_command("gcov src/alarm.cpp")
    
    # カバレッジ結果の解析
    coverage = parse_gcov_results()
    return coverage
```

### 6.2 CI/CD統合

#### GitHub Actions設定
```yaml
# .github/workflows/test.yml
name: Test
on: [push, pull_request]

jobs:
  test:
    runs-on: windows-latest
    steps:
    - uses: actions/checkout@v2
    
    - name: Setup Python
      uses: actions/setup-python@v2
      with:
        python-version: 3.8
    
    - name: Install dependencies
      run: |
        pip install platformio
        pio platform install native
    
    - name: Run tests
      run: |
        python scripts/run_tests.py
    
    - name: Upload coverage
      uses: codecov/codecov-action@v1
```

## 7. テスト結果の管理

### 7.1 結果記録

#### テスト結果ファイル
```
test_results/
├── unit_test_results.json     # Unit Test結果
├── integration_test_results.json  # 統合テスト結果
├── coverage_report.html       # カバレッジレポート
└── performance_log.txt        # パフォーマンスログ
```

#### 結果フォーマット
```json
{
  "timestamp": "2025-01-XX HH:MM:SS",
  "test_type": "unit",
  "total_tests": 25,
  "passed_tests": 23,
  "failed_tests": 2,
  "coverage": 85.2,
  "execution_time": 12.5,
  "details": {
    "time_logic": {"passed": 8, "failed": 0},
    "alarm_logic": {"passed": 6, "failed": 1},
    "input_logic": {"passed": 5, "failed": 1},
    "settings_logic": {"passed": 4, "failed": 0}
  }
}
```

### 7.2 品質指標

#### 成功基準
- **Unit Test**: 全テスト通過、カバレッジ80%以上
- **統合テスト**: 全テスト通過、パフォーマンス基準達成
- **実機テスト**: 基本動作確認成功、長時間動作安定

#### 失敗時の対応
- **即座の修正**: 失敗したテストの即座の修正
- **原因分析**: 失敗原因の詳細分析
- **再テスト**: 修正後の完全再テスト

## 8. 次のステップ

### 8.1 Phase 0.3: Unityテスト環境の改善
- モック環境の完全実装
- ヘッダー競合の解決
- テストファイルの修正

### 8.2 Phase 0.4: テスト戦略の確立
- テスト実行の自動化
- カバレッジ測定の導入
- テスト結果の可視化

### 8.3 Phase 1以降: 継続的テスト
- 各フェーズでのテスト実行
- 新機能のテスト追加
- テストカバレッジの維持 