# Aimatix プロダクト開発チェックリスト

本ドキュメントは、`doc/spec/`・`doc/design/`を参照し、Aimatix要件を満たすプロダクトを開発するための実施手順書です。
各タスクには進捗管理用のチェックボックスと、参照・指示しやすいタスク番号（${phase番号}-${task番号}-${sub-task番号}）を付与しています。

---

## Phase1: 要件・設計準備

### 1-1: 要件・仕様理解
- [x] 1-1-1: 要件定義書（`spec/requirements.md`）を精読し、疑問点を洗い出す
- [x] 1-1-2: システム概要（`spec/system_overview.md`）・API仕様（`spec/api_specification.md`）を確認
- [x] 1-1-3: カバレッジ・テスト戦略（`operation/testing_strategy.md`等）を確認

### 1-2: 設計方針理解
- [x] 1-2-1: アーキテクチャ設計（`design/architecture.md`）のレイヤ構造・責務分離を理解
- [x] 1-2-2: インターフェース設計（`design/interface_design.md`）のDI/アダプタ方針を理解
- [x] 1-2-3: ドキュメント間の整合性を確認

---

## Phase2: 設計・プロトタイピング

### 2-1: ロジック・アダプタ設計
- [x] 2-1-1: 純粋ロジック（TimeLogic, AlarmLogic, InputLogic, SettingsLogic等）のインターフェース設計
- [x] 2-1-2: ハードウェアアダプタ（M5StackEEPROMAdapter等）の設計
- [x] 2-1-3: UIレイヤの画面遷移・状態管理設計

### 2-2: 状態遷移・API設計
- [x] 2-2-1: 状態遷移API（`spec/api_specification.md`）の設計・型定義
- [x] 2-2-2: Transition/Command/Event設計方針の明確化

### 2-3: モック・テスト設計
- [x] 2-3-1: モック実装方針の決定（`test/mock/`の活用）
- [x] 2-3-2: テストケース設計（Unit/Integration）

---

## Phase3: 段階的マイルストーン開発・動作/品質チェック

### 3-0: 機能・UI積み上げ型マイルストーン（背骨構築＋段階的拡張）
- [x] 3-0-1: 実機で「でたらめな」メイン画面が表示される（MainDisplay雛形＋TimeLogic stub）
    - [x] 実機転送・画面表示確認
    - [x] 最低限のビルド・テスト・カバレッジ確認
    - [x] 静的解析実行（Clang-Tidy）
- [x] 3-0-2: メイン画面の体裁が整う（MainDisplay UIレイアウト・描画拡張、TimeLogic「現在時刻」仮実装）
    - [x] 実機での見た目確認
- [x] 3-0-3: 決め打ちで2分カウントダウンが走る（TimeLogic/AlarmLogic stub/仮実装、コマンド/イベント駆動の背骨完成）
    - [x] カウントダウン動作確認
    - [x] テスト・カバレッジ確認
    - [x] 静的解析実行（Clang-Tidy）
- [x] 3-0-4: 入力画面（InputDisplay）雛形追加・画面遷移（InputLogic stub、状態遷移/nextMode）
    - [x] StateManager（状態遷移管理クラス）雛形追加・画面/ロジック状態の一元管理
    - [x] MainDisplay, InputDisplay, InputLogic等をStateManagerに登録し、状態遷移APIで画面遷移を制御
    - [x] ボタン操作イベントをStateManager経由で処理し、画面遷移・状態遷移のテスト容易性を確保
- [x] 3-0-5: 入力画面からC長押しでメイン画面に戻れる（状態遷移・画面遷移の双方向確認）
- [x] 3-0-6: 入力画面に初期値__:__が表示される（InputLogicの初期値管理、InputDisplayで値表示）
- [x] 3-0-7: 入力画面で+1/+5ができる（InputLogicの+1/+5実装、ユーザー入力反映の背骨完成）
    - [x] ButtonManagerがDebounceManagerを内部で利用し、物理ボタン入力のデバウンス処理を行う
    - [x] コマンド/イベント駆動で「ユーザー入力→状態変化→UI反映」仕組みを整備
