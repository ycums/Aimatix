## バグフィックス対応計画（TDDベース）

### 1. バグの詳細分析

#### バグ1: `_1:00` → 期待値: `+1d 01:00`, 実際: `01:00`
**原因**: 部分入力時のプレビュー表示で、絶対時刻入力モードでも`PartialInputLogic::formatTime`を使用しているため、過去時刻の翌日処理が適用されていない。

#### バグ2: `__:_1` → 期待値: `13:01`, 実際: `00:01`
**原因**: 同様に部分入力時のプレビュー表示の問題。

### 2. TDDベースの実装手順

#### Phase 1: テストファースト（Red）
**目標**: 失敗するテストケースを作成

**作業内容**:
1. **新規テストケース作成**
   ```bash
   # テストファイルの確認
   test/pure/test_input_display_pure/test_main.cpp
   ```

2. **バグ再現テストケース追加**
   ```cpp
   // test_input_display_pure/test_main.cpp
   void test_bugfix_1_hour_only_digit() {
       // 現在時刻を12:34に設定
       MockTimeProvider mockTimeProvider;
       mockTimeProvider.setCurrentTime(12, 34);
       
       // 入力状態: _1:00
       InputLogic inputLogic(&mockTimeProvider);
       inputLogic.setDigit(1, 1);  // 時一桁に1を設定
       inputLogic.setEntered(1, true);
       
       // 絶対時刻入力モードでプレビュー生成
       InputDisplayState state;
       state.setInputLogic(&inputLogic);
       state.setRelativeMode(false);
       
       char preview[32] = "";
       state.generateAbsolutePreview(preview, sizeof(preview));
       
       // 期待値: +1d 01:00
       TEST_ASSERT_EQUAL_STRING("+1d 01:00", preview);
   }
   
   void test_bugfix_2_minute_only_digit() {
       // 現在時刻を12:34に設定
       MockTimeProvider mockTimeProvider;
       mockTimeProvider.setCurrentTime(12, 34);
       
       // 入力状態: __:_1
       InputLogic inputLogic(&mockTimeProvider);
       inputLogic.setDigit(3, 1);  // 分一桁に1を設定
       inputLogic.setEntered(3, true);
       
       // 絶対時刻入力モードでプレビュー生成
       InputDisplayState state;
       state.setInputLogic(&inputLogic);
       state.setRelativeMode(false);
       
       char preview[32] = "";
       state.generateAbsolutePreview(preview, sizeof(preview));
       
       // 期待値: 13:01
       TEST_ASSERT_EQUAL_STRING("13:01", preview);
   }
   ```

3. **テスト実行（失敗確認）**
   ```bash
   # テスト実行（失敗することを確認）
   pio test -e native -f pure/test_input_display_pure
   ```

#### Phase 2: 実装（Green）
**目標**: テストを成功させる最小限の実装

**作業内容**:
1. **修正実装**
   ```cpp
   // lib/libaimatix/src/InputDisplayState.h
   // 部分入力時のプレビュー表示ロジック修正
   
   // 修正前
   } else {
       // 絶対時刻入力モードの場合：確定処理と同じロジックで表示
       std::string timeStr = PartialInputLogic::formatTime(parsedTime.hour, parsedTime.minute);
       strncpy(preview, timeStr.c_str(), sizeof(preview) - 1);
       preview[sizeof(preview) - 1] = '\0';
   }
   
   // 修正後
   } else {
       // 絶対時刻入力モードの場合：確定処理と同じロジックで表示
       generateAbsolutePreview(preview, sizeof(preview));
   }
   ```

2. **ビルド確認**
   ```bash
   # ビルドエラーチェック
   pio run -e native
   ```

3. **テスト実行（成功確認）**
   ```bash
   # テスト実行（成功することを確認）
   pio test -e native -f pure/test_input_display_pure
   ```

#### Phase 3: リファクタリング（Refactor）
**目標**: コードの品質向上と保守性の確保

