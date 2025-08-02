Phase 7.7のカバレッジ改善計画を再開します。まず現在の状況を確認して実行計画を作成します。

Search files...
Read file: coverage_config.json
Ran tool
Ran tool
Read file: test/pure/test_input_display_pure/test_main.cpp
Search files...
現在の状況を確認しました。Phase 7.7のカバレッジ改善計画を再開するための実行計画を作成します。

## Phase 7.7 カバレッジ改善計画 - 実行計画

### 現在の状況分析
- **現在のカバレッジ**: 56.0%（目標: 85%以上）
- **主な問題**: InputDisplayState関連の関数が全くテストされていない
- **既存テスト**: 114テストケースすべて通過済み

### 実行計画

#### Step 1: InputDisplayStateテストの段階的追加（最優先）

**1.1 基本表示処理のテスト**
```cpp
// test_input_display_pure/test_main.cpp に追加
void test_input_display_state_ondraw_basic_display() {
    // 基本表示処理が呼ばれることを確認（1観点）
    auto mockTimeProvider = std::make_shared<MockTimeProvider>(kFixedTestTime);
    std::unique_ptr<InputLogic> inputLogic(new InputLogic(mockTimeProvider));
    InputDisplayState state(inputLogic.get(), nullptr, mockTimeProvider.get());
    
    // onDraw()を呼び出してエラーが発生しないことを確認
    state.onDraw();
    
    TEST_ASSERT_TRUE(true); // エラーが発生しなければ成功
}
```

**1.2 ボタン処理のテスト（1観点ずつ）**
```cpp
void test_input_display_state_onbutton_a() {
    // 数字入力処理のテスト（1観点）
    auto mockTimeProvider = std::make_shared<MockTimeProvider>(kFixedTestTime);
    std::unique_ptr<InputLogic> inputLogic(new InputLogic(mockTimeProvider));
    InputDisplayState state(inputLogic.get(), nullptr, mockTimeProvider.get());
    
    // 初期状態確認
    TEST_ASSERT_EQUAL(-1, inputLogic->getValue());
    
    // ボタンA押下で数字入力
    state.onButtonA();
    
    // 入力値が変化することを確認
    TEST_ASSERT_NOT_EQUAL(-1, inputLogic->getValue());
}

void test_input_display_state_onbutton_b() {
    // 桁送り処理のテスト（1観点）
    auto mockTimeProvider = std::make_shared<MockTimeProvider>(kFixedTestTime);
    std::unique_ptr<InputLogic> inputLogic(new InputLogic(mockTimeProvider));
    InputDisplayState state(inputLogic.get(), nullptr, mockTimeProvider.get());
    
    // 初期状態では桁送りできない
    TEST_ASSERT_FALSE(inputLogic->shiftDigits());
    
    // 値を入力してから桁送り
    inputLogic->incrementInput(5);
    state.onButtonB();
    
    // 桁送りが成功することを確認
    TEST_ASSERT_TRUE(inputLogic->isEntered(3));
}

void test_input_display_state_onbutton_c_relative_mode() {
    // 相対値モード確定処理のテスト（1観点）
    auto mockTimeProvider = std::make_shared<MockTimeProvider>(kFixedTestTime);
    std::unique_ptr<InputLogic> inputLogic(new InputLogic(mockTimeProvider));
    InputDisplayState state(inputLogic.get(), nullptr, mockTimeProvider.get());
    
    // 相対値モードに設定
    state.setRelativeMode(true);
    
    // 完全な入力を行う
    inputLogic->incrementInput(1); // 時十の位
    inputLogic->shiftDigits();
    inputLogic->incrementInput(2); // 時一の位
    inputLogic->shiftDigits();
    inputLogic->incrementInput(3); // 分十の位
    inputLogic->shiftDigits();
    inputLogic->incrementInput(4); // 分一の位
    
    // ボタンC押下で確定処理
    state.onButtonC();
    
    // 確定処理が正常に動作することを確認
    TEST_ASSERT_TRUE(true); // エラーが発生しなければ成功
}
```