- [x] 3-0-8: 入力画面C短押しでアラーム追加（AlarmLogicのaddAlarm実装、内部状態コミット・画面反映の背骨完成）
    - [x] アラーム追加時の不正値（例: 99:99, 15:99, 9:90 など）はspec.md準拠で「最も近い未来の正しい時刻」に補正して追加すること。
- [x] 3-0-9: 入力画面で桁送りができる（InputLogic/UIの編集操作拡張）
    - [x] InputLogicにshiftDigits()メソッドを追加（純粋ロジック）
    - [x] 桁送り方向: 右から左（3→2→1→0）
    - [x] 全桁入力済み時は拒絶
    - [x] 失敗時は何もしない（音・視覚的フィードバックなし）
    - [x] InputDisplayStateのonButtonB()で桁送り処理を実装
    - [x] ボタンヒント: A: "INC" B: "NEXT" C: "SET"
    - [x] 桁送り機能のUnit Testを追加
    - [x] 境界値テスト（全桁入力済み、部分入力等）
    - [x] カバレッジ品質ゲートを上回る（84.3%）
    - [x] 静的解析実行（Clang-Tidy）
    - [x] 実機テスト合格（入力と同時に桁送り問題解決）
- [x] 3-0-10: 相対値入力ができる
  - [x] 相対値入力画面に移行できる
  - [x] 相対値計算が正しくできる
  - [x] アラームが設定できる
  - [x] 静的解析実行（Clang-Tidy）
- [ ] 3-0-11: アラーム管理画面（AlarmDisplay）雛形追加・遷移（AlarmLogic連携、リスト表示・選択）
  - [x] メイン画面C短押しでアラーム管理画面に遷移できる
  - [x] アラームリストが正しく表示される（時刻順）
  - [x] アラームが存在しない場合は"NO ALARMS"が表示される
  - [x] A/Bボタンで選択位置を上下移動できる（端で停止）
  - [x] A/B長押しで選択位置を一番上/下に移動できる
  - [x] C短押しで選択中のアラームを削除できる
  - [x] C長押しでメイン画面に戻れる
  - [x] 削除後の選択位置が適切に調整される
  - [x] 最後のアラーム削除後は"NO ALARMS"が表示される
  - [ ] カバレッジが品質ゲートを上回っている
  - [ ] 静的解析実行（Clang-Tidy）
- [ ] 3-0-11.5: 相対入力モードのバグ修正
  - [ ] 相対時間→絶対時間の変換時に秒以下が反映されていない。
    - [ ] now: 15:34:12 & input: __:05 なら expected: 15:39:12
      - [ ] actual: 15:39:00
- [ ] 3-0-12: 設定画面（SettingsDisplay）雛形追加・遷移（SettingsLogic stub、設定値の表示・編集雛形）
  - [ ] 静的解析実行（Clang-Tidy）
- [ ] 3-0-13: 項目選択・アラーム削除（AlarmLogic/AlarmDisplayの機能拡張、副作用コマンド一元管理）
  - [ ] 静的解析実行（Clang-Tidy）
- [ ] 3-0-14: 設定保存・復元・バリデーション（SettingsLogic本実装＋MockEEPROMAdapter連携、設定保存/復元/バリデーションのUI・副作用コマンド）
  - [ ] 静的解析実行（Clang-Tidy）
- [ ] 3-0-15: 警告・エラー表示、副作用コマンド拡張（ShowWarningコマンド、エラー時のUI反映）
  - [ ] 入力画面の桁送り失敗時に何らかのリアクションを追加する。
  - [ ] 静的解析実行（Clang-Tidy）