**作業内容**:
1. **静的解析実行**
   ```bash
   # Clang-Tidy静的解析
   pio check -e native
   ```

2. **品質ゲート確認**
   ```bash
   # 警告件数の確認（85件以下）
   Component            HIGH    MEDIUM    LOW
   ------------------  ------  --------  -----
   lib\libaimatix\src    0        78       0
   src                   0        22       0
   
   Total                 0       100       0  # ← 85件以下で合格
   ```

3. **コードレビュー**
   - 修正内容が仕様に適合しているか
   - 既存機能に影響がないか
   - テストケースが十分か

#### Phase 4: 統合テスト・回帰テスト
**目標**: 修正による影響範囲の確認

**作業内容**:
1. **全テスト実行**
   ```bash
   # 全テスト実行
   pio test -e native
   ```

2. **カバレッジ測定**
   ```bash
   # テストカバレッジ測定
   python scripts/test_coverage.py
   ```

3. **既存テストケースの確認**
   ```bash
   # 関連テストの実行
   pio test -e native -f pure/test_partial_input_logic_pure
   pio test -e native -f pure/test_alarm_logic_pure
   ```

#### Phase 5: 最終検証・品質保証
**目標**: 修正内容の最終確認と品質保証

**作業内容**:
1. **最終テスト実行**
   ```bash
   # 最終テスト実行
   pio test -e native
   ```

2. **最終静的解析**
   ```bash
   # 最終静的解析
   pio check -e native
   ```

3. **品質チェックリスト確認**
   - [ ] 修正内容が仕様に適合しているか
   - [ ] 既存機能に影響がないか
   - [ ] テストケースが十分か
   - [ ] 静的解析警告が適切に対処されているか
   - [ ] 品質ゲート基準（85件以下）を満たしているか

### 3. TDDサイクル詳細

#### サイクル1: バグ1の修正
1. **Red**: `test_bugfix_1_hour_only_digit()`を作成（失敗）
2. **Green**: 最小限の修正でテストを成功
3. **Refactor**: コード品質の向上

#### サイクル2: バグ2の修正
1. **Red**: `test_bugfix_2_minute_only_digit()`を作成（失敗）
2. **Green**: 最小限の修正でテストを成功
3. **Refactor**: コード品質の向上

#### サイクル3: 統合確認
1. **Red**: 既存テストの確認
2. **Green**: 全テストが成功
3. **Refactor**: 最終品質チェック

### 4. 成功基準

#### 機能面
- ✅ バグ1: `_1:00` → `+1d 01:00`が正しく表示される
- ✅ バグ2: `__:_1` → `13:01`が正しく表示される
- ✅ 既存機能に影響がない

#### 品質面
- ✅ ビルドエラーなし
- ✅ 全テスト成功
- ✅ 中重要度警告85件以下
- ✅ 既存カバレッジ維持

### 5. リスク管理

#### 低リスク項目
- 既存の完全入力時の処理は変更なし
- 相対時刻入力モードは影響なし
- 確定処理は既に正しく動作している

#### 注意点
- 部分入力時のプレビュー表示のみ変更
- 既存のテストケースへの影響を確認
- 静的解析警告の増加を監視

## 回答

1. **テストケースの追加場所**: バグ修正用のテストケースは`test_input_display_pure`に追加するべきでしょうか？それとも`test_partial_input_logic_pure`に追加するべきでしょうか？
→ lib/libaimatix/src/InputDisplayState.h しか変更しないのであれば test_input_display_pure のみ追加でOKです。

2. **モックの必要性**: テストケースで`MockTimeProvider`を使用して現在時刻を固定する必要がありますが、既存のモック実装で十分でしょうか？
→ おそらく yes. 不足なら変更してください。

3. **TDサイクルの粒度**: バグ1とバグ2を別々のサイクルで修正するべきでしょうか？それとも一つのサイクルで両方修正するべきでしょうか？
→ 同一箇所の問題であれば1サイクルでよいのでは?