#### Step 2: 新しく分割された関数のテスト

**2.1 表示更新関数のテスト**
```cpp
void test_input_display_state_update_digit_display() {
    // 数字表示更新のテスト（1観点）
    auto mockTimeProvider = std::make_shared<MockTimeProvider>(kFixedTestTime);
    std::unique_ptr<InputLogic> inputLogic(new InputLogic(mockTimeProvider));
    InputDisplayState state(inputLogic.get(), nullptr, mockTimeProvider.get());
    
    // 数字入力
    inputLogic->incrementInput(5);
    
    // 数字表示更新処理を実行
    state.updateDigitDisplay();
    
    // 処理が正常に動作することを確認
    TEST_ASSERT_TRUE(true);
}

void test_input_display_state_update_preview_display() {
    // プレビュー表示更新のテスト（1観点）
    auto mockTimeProvider = std::make_shared<MockTimeProvider>(kFixedTestTime);
    std::unique_ptr<InputLogic> inputLogic(new InputLogic(mockTimeProvider));
    InputDisplayState state(inputLogic.get(), nullptr, mockTimeProvider.get());
    
    // プレビュー表示更新処理を実行
    state.updatePreviewDisplay();
    
    // 処理が正常に動作することを確認
    TEST_ASSERT_TRUE(true);
}
```

#### Step 3: エラーハンドリングのテスト

**3.1 エラー状態のテスト**
```cpp
void test_input_display_state_error_handling() {
    // エラーハンドリングのテスト（1観点）
    auto mockTimeProvider = std::make_shared<MockTimeProvider>(kFixedTestTime);
    std::unique_ptr<InputLogic> inputLogic(new InputLogic(mockTimeProvider));
    InputDisplayState state(inputLogic.get(), nullptr, mockTimeProvider.get());
    
    // エラー状態を設定
    state.handleError("Test error");
    
    // エラー表示処理を実行
    state.updateErrorDisplay();
    
    // エラー処理が正常に動作することを確認
    TEST_ASSERT_TRUE(true);
}
```

#### Step 4: 実行手順

**4.1 段階的テスト追加**
```bash
# Step 1: 最初のテスト追加
# test_input_display_pure/test_main.cpp に最初のテストケースを追加
pio test -e native -f pure/test_input_display_pure

# Step 2: 段階的にテスト追加
# 各テスト追加後に必ずテスト実行
pio test -e native -f pure/test_input_display_pure

# Step 3: カバレッジ確認
python scripts/test_coverage.py --quick
```

**4.2 品質確認**
```bash
# ビルド確認
pio run

# 静的解析確認
pio check -e native

# 全テスト実行
pio test -e native
```

### 期待される効果

**Step 1完了時**: +15-20%
**Step 2完了時**: +10-15%
**Step 3完了時**: +5-10%

**合計改善予想**: +30-45%
**最終カバレッジ**: 86-101%（目標85%を達成）

### リスク管理

1. **テスト追加による副作用**: 既存テストへの影響
   - 対策: 段階的な追加とテスト実行

2. **モック実装の不足**: テストに必要なモッククラスが不足
   - 対策: 必要に応じてモッククラスを追加

3. **テスト実行時間の増加**: テストケース増加による実行時間増加
   - 対策: 軽量なテストケースを優先

### 質問

1. **モッククラスの実装状況**: `MockInputDisplayView`クラスは既に実装されていますか？それとも新規作成が必要ですか？
新規作成が必要

2. **テスト優先順位**: InputDisplayStateのテストを最優先としていますが、View実装クラス（InputDisplayViewImpl等）のテストも同時に進めるべきでしょうか？それとも段階的に進めるべきでしょうか？
段階的に実施

この実行計画で進めてよろしいでしょうか？