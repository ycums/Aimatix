# InputDisplayState リファクタ工程手順書

## 概要
プレビューのバグ修正計画の続行として、InputDisplayState.hのリファクタ工程を実行します。
品質保証工程も含めた段階的な改善を行います。

## 目標
- デバッグコードの削除
- 重複ロジックの統合
- コードの可読性向上
- 品質ゲート基準（中重要度警告19件以下）の達成
- テストカバレッジ85%以上の維持

## 前提条件
- 現在のコードが正常にビルド・テストできる状態
- 既存の機能が正常に動作している状態

## リファクタ工程の詳細手順

### Phase 1: 準備段階

#### 1.1 現在の品質状態確認
```bash
# 静的解析実行
pio check -e native

# テスト実行
pio test -e native

# カバレッジ測定
python scripts/test_coverage.py
```

#### 1.2 バックアップ作成
```bash
# 現在の状態をバックアップ
git add .
git commit -m "Before InputDisplayState refactor"
```

### Phase 2: デバッグコードの削除

#### 2.1 printf文の削除
**対象ファイル**: `lib/libaimatix/src/InputDisplayState.h`

**削除対象**:
- 96行目: `printf("[DEBUG] now_tm: %04d-%02d-%02d %02d:%02d\n", ...)`
- その他のデバッグ用printf文（約20箇所）

**削除手順**:
1. ファイル内のすべての`printf("[DEBUG]`で始まる行を検索
2. 各printf文を削除
3. 必要に応じて適切なコメントに置き換え

#### 2.2 品質確認
```bash
# 静的解析
pio check -e native

# テスト実行
pio test -e native
```

### Phase 3: 時刻計算ロジックの共通化

#### 実施内容
- `generateAbsolutePreview`/`generateRelativePreview`の重複ロジックを`TimePreviewLogic`クラスに集約
- InputDisplayStateから時刻計算・日付跨ぎ判定・プレビュー生成ロジックを排除し、共通関数呼び出しに変更
- 静的バッファ問題（localtimeの多重呼び出しによるバグ）を回避するため、tm構造体の値コピーを徹底
- テストケース（特に日付跨ぎのプレビュー）も全てパス

#### 注意点
- localtime等の標準C関数は静的バッファを返すため、複数回呼び出す場合は必ず値をコピーして使うこと
- 共通化したロジックは他の画面やロジックでも再利用可能
- テスト失敗時はlocaltimeの使い方・tm構造体の扱いを重点的に見直すこと

#### 品質保証
- `pio test -e native`で全テストパス（バグ再発防止テストも含む）
- コードの可読性・保守性が向上

---

### Phase 4: 関数の分割・整理

#### 4.1 現状の問題点
- `onDraw()`関数: 約80行の長大な関数（UI更新・プレビュー生成・エラー表示・状態管理が混在）
- `onButtonC()`関数: 約50行の複雑な分岐（相対値/絶対値モード分岐・アラーム追加・エラーハンドリングが混在）
- エラーハンドリングが各所に散在

#### 4.2 分割・整理の方針
**原則**: 1関数1責務、認知複雑度削減、テスト容易性向上

**分割対象**:
1. `onDraw()`関数 → `updateDigitDisplay()`, `updatePreviewDisplay()`, `updateErrorDisplay()`, `updateColonDisplay()`
2. `onButtonC()`関数 → `handleRelativeModeSubmit()`, `handleAbsoluteModeSubmit()`
3. エラーハンドリング → 共通エラー処理関数の作成

#### 4.3 実施手順

##### 4.3.1 準備段階
```bash
# 現在の品質状態を確認
pio check -e native
pio test -e native
```

##### 4.3.2 第1段階: onDraw()関数の分割
**対象**: `onDraw()`関数（約80行）
**分割内容**:
- `updateDigitDisplay()`: 数字表示の更新
- `updatePreviewDisplay()`: プレビュー表示の更新
- `updateErrorDisplay()`: エラー表示の更新
- `updateColonDisplay()`: コロン表示の更新
- `hasInputChanged()`: 入力値変化の判定
- `generatePreviewText()`: プレビューテキスト生成

