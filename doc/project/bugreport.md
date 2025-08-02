Phase 7の品質保証工程を再開します。現在の状況を確認し、実行計画を立案します。

Search files...
Ran tool
Read file: doc/project/input_display_state_refactor_plan.md
現在の状況を確認するため、実際の静的解析結果を取得します。

Search files...
現在の状況を把握するため、実際のファイルを確認します。

Ran tool
Ran tool
## Phase 7 品質保証工程 - 実行計画

現在の状況を分析し、Phase 7の実行計画を立案します。

### 現在の状況確認

ドキュメントから以下の情報を確認しました：
- **現在の警告数**: 46件（中重要度）
- **目標**: 0-5件程度
- **テストカバレッジ**: 56.0%（目標: 85%以上）
- **テスト通過**: 114テストケースすべて通過済み

### 実行計画

#### Step 7-1: 現在の品質状態の詳細確認

まず、現在の具体的な警告内容を確認します：

```bash
# 1. 現在の静的解析結果を詳細確認
pio check -e native

# 2. テストカバレッジの詳細確認
python scripts/test_coverage.py

# 3. ビルド確認
pio run
```

#### Step 7-2: 静的解析改善の段階的実行

**Step 7-2-1: AlarmLogicのstatic化（8件削減予定）**
- 対象: `lib/libaimatix/src/AlarmLogic.cpp`
- 問題: `auto`キーワード使用によるstatic化の認識問題
- 解決策: `auto`を具体的な型に変更

**Step 7-2-2: 明示的nullptrチェック（20件削減予定）**
- 対象: 
  - `TimePreviewLogic.cpp` (8件)
  - `TimeValidationLogic.cpp` (3件)  
  - `DateTimeInputViewImpl.cpp` (9件)
- 変更: `if (!timeProvider)` → `if (timeProvider != nullptr)`

**Step 7-2-3: const修飾子追加（8件削減予定）**
- 対象:
  - `PartialInputLogic.cpp`: `buffer`変数
  - `DateTimeInputViewImpl.cpp`: `totalWidth`変数
  - その他の局所変数

**Step 7-2-4: 重複分岐統合（5件削減予定）**
- 対象:
  - `DateTimeInputState.cpp` (4件)
  - `SettingsLogic.cpp` (1件)

**Step 7-2-5: 型変換明示化（3件削減予定）**
- 対象:
  - `SettingsDisplayViewImpl.cpp`: `size_t` → `int`変換
  - その他の型変換

**Step 7-2-6: その他（2件削減予定）**
- 対象:
  - `main.cpp`: `.cpp`ファイルのinclude
  - `DateTimeInputViewImpl.cpp`: `getCursorPixelPosition`のstatic化

#### Step 7-3: カバレッジ改善

**Step 7-3-1: カバレッジ分析**
- どのファイル/関数のカバレッジが低いかを特定
- InputDisplayState関連のカバレッジ状況を確認

**Step 7-3-2: テストケース追加**
- カバレッジが最も低い関数から順次追加
- 特に新しく分割された関数のテストを優先

#### Step 7-4: 品質ゲート通過確認

```bash
# 最終確認
pio run
pio check -e native
pio test -e native
python scripts/test_coverage.py
```

### 各ステップでの確認項目

各ステップ実行後、以下を確認します：
- ✅ ビルド確認: `pio run`
- ✅ テスト実行: `pio test -e native`
- ✅ 静的解析確認: `pio check -e native`
- ✅ 警告数の削減確認

### 期待される最終結果

- **静的解析**: 0-5件程度（現在46件から大幅改善）
- **カバレッジ**: 85%以上（現在56.0%から改善）
- **品質ゲート基準**: 完全達成

### リスク評価

- ✅ **低リスク**: 既存のテストが114件すべて通過
- ✅ **段階的修正**: 各ステップでテスト確認可能
- ✅ **機能変更なし**: 静的解析の改善のみ

## 不明点・質問

1. **現在の警告の詳細**: 現在の46件の警告の具体的な内容（どのファイルのどの行のどの種類の警告か）を確認する必要がありますか？
すでに確認いただいているとおりです。

2. **カバレッジ不足の原因**: カバレッジが56.0%と低い原因として、どのファイルや関数のカバレッジが特に低いかを特定する必要がありますか？
このスレッドでは行わないため回答せず。

3. **修正の優先順位**: 静的解析の改善とカバレッジ改善のどちらを優先すべきか、または並行して進めるべきかについて、ご指示をお願いします。
静的解析改善を優先。このスレッドではこれのみを行います。

この実行計画に基づいて、Step 7-1から開始し、段階的に品質改善を進めます。