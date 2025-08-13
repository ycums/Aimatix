# Aimatix ドキュメント

## 概要

Aimatixプロジェクトのドキュメント集です。各ドキュメントは目的に応じて適切なカテゴリに分類されています。

## ドキュメント構成

### 📋 仕様書 (`spec/`)

#### [要件定義](spec/requirements.md)
- 機能要件・非機能要件
- 制約事項・成功指標
- プロジェクトの全体要件

#### [システム概要](spec/system_overview.md)
- システム構成・動作モード
- データフロー・技術仕様
- パフォーマンス仕様

#### [API仕様書](spec/api_specification.md)
- 状態遷移システムのAPI
- 型定義・主要関数
- 使用例・エラーメッセージ

### 🏗️ 設計書 (`design/`)

#### [アーキテクチャ設計](design/architecture.md)
- レイヤーアーキテクチャ
- 依存性注入パターン
- モジュール間依存関係

#### [インターフェース設計](design/interface_design.md)
- DIパターンのインターフェース
- アダプター実装
- 設計方針・拡張計画

#### [ハードウェアアダプタ設計](design/hw_adapter_design.md)
- M5Unified対応のアダプタ設計
- 色の型統一（uint16_t）
- 型キャスト回避の実装方針

### 📖 使用方法ガイド (`guide/`)

#### [開発者ガイド](guide/developer_guide.md)
- 開発環境セットアップ
- ビルド・テスト方法
- M5Unified対応の技術的注意事項
- 色の型統一とトラブルシューティング

#### [GitHub管理ガイド](guide/issue_management.md)
- Issue管理のルール
- テンプレート使用方法
- Cursor Chatでの活用方法

#### [Conventional Commits](guide/conventional_commits.md)
- コミットメッセージ形式
- 自動化方法
- 品質管理

#### [GitHub Flow](guide/github_flow.md)
- ブランチ戦略
- 開発フロー
- 品質ゲート

### ⚙️ 運用・開発ルール (`operation/`)

#### [PlatformIO運用](operation/platformio_best_practices.md)
- PlatformIO/Unity/LDF運用ルール
- ディレクトリ構成・テスト戦略
- トラブルシューティング

#### [テスト戦略](operation/testing_strategy.md)
- テスト戦略の基本方針
- テスト環境構成・実行方法
- モック戦略・品質指標

#### [静的解析ガイド](operation/clang_tidy_guide.md)
- Clang-Tidy静的解析の設定・使用方法
- 警告管理・品質向上のための活用方法
- 開発フローへの統合方法

#### [品質ゲート基準](operation/quality_gates.md)
- プロジェクトの品質基準の一元管理
- カバレッジ・静的解析・パフォーマンス基準
- 現在の達成状況と品質ゲート判定

### 📊 プロジェクト管理 (`project/`)

#### [実装計画](project/integrated_work_plan.md)
- プロジェクト全体の実装計画
- Phase別の詳細計画
- 品質保証・リスク管理

## GitHub管理への移行

### 移行状況
- [x] Issue Template作成完了
- [x] 開発ルール整備完了
- [ ] 現在進行中タスクのIssue化
- [ ] ドキュメント更新完了

### Issue管理の活用
1. **Issue作成**: 新機能・バグ修正の管理
2. **Cursor Chat連携**: Issue番号を参照した開発
3. **自動化**: ghコマンドによる効率的な操作

### 開発フローの変更点
- **従来**: ローカルドキュメント中心の管理
- **現在**: GitHub Issue中心の管理
- **Cursor Chat**: Issue参照による効率的な開発

## 技術的更新情報

### M5Unified移行完了（2025年1月）
- **色の型統一**: すべての色パラメータを`uint16_t`に統一
- **型キャスト削除**: 内部での型変換を排除し、直接的な型一致を実現
- **色定数の統一**: `AMBER_COLOR`、`TFT_BLACK`等の色定数を統一使用
- **インターフェース更新**: `IDisplay`インターフェースの色パラメータを`uint16_t`に変更
- **テスト環境対応**: 全テストファイルの色パラメータを`uint16_t`に統一