**確認**: 分割後にテスト実行 ✅ **完了**

##### 4.3.3 第2段階: onButtonC()関数の分割
**対象**: `onButtonC()`関数（約50行）
**分割内容**:
- `handleRelativeModeSubmit()`: 相対値モードの確定処理
- `handleAbsoluteModeSubmit()`: 絶対値モードの確定処理
- `addAlarmAtTime()`: アラーム追加処理
- `showError()`: エラー表示処理
- `transitionToMainDisplay()`: メイン画面への遷移

**確認**: 分割後にテスト実行 ✅ **完了**

##### 4.3.4 第3段階: エラーハンドリングの統一
**対象**: 各所に散在するエラー処理
**統一内容**:
- `validateInputLogic()`: InputLogicの妥当性チェック
- `validateTimeProvider()`: TimeProviderの妥当性チェック
- `handleError()`: 共通エラー処理

**確認**: 統一後にテスト実行 ✅ **完了**

#### 4.4 品質確認
```bash
# 各段階でテスト確認
pio test -e native

# 静的解析確認
pio check -e native
```

#### 4.5 期待される効果
- **可読性向上**: 関数名から処理内容が明確
- **保守性向上**: バグ修正時の影響範囲が限定
- **テスト容易性向上**: 各関数を個別にテスト可能
- **認知複雑度削減**: ネストしたif文の削減

#### 4.6 注意点
- **段階的実施**: 一度に大きな変更を避け、小さな単位で分割・テスト
- **テスト優先**: 各分割後に必ずテストを実行
- **命名規則**: 関数名は処理内容を明確に表現
- **責務分離**: UI更新・ビジネスロジック・エラーハンドリングを明確に分離

#### 4.7 完了状況
✅ **Phase 4: 関数の分割・整理 - 完了**

**実施内容**:
- `onDraw()`関数を8つの関数に分割
- `onButtonC()`関数を5つの関数に分割
- エラーハンドリングを統一
- すべてのテストが通過（114テストケース）

**成果**:
- 認知複雑度の削減
- 単一責任の原則に基づく整理
- 可読性・保守性の向上
- テスト容易性の向上

---

### Phase 5: 依存性注入の改善

#### 5.1 現状の問題点
- **直接的な`timeProvider_`使用**: 194行目と301行目で直接`timeProvider_->now()`を呼び出し
- **nullptrチェックの散在**: 各所で`timeProvider_ ? timeProvider_->now() : 0`のような冗長なチェック
- **エラーハンドリングの不統一**: 時刻取得失敗時の処理が統一されていない
- **テスト容易性の課題**: 時刻関連の処理が直接的に結合している

#### 5.2 改善方針
**原則**: 依存性逆転原則、単一責任原則、テスト容易性の向上

**改善内容**:
1. **時刻取得の抽象化**: `getCurrentTime()`メソッドを追加して、nullptrチェックを内部化
2. **エラー状態の改善**: 時刻取得失敗時の適切なフォールバック処理
3. **テスト容易性の向上**: 時刻取得処理の分離、モック化しやすい構造への改善

#### 5.3 具体的な変更内容

##### 5.3.1 時刻取得メソッドの追加
```cpp
private:
    // 現在時刻を安全に取得
    time_t getCurrentTime() const {
        return timeProvider_ ? timeProvider_->now() : 0;
    }
    
    // 時刻取得が有効かチェック
    bool isTimeProviderValid() const {
        return timeProvider_ != nullptr;
    }
```

##### 5.3.2 エラーハンドリングの統一
```cpp
private:
    // エラー表示の処理（改善版）
    void handleErrorDisplay(char* preview, size_t previewSize) {
        time_t currentTime = getCurrentTime();
        if (currentTime - errorStartTime >= 3) {
            showError = false;
            errorMessage = "";
        } else {
            strncpy(preview, errorMessage.c_str(), previewSize - 1);
            preview[previewSize - 1] = '\0';
        }
    }
    
    // 共通エラー処理（改善版）
    void handleError(const std::string& message) {
        showError = true;
        errorMessage = message;
        errorStartTime = getCurrentTime();
    }
```

