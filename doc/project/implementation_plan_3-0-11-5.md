# 【実装計画】3-0-11.5: 相対入力モードのバグ修正

## 概要

相対入力モードで相対時間→絶対時間の変換時に秒以下が反映されていないバグを修正します。
- **バグ**: now: 15:34:12 & input: __:05 なら expected: 15:39:12, actual: 15:39:00
- **原因**: 相対値計算時に `tm_sec = 0` で秒をリセットしている
- **修正**: 現在時刻の秒を保持するように変更

## 修正方針

### 1. 関数名の変更
- `getRelativeValue()` → `getAbsoluteValue()`
- 実際に返している値（絶対時刻）と関数名を一致させる

### 2. ロジック修正
- `alarm_tm.tm_sec = 0;` → `alarm_tm.tm_sec = tm_now->tm_sec;`
- 現在時刻の秒を保持するように修正

### 3. 設計の一貫性
- `InputDisplayState` で絶対値入力モードと同一の表示ルーチンを使用
- コードの重複を避け、保守性を向上

## 実装手順（TDDアプローチ）

### Phase 1: テスト修正・準備（30分）

#### 1.1 既存テストの関数名変更対応
- **ファイル**: `test/pure/test_input_display_pure/test_main.cpp`
- **作業内容**:
  - `test_relative_time_calculation_basic()` の関数名変更対応
  - `test_relative_input_alarm_calculation_debug()` の関数名変更対応
  - `test_relative_input_partial_alarm_calculation()` の関数名変更対応
- **確認事項**:
  - コンパイルエラーが解消されること
  - 既存のテストが通ること

#### 1.2 秒保持テストの追加
- **ファイル**: `test/pure/test_input_display_pure/test_main.cpp`
- **追加テスト**:
  ```cpp
  void test_relative_input_seconds_preservation() {
      // 現在時刻が秒を含む場合の相対値計算テスト
      // 期待値: 秒が正しく保持される
  }
  
  void test_relative_input_seconds_preservation_partial() {
      // 部分入力時の秒保持テスト
      // 期待値: 秒が正しく保持される
  }
  ```

### Phase 2: InputLogic修正（45分）

#### 2.1 関数名変更
- **ファイル**: `lib/libaimatix/src/InputLogic.h`
- **作業内容**:
  - `getRelativeValue()` → `getAbsoluteValue()` に変更
  - 関数のコメントも更新

#### 2.2 ロジック修正
- **ファイル**: `lib/libaimatix/src/InputLogic.h`
- **修正箇所**: `getAbsoluteValue()` メソッド内
- **変更内容**:
  ```cpp
  // 変更前
  alarm_tm.tm_sec = 0;
  
  // 変更後
  alarm_tm.tm_sec = tm_now->tm_sec;
  ```

#### 2.3 テスト実行・確認
- **コマンド**: `pio test -e native -f pure/test_input_display_pure -v`
- **確認事項**:
  - 新規追加した秒保持テストが通ること
  - 既存のテストが通ること
  - コンパイルエラーがないこと

### Phase 3: InputDisplayState修正（30分）

#### 3.1 関数呼び出し修正
- **ファイル**: `lib/libaimatix/src/InputDisplayState.h`
- **作業内容**:
  - `getRelativeValue()` → `getAbsoluteValue()` に変更
  - 相対値入力モードでの呼び出し箇所を修正

#### 3.2 表示ロジック統一
- **ファイル**: `lib/libaimatix/src/InputDisplayState.h`
- **作業内容**:
  - 相対値入力モードと絶対値入力モードで同一の表示ルーチンを使用
  - プレビュー表示の重複コードを削除
- **修正箇所**:
  - `onDraw()` メソッド内の相対値計算部分
  - プレビュー表示ロジックの統一

#### 3.3 テスト実行・確認
- **コマンド**: `pio test -e native -f pure/test_input_display_pure -v`
- **確認事項**:
  - 全てのテストが通ること
  - プレビュー表示が正しく動作すること

### Phase 4: 統合テスト・品質確認（45分）

#### 4.1 全テスト実行
- **コマンド**: `pio test -e native`
- **確認事項**:
  - 全てのテストが通ること
  - 他の機能に影響がないこと

#### 4.2 カバレッジ確認
- **コマンド**: `python scripts/test_coverage.py`
- **確認事項**:
  - 品質ゲート（84.3%）を上回っていること
  - 修正箇所のカバレッジが適切であること

#### 4.3 静的解析
- **コマンド**: `pio check -e native`
- **確認事項**:
  - Clang-Tidy警告数が90件以下であること
  - 新規の警告が発生していないこと

#### 4.4 ビルド確認
- **コマンド**: `pio run -e native`
- **確認事項**:
  - ビルドエラーがないこと
  - 警告が適切に処理されていること

### Phase 5: 実機テスト（30分）

#### 5.1 実機ビルド・転送
- **コマンド**: `pio run -e m5stack-fire -t upload`
- **確認事項**:
  - ビルド・転送が成功すること

#### 5.2 動作確認
- **確認項目**:
  - 相対値入力モードで秒が正しく保持されること
  - プレビュー表示が正しく動作すること
  - アラーム追加時に正しい時刻が設定されること
  - 絶対値入力モードの動作に影響がないこと

## リスク・注意事項

### 1. 関数名変更の影響
- **リスク**: 既存のテストや呼び出し箇所でコンパイルエラーが発生
- **対策**: 段階的に修正し、各段階でテストを実行

### 2. 表示ロジック統一の複雑性
- **リスク**: プレビュー表示の動作が変わってしまう
- **対策**: 既存の絶対値入力モードの表示ロジックを参考に慎重に実装

### 3. 秒保持の境界値
- **リスク**: 59秒の繰り上げ処理で問題が発生
- **対策**: 境界値テストを追加して確認

## 完了条件

### 必須条件
- [ ] 相対値入力で秒が正しく保持される
- [ ] プレビュー表示が正しく更新される
- [ ] アラーム追加時に正しい時刻が設定される
- [ ] 既存のテストが全て通る
- [ ] カバレッジが品質ゲートを上回っている
- [ ] Clang-Tidy警告数が90件以下
- [ ] 実機テストで動作確認済み

### 品質条件
- [ ] 関数名が実際の動作と一致している
- [ ] 設計の一貫性が保たれている
- [ ] コードの重複が削減されている
- [ ] 既存機能に影響がない

## 見積もり時間

- **Phase 1**: 30分（テスト修正・準備）
- **Phase 2**: 45分（InputLogic修正）
- **Phase 3**: 30分（InputDisplayState修正）
- **Phase 4**: 45分（統合テスト・品質確認）
- **Phase 5**: 30分（実機テスト）

**合計**: 3時間

## 参考資料

- [質問票](./questionnaire_3-0-11-5.md)
- [開発チェックリスト](./development_checklist.md)
- [設計ドキュメント](../design/logic_interface.md)
- [仕様書](../spec/spec.md)

---

**作成日**: 2025年1月
**バージョン**: 1.0.0
**更新日**: 2025年1月 