### 主要な技術的変更
1. **DisplayAdapter**: M5Unified/M5GFX APIに対応
2. **色の型**: `uint32_t` → `uint16_t`に統一
3. **型キャスト**: 内部での型変換を削除
4. **テスト環境**: Native環境でのテストが正常動作

## 使用方法

### 新規参加者の方
1. [システム概要](spec/system_overview.md)で全体を理解
2. [要件定義](spec/requirements.md)で要件を確認
3. [開発者ガイド](guide/developer_guide.md)で開発環境をセットアップ
4. [Issue管理ガイド](guide/issue_management.md)でGitHub管理を理解
5. [実装計画](project/integrated_work_plan.md)で現在の状況を確認

### 開発者の方
- **実装時**: [要件定義](spec/requirements.md) → [設計書](design/) → [開発者ガイド](guide/developer_guide.md)
- **Issue管理**: [Issue管理ガイド](guide/issue_management.md) → [GitHub Flow](guide/github_flow.md)
- **テスト時**: [テスト戦略](operation/testing_strategy.md) → [PlatformIO運用](operation/platformio_best_practices.md)
- **品質管理**: [静的解析ガイド](operation/clang_tidy_guide.md) → [カバレッジ測定ガイド](operation/coverage_measurement_guide.md)
- **トラブル時**: [開発者ガイド](guide/developer_guide.md)のトラブルシューティング

### プロジェクト管理者の方
- **全体把握**: [システム概要](spec/system_overview.md) → [実装計画](project/integrated_work_plan.md)
- **品質管理**: [品質ゲート基準](operation/quality_gates.md) → [テスト戦略](operation/testing_strategy.md) → [静的解析ガイド](operation/clang_tidy_guide.md) → [要件定義](spec/requirements.md)
- **Issue管理**: [Issue管理ガイド](guide/issue_management.md) → [GitHub Flow](guide/github_flow.md)

## ドキュメント更新方針

### 更新タイミング
- **仕様変更時**: 関連する仕様書・設計書を更新
- **実装完了時**: 開発者ガイド・テスト戦略を更新
- **運用変更時**: 運用ルールを更新
- **計画変更時**: 実装計画を更新
- **Issue化時**: 関連ドキュメントを更新

### 更新手順
1. **変更内容の確認**: どのドキュメントに影響があるか確認
2. **関連ドキュメントの更新**: 影響を受けるドキュメントを更新
3. **整合性の確認**: ドキュメント間の整合性を確認
4. **レビュー**: 必要に応じてレビューを実施

## 品質保証

### ドキュメント品質指標
- **完全性**: 必要な情報が全て記載されているか
- **正確性**: 情報が正確で最新か
- **一貫性**: ドキュメント間で一貫しているか
- **可読性**: 読みやすく理解しやすいか

### 定期レビュー
- **月次レビュー**: ドキュメントの更新状況確認
- **四半期レビュー**: ドキュメント構成の見直し
- **年次レビュー**: ドキュメント戦略の見直し

## コーディング規約

開発時のコーディング規約は `guide/developer_guide.md` を参照してください。
特に、ファイル名の命名規則（クラス名をスネークケースに変換）を遵守してください。

## 開発ガイド

- [開発者ガイド](./guide/developer_guide.md) - 開発の基本方針
- [Issue管理ガイド](./guide/issue_management.md) - GitHub Issue管理
- [Conventional Commits](./guide/conventional_commits.md) - コミットメッセージ形式
- [GitHub Flow](./guide/github_flow.md) - ブランチ戦略
- [テスト戦略](./operation/testing_strategy.md) - テストの実行方法
- [カバレッジ向上戦略](./project/coverage_strategy.md) - 効率的なカバレッジ向上の戦略

---

**作成日**: 2025年1月  
**バージョン**: 1.0.0  
**更新日**: 2025年1月 