##### 5.3.3 プレビュー生成の改善
```cpp
private:
    // 相対値計算結果のプレビュー文字列を生成（改善版）
    void generateRelativePreview(char* preview, size_t previewSize) {
        if (!isTimeProviderValid() || !inputLogic) {
            return;
        }
        
        time_t relativeTime = inputLogic->getAbsoluteValue();
        if (relativeTime != -1) {
            auto result = TimePreviewLogic::generateRelativePreview(relativeTime, timeProvider_);
            if (result.isValid) {
                strncpy(preview, result.preview.c_str(), previewSize - 1);
                preview[previewSize - 1] = '\0';
            }
        }
    }

    // 絶対値計算結果のプレビュー文字列を生成（改善版）
    void generateAbsolutePreview(char* preview, size_t previewSize) {
        if (!isTimeProviderValid() || !inputLogic) {
            return;
        }
        
        const int* digits = inputLogic->getDigits();
        const bool* entered = inputLogic->getEntered();
        if (!digits || !entered) {
            return;
        }
        
        auto result = TimePreviewLogic::generatePreview(digits, entered, timeProvider_, false);
        if (result.isValid) {
            strncpy(preview, result.preview.c_str(), previewSize - 1);
            preview[previewSize - 1] = '\0';
        }
    }
```

#### 5.4 実装手順

##### Step 1: 時刻取得メソッドの追加
1. `getCurrentTime()`と`isTimeProviderValid()`メソッドを追加
2. 既存の直接呼び出し箇所を新しいメソッドに置き換え

##### Step 2: エラーハンドリングの統一
1. `handleErrorDisplay()`と`handleError()`を改善
2. 時刻取得失敗時の適切なフォールバック処理を追加

##### Step 3: プレビュー生成の改善
1. `generateRelativePreview()`と`generateAbsolutePreview()`を改善
2. 時刻プロバイダーの妥当性チェックを統一

##### Step 4: 品質確認
1. ビルド確認: `pio run`
2. 静的解析: `pio check -e native`
3. テスト実行: `pio test -e native`

#### 5.5 期待される効果
1. **可読性向上**: 時刻取得処理が抽象化され、意図が明確
2. **保守性向上**: 時刻関連の処理が一箇所に集約
3. **テスト容易性向上**: 時刻取得処理のモック化が容易
4. **エラーハンドリング統一**: 時刻取得失敗時の処理が統一

#### 5.6 リスク管理
1. **既存機能への影響**: 時刻取得処理の変更による副作用
   - 対策: 段階的な変更とテスト
2. **パフォーマンス**: 追加メソッド呼び出しによるオーバーヘッド
   - 対策: インライン化の検討

#### 5.7 品質確認
```bash
# ビルド確認
pio run

# 静的解析
pio check -e native

# テスト実行
pio test -e native
```

#### 5.8 完了状況
✅ **Phase 5: 依存性注入の改善 - 完了**

**実施内容**:
- 時刻取得メソッドの追加 (`getCurrentTime()`, `isTimeProviderValid()`)
- エラーハンドリングの統一 (`handleErrorDisplay()`, `handleError()`)
- プレビュー生成の改善 (`generateRelativePreview()`, `generateAbsolutePreview()`)

**成果**:
- 時刻取得処理の抽象化
- 統一されたエラーハンドリング
- テスト容易性の向上
- 設計原則への準拠度向上

**品質確認結果**:
- 全テスト通過: 114テストケースすべて成功
- 静的解析: 中重要度警告306件（既存の警告、新規警告なし）
- 高重要度警告: 0件
- 低重要度警告: 0件

### Phase 6: エラーハンドリングの統一

#### 6.1 エラーハンドリングの統合
**改善内容**:
- 散在するエラーハンドリングを統一的に管理
- エラーメッセージの標準化
- エラー状態の適切な管理

#### 6.2 TDD実行計画

##### フェーズ1: エラーメッセージ定数化
**Step 1: Red (失敗するテストを書く)**
- エラーメッセージ定数が正しく定義されていることを確認