- [ ] 3-0-16: すべての主要コンポーネント・全仕様網羅（MainDisplay, InputDisplay, AlarmDisplay, SettingsDisplay, TimeLogic, AlarmLogic, InputLogic, SettingsLogic, ButtonManager, DebounceManager, 各Adapter/Hardware層）
    - [ ] すべての画面・機能・副作用コマンドが連携し、全仕様を満たす
    - [ ] 静的解析実行（Clang-Tidy）

---

#### 主要コンポーネントの導入・完成タイミング

| コンポーネント         | 初登場         | 完成           |
|----------------------|---------------|---------------|
| MainDisplay          | 3-0-1         | 3-0-15        |
| InputDisplay         | 3-0-4         | 3-0-15        |
| AlarmDisplay         | 3-0-8/10      | 3-0-15        |
| SettingsDisplay      | 3-0-11        | 3-0-15        |
| TimeLogic            | 3-0-1/3       | 3-1-1/3-0-15  |
| AlarmLogic           | 3-0-3/8       | 3-1-1/3-0-15  |
| InputLogic           | 3-0-4/7       | 3-1-1/3-0-15  |
| SettingsLogic        | 3-0-11/13     | 3-1-1/3-0-15  |
| ButtonManager        | 3-0-1         | 3-1-1/3-0-15  |
| DebounceManager      | 3-0-1         | 3-1-1/3-0-15  |
| 各Adapter/Hardware層 | 3-0-1         | 3-1-2/3-0-15  |

---

### 3-1: ロジック・アダプタ最終実装・結合
- [ ] 3-1-1: 純粋ロジック（lib/libaimatix/src/）の本実装（`doc/design/logic_interface.md`に基づく）
    - [ ] ButtonManagerがDebounceManagerを内部で利用し、ボタン入力のデバウンス処理を実装
- [ ] 3-1-2: ハードウェアアダプタ（src/）の本実装（`doc/design/hw_adapter_design.md`に基づく）
- [ ] 3-1-3: DI/アダプタの結合テスト（本番/モック切替の動作確認）

### 3-2: 状態遷移・UI最終実装（コマンド/イベント駆動設計）
- [ ] 3-2-1: 状態遷移システム（src/state_transition/）の本実装（`doc/spec/api_specification.md`・`doc/design/ui_state_management.md`参照）
- [ ] 3-2-2: UIレイヤ（src/ui.cpp等）の本実装（コマンド/イベント駆動で副作用一元管理）
- [ ] 3-2-3: 画面遷移・エラー表示・警告表示の本実装（状態遷移テーブル・コマンド/イベント分岐に基づく）

---

## Phase4: テスト・品質保証

### 4-1: テスト作成・実行
- [ ] 4-1-1: Unit Test作成（test/pure/、native環境、`doc/design/test_case_policy.md`参照）
- [ ] 4-1-2: Integration Test作成（test/integration/等、コマンド/イベント副作用の検証）
- [ ] 4-1-3: 実機テスト（M5Stack Fire、UI/副作用の一貫性確認）

### 4-2: カバレッジ・品質管理
- [ ] 4-2-1: カバレッジ計測（scripts/test_coverage.py等、`doc/spec/coverage_measurement_spec.md`参照）
- [ ] 4-2-2: 静的解析実行（Clang-Tidy、`doc/operation/clang_tidy_guide.md`参照）
- [ ] 4-2-3: 品質指標（要件定義5.2, 5.3）充足の確認
- [ ] 4-2-4: 成果物レビュー・要件充足確認

---

## Phase5: ドキュメント・運用

### 5-1: ドキュメント整備
- [ ] 5-1-1: 開発者ガイド・運用手順（guide/developer_guide.md等）の整備
- [ ] 5-1-2: コーディング規約・運用ルールの明文化

### 5-2: プロジェクト管理
- [ ] 5-2-1: 進捗レビュー・タスク管理
- [ ] 5-2-2: ドキュメント・成果物の最終レビュー

---

**作成日**: 2025年1月
**バージョン**: 1.0.0
**更新日**: 2025年1月 