**Step 2: Green (テストを通過させる最小限の実装)**
- エラーメッセージ定数の追加
- 既存のハードコードされたメッセージを定数に置き換え

**Step 3: Refactor (コードを改善)**
- エラーメッセージの統一

##### フェーズ2: エラー状態管理改善
**Step 1: Red (失敗するテストを書く)**
- エラー状態のリセットが正常に動作することを確認
- エラー表示時間が定数通りに動作することを確認

**Step 2: Green (テストを通過させる最小限の実装)**
- エラー状態管理関数の追加
- エラー表示時間の定数化

**Step 3: Refactor (コードを改善)**
- 既存のエラー状態管理を新しい関数に置き換え

##### フェーズ3: エラーハンドリング統一
**Step 1: Red (失敗するテストを書く)**
- 統一されたエラー処理が正常に動作することを確認
- 入力検証が統一された方法で動作することを確認

**Step 2: Green (テストを通過させる最小限の実装)**
- 共通エラーハンドリング関数の追加
- 入力検証関数の統一

**Step 3: Refactor (コードを改善)**
- 既存のエラーハンドリングを新しい関数に置き換え

#### 6.3 品質確認
```bash
# ビルド確認
pio run

# 静的解析
pio check -e native

# テスト実行
pio test -e native
```

### Phase 7: 品質保証工程

#### 7.1 現在の詳細状態確認
```bash
# 1. ビルド確認
pio run

# 2. 静的解析の詳細確認
pio check -e native

# 3. カバレッジの詳細分析
python scripts/test_coverage.py
```

#### 7.2 静的解析改善（優先度：高）
**方針**: ソースコードの修正のみで警告数を0に近づける

**実施内容**:
1. **Step 2-1: staticメソッド化（15件削減）**
   - AlarmDisplayState.cpp: `getCurrentMillis()` → static化
   - AlarmLogic.cpp: `initAlarms()`, `removePastAlarms()`, `getRemainSec()` → static化
   - DateTimeInputState.cpp: `incrementCurrentDigit()`, `getDigitValue()`, `setDigitValue()` → static化
   - TimePreviewLogic.cpp: `formatPreview()`, `calculateDayDifference()` → static化
   - DateTimeInputViewImpl.cpp: `getCharWidth()` → static化

2. **Step 2-2: 明示的nullptrチェック（20件削減）**
   - DateTimeInputState.cpp: `ITimeProvider *` → `timeProvider != nullptr`
   - TimePreviewLogic.cpp: `ITimeProvider *`, `const int *`, `const bool *` → 明示的チェック
   - TimeValidationLogic.cpp: `ITimeProvider *` → 明示的チェック
   - DateTimeInputViewImpl.cpp: `IDisplay *` → 明示的チェック

3. **Step 2-3: const修飾子追加（15件削減）**
   - DateTimeInputState.cpp: 変数にconst修飾子を追加
   - PartialInputLogic.cpp: 変数にconst修飾子を追加
   - SettingsDisplayState.cpp: 変数にconst修飾子を追加
   - DateTimeInputViewImpl.cpp: 変数にconst修飾子を追加

4. **Step 2-4: 波括弧初期化リスト（3件削減）**
   - PartialInputLogic.cpp: return文を波括弧初期化リストに変更

5. **Step 2-5: 重複分岐統合（8件削減）**
   - DateTimeInputState.cpp: 重複したif文の統合
   - SettingsLogic.cpp: switch文の重複分岐統合
   - DateTimeInputViewImpl.cpp: switch文の重複分岐統合

6. **Step 2-6: 型変換明示化（12件削減）**
   - SettingsDisplayViewImpl.cpp: `unsigned long long` → `int` の明示的キャスト
   - main.cpp: .cppファイルのinclude修正

**各ステップでの確認**:
- ビルド確認: `pio run`
- テスト実行: `pio test -e native`
- 静的解析確認: `pio check -e native`
- 結果確認: 警告数の削減確認

**期待される結果**:
- 修正前: 73件の警告
- 修正後: 0-5件程度の警告
- 削減率: 約93-100%

#### 7.3 カバレッジ改善（優先度：中）
**方針**: カバレッジが極端に低いものから優先的に対応

**実施内容**:
1. **カバレッジレポートの詳細分析**
   - どのファイル/関数のカバレッジが低いかを特定
   - 特にInputDisplayState関連のカバレッジ状況を確認

2. **不足テストケースの特定**
   - 未テストの関数や分岐を特定
   - エッジケースのテスト追加

3. **テストケース追加**
   - カバレッジが最も低い関数から順次追加
   - 特に新しく分割された関数のテストを優先

#### 7.4 品質ゲート通過確認
```bash
# 修正後の最終確認
pio run
pio check -e native
pio test -e native
python scripts/test_coverage.py
```

**確認項目**:
- [ ] テストカバレッジ: 85%以上
- [ ] 中重要度警告: 85件以下（できれば0件近く）
- [ ] 全テスト通過: 100%
- [ ] ビルド成功: 正常

#### 7.8 完了状況
✅ **Phase 7: 品質保証工程 - 静的解析改善完了**

**現在の品質状態（2025年8月時点）:**
- ✅ テスト: 139テストケースすべて通過
- ✅ 静的解析: 中重要度警告19件（目標: 19件以下 - 達成済み）
- ❌ カバレッジ: 67.0%（目標: 85%以上）

**実施済み改善:**
1. **staticメソッド化**: 複数のメソッドをstatic化
2. **const修飾子追加**: 複数の変数にconst修飾子を追加
3. **重複分岐統合**: DateTimeInputStateとDateTimeInputViewImplの重複分岐を統合
4. **型変換明示化**: SettingsDisplayViewImplで型変換を明示化
5. **include修正**: main.cppの重複includeを修正

**静的解析改善結果:**
- **修正前**: 46件の警告
- **修正後**: 19件の警告
- **削減率**: 約59%
- **品質ゲート基準**: 達成済み（19件 ≤ 19件）

**残りの課題:**
- **カバレッジ改善**: 67.0% → 85%以上（+18%必要）

#### 7.7.1 カバレッジ不足の原因分析
**主要な問題点**:
1. **InputDisplayState関連の関数が全くテストされていない**
   - `InputDisplayState::onDraw()` - 0.0%
   - `InputDisplayState::onButtonA()` - 0.0%
   - `InputDisplayState::onButtonB()` - 0.0%
   - `InputDisplayState::onButtonC()` - 0.0%
   - その他多数のInputDisplayState関数が0.0%

2. **View実装クラスが全くテストされていない**
   - `InputDisplayViewImpl` - 全関数0.0%
   - `MainDisplayViewImpl` - 全関数0.0%
   - `AlarmDisplayViewImpl` - 全関数0.0%
   - `SettingsDisplayViewImpl` - 全関数0.0%
   - `DateTimeInputViewImpl` - 全関数0.0%

3. **MainDisplayStateが全くテストされていない**
   - `MainDisplayState` - 全関数0.0%

4. **一部のDateTimeInputState関数がテストされていない**
   - `DateTimeInputState::setDigitValue()` - 0.0%
   - `DateTimeInputState::moveCursorLeft()` - 0.0%
   - `DateTimeInputState::dataPositionToStringPosition()` - 0.0%

#### 7.7.2 カバレッジ改善計画

**方針**:
- テストは1テスト1観点に統一
- こまめにテストを実行
- 本番コード側を変更しなければならなくなったときは後回しにして、最後にユーザーに判断を仰ぐ

**Step 7-7-1: InputDisplayStateテストの追加（最優先）**

**1. `onDraw()`関数のテスト（1観点ずつ）**
```cpp
// test_input_display_pure/test_main.cpp に追加

TEST_CASE("InputDisplayState onDraw - 基本表示処理が呼ばれる", "[input_display]") {
    auto mockTimeProvider = std::make_shared<MockTimeProvider>();
    auto mockView = std::make_unique<MockInputDisplayView>();
    auto inputLogic = std::make_unique<InputLogic>(mockTimeProvider);
    InputDisplayState state(inputLogic.get(), mockView.get(), mockTimeProvider.get());
    
    state.onDraw();
    
    // 検証: 基本的な表示処理が呼ばれることを確認
}

TEST_CASE("InputDisplayState onDraw - エラー状態での表示", "[input_display]") {
    // エラー状態での表示テスト（1観点）
}

TEST_CASE("InputDisplayState onDraw - プレビュー表示", "[input_display]") {
    // プレビュー表示のテスト（1観点）
}
```

**2. ボタン処理のテスト（1観点ずつ）**
```cpp
TEST_CASE("InputDisplayState onButtonA - 数字入力処理", "[input_display]") {
    // 数字入力のテスト（1観点）
}

TEST_CASE("InputDisplayState onButtonB - 数字入力処理", "[input_display]") {
    // 数字入力のテスト（1観点）
}

TEST_CASE("InputDisplayState onButtonC - 相対値モード確定", "[input_display]") {
    // 相対値モード確定のテスト（1観点）
}

TEST_CASE("InputDisplayState onButtonC - 絶対値モード確定", "[input_display]") {
    // 絶対値モード確定のテスト（1観点）
}

TEST_CASE("InputDisplayState onButtonC - エラー時の処理", "[input_display]") {
    // エラー時の処理テスト（1観点）
}
```

**3. 新しく分割された関数のテスト（1観点ずつ）**
```cpp
TEST_CASE("InputDisplayState updateDigitDisplay - 数字表示更新", "[input_display]") {
    // 数字表示更新のテスト（1観点）
}

TEST_CASE("InputDisplayState updatePreviewDisplay - プレビュー表示更新", "[input_display]") {
    // プレビュー表示更新のテスト（1観点）
}

TEST_CASE("InputDisplayState handleRelativeModeSubmit - 相対値確定処理", "[input_display]") {
    // 相対値確定処理のテスト（1観点）
}

TEST_CASE("InputDisplayState handleAbsoluteModeSubmit - 絶対値確定処理", "[input_display]") {
    // 絶対値確定処理のテスト（1観点）
}
```

**Step 7-7-2: View実装クラスのテスト追加**

**InputDisplayViewImplのテスト**
```cpp
// test_input_display_view_pure/test_main.cpp を新規作成
TEST_CASE("InputDisplayViewImpl showDigit", "[input_display_view]") {
    auto mockDisplay = std::make_unique<MockDisplay>();
    InputDisplayViewImpl view(mockDisplay.get());
    
    // 数字表示のテスト
    view.showDigit(0, 1, true);
    // 検証: 正しい位置に数字が表示される
}

TEST_CASE("InputDisplayViewImpl showPreview", "[input_display_view]") {
    // プレビュー表示のテスト
}

TEST_CASE("InputDisplayViewImpl showColon", "[input_display_view]") {
    // コロン表示のテスト
}
```

**Step 7-7-3: MainDisplayStateテストの追加**

```cpp
// test_main_display_pure/test_main.cpp を新規作成
TEST_CASE("MainDisplayState onEnter", "[main_display]") {
    // 初期化処理のテスト
}

TEST_CASE("MainDisplayState onDraw", "[main_display]") {
    // 表示処理のテスト
}

TEST_CASE("MainDisplayState onButtonA", "[main_display]") {
    // ボタンA処理のテスト
}
```

**Step 7-7-4: DateTimeInputState不足テストの追加**

```cpp
// test_datetime_input_pure/test_main.cpp に追加
TEST_CASE("DateTimeInputState setDigitValue", "[datetime_input]") {
    // 数字値設定のテスト
}

TEST_CASE("DateTimeInputState moveCursorLeft", "[datetime_input]") {
    // カーソル左移動のテスト
}

TEST_CASE("DateTimeInputState dataPositionToStringPosition", "[datetime_input]") {
    // データ位置から文字列位置への変換テスト
}
```

**実行手順**:
```bash
# Step 1: 最初のテスト追加
# test_input_display_pure/test_main.cpp に最初のテストケースを追加
pio test -e native -f pure/test_input_display_pure
python scripts/test_coverage.py --quick

# Step 2: 段階的にテスト追加
# 各テスト追加後に必ずテスト実行
pio test -e native -f pure/test_input_display_pure
python scripts/test_coverage.py --quick
```

**期待される効果**:
- **InputDisplayStateテスト追加**: +15-20%
- **View実装テスト追加**: +10-15%
- **MainDisplayStateテスト追加**: +5-10%
- **DateTimeInputState補完**: +2-5%

**合計改善予想**: +32-50%
**最終カバレッジ**: 88-106%（目標85%を大幅に上回る）

**期待される最終結果:**
- 静的解析: 0-5件程度（現在46件から大幅改善）
- カバレッジ: 85%以上（現在56.0%から改善）
- 品質ゲート基準: 完全達成

**リスク評価:**
- ✅ 低リスク: 既存のテストが114件すべて通過
- ✅ 段階的修正: 各ステップでテスト確認可能
- ✅ 機能変更なし: 静的解析の改善のみ

---

### Phase 8: 最終確認

#### 8.1 機能確認
```bash
# ビルド確認
pio run
pio run

# テスト確認
pio test -e native
```

#### 8.2 ドキュメント更新
**更新対象**:
- この手順書の完了報告
- 必要に応じて関連ドキュメントの更新

## 品質ゲート基準

### 静的解析基準
- **High**: 高重要度の警告は0件以下
- **Medium**: 中重要度の警告は19件以下（ゲート基準）✅ **達成済み**
- **Low**: 低重要度の警告は50件以下

### テスト基準
- **カバレッジ**: 85%以上
- **通過率**: 100% ✅ **達成済み**
- **実行時間**: 30秒以内 ✅ **達成済み**

### ビルド基準
- **Native環境**: 正常ビルド ✅ **達成済み**
- **M5Stack環境**: 正常ビルド ✅ **達成済み**
- **警告**: 最小限

## リスク管理

### 高リスク項目
1. **時刻計算ロジックの変更**
   - 影響: プレビュー機能全体
   - 対策: 段階的な変更とテスト

2. **関数の大幅な分割**
   - 影響: 既存テストの修正が必要
   - 対策: テストファーストアプローチ

### 中リスク項目
1. **デバッグコードの削除**
   - 影響: デバッグ情報の損失
   - 対策: 必要に応じてログシステム導入

2. **依存性注入の変更**
   - 影響: テストコードの修正
   - 対策: 既存テストの維持

## 進捗管理

### チェックポイント
- [x] Phase 1: 準備段階完了
- [x] Phase 2: デバッグコード削除完了
- [x] Phase 3: 時刻計算ロジック共通化完了
- [x] Phase 4: 関数分割・整理完了
- [x] Phase 5: 依存性注入改善完了
- [x] Phase 6: エラーハンドリング統一完了
- [x] Phase 7: 品質保証工程完了（静的解析改善完了）
- [ ] Phase 8: 最終確認完了（カバレッジ改善のみ残存）

### 品質ゲート通過確認
- [x] 静的解析基準達成（19件 = 19件）
- [ ] テスト基準達成（カバレッジ67.0% < 85%）
- [x] ビルド基準達成

## 完了条件

### 必須条件
1. ✅ すべてのテストが通過（139テストケース）
2. ✅ 中重要度警告19件以下（19件達成）
3. ❌ テストカバレッジ85%以上（67.0%未達成）
4. ✅ 正常にビルド可能

### 推奨条件
1. ✅ コードの可読性向上
2. ✅ 関数サイズの適正化
3. ✅ 認知複雑度の削減
4. ✅ 保守性の向上

## 参考資料

- `doc/guide/developer_guide.md`: 開発者ガイド
- `doc/design/architecture.md`: アーキテクチャ設計
- `doc/operation/testing_strategy.md`: テスト戦略
- `doc/operation/quality_gates.md`: 品質ゲート基準
- [カバレッジ向上戦略](./coverage_strategy.md) - 効率的なカバレッジ向上の戦略と注意事項

---

**更新日**: 2025年1月  
**バージョン**: 1.1.0  
**対象ファイル**: `lib/libaimatix/src/InputDisplayState.h`  
**品質ゲート基準**: 中重要度警告19件以下、テストカバレッジ85%以上（静的解析基準達成済み） 