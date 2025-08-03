# GitHub管理移行ガイド

AimatixプロジェクトのGitHub管理移行手順書です。

## 移行完了項目

### ✅ 完了済み
- [x] Issue Template作成
  - `.github/ISSUE_TEMPLATE/epic.md`
  - `.github/ISSUE_TEMPLATE/feature.md`
  - `.github/ISSUE_TEMPLATE/bug.md`
  - `.github/ISSUE_TEMPLATE/enhancement.md`
  - `.github/ISSUE_TEMPLATE/refactor.md`

- [x] 開発ルール整備
  - `doc/guide/conventional_commits.md`
  - `doc/guide/github_flow.md`
  - `doc/guide/issue_management.md`

- [x] ドキュメント更新
  - `doc/README.md` - GitHub管理への移行説明追加

## 現在進行中タスクのIssue化

### Phase3-0-16: 設定画面の項目選択・値変更UI/UX実装

#### Epic Issue作成
```bash
gh issue create --title "設定画面の項目選択・値変更UI/UX実装" \
  --body "## 概要\n設定画面で項目選択と値変更のUI/UXを実装\n\n## 背景・目的\nユーザーが設定値を直感的に変更できるUIを提供する\n\n## 含まれる機能\n- [ ] 項目選択モードと値変更モードの切り替え\n- [ ] 設定保存/復元は未実装（UIのみ）\n- [ ] 静的解析実行（Clang-Tidy）\n\n## 完了条件\n- [ ] 機能実装完了\n- [ ] Unit Test追加\n- [ ] カバレッジ85%以上\n- [ ] 静的解析実行\n- [ ] 実機テスト合格\n\n## 技術的考慮事項\n- SettingsDisplayStateの拡張\n- 状態遷移の実装\n- UI/UXの改善\n\n## 参照\n- development_checklist.md Phase3-0-16" \
  --label "epic" \
  --label "ui" \
  --label "settings" \
  --milestone "v3.0.16"
```

### Phase3-0-17: 設定保存・復元・バリデーション

#### Epic Issue作成
```bash
gh issue create --title "設定保存・復元・バリデーション機能実装" \
  --body "## 概要\nSettingsLogic本実装＋MockEEPROMAdapter連携、設定保存/復元/バリデーションのUI・副作用コマンド\n\n## 背景・目的\n設定値の永続化とバリデーション機能を提供する\n\n## 含まれる機能\n- [ ] SettingsLogic本実装\n- [ ] MockEEPROMAdapter連携\n- [ ] 設定保存/復元機能\n- [ ] バリデーション機能\n- [ ] 静的解析実行（Clang-Tidy）\n\n## 完了条件\n- [ ] 機能実装完了\n- [ ] Unit Test追加\n- [ ] カバレッジ85%以上\n- [ ] 静的解析実行\n- [ ] 実機テスト合格\n\n## 技術的考慮事項\n- SettingsLogicの実装\n- EEPROMアダプタの連携\n- バリデーションロジック\n\n## 参照\n- development_checklist.md Phase3-0-17" \
  --label "epic" \
  --label "settings" \
  --label "storage" \
  --milestone "v3.0.17"
```

### Phase3-0-18-21: EEPROM保存値の起動時読み出し

#### Epic Issue作成
```bash
gh issue create --title "EEPROM保存値の起動時読み出し機能実装" \
  --body "## 概要\nEEPROMに保存された設定値の起動時読み出し機能を実装\n\n## 背景・目的\n設定値の永続化と起動時の復元機能を提供する\n\n## 含まれる機能\n- [ ] Set Date/Time設定の読み出し\n- [ ] LCD Brightness設定の読み出し\n- [ ] Sound設定の読み出し\n- [ ] 警告・エラー表示機能\n- [ ] 副作用コマンド拡張\n\n## 完了条件\n- [ ] 機能実装完了\n- [ ] Unit Test追加\n- [ ] カバレッジ85%以上\n- [ ] 静的解析実行\n- [ ] 実機テスト合格\n\n## 技術的考慮事項\n- EEPROMアダプタの実装\n- 起動時の初期化処理\n- エラーハンドリング\n\n## 参照\n- development_checklist.md Phase3-0-18, 3-0-19, 3-0-20, 3-0-21" \
  --label "epic" \
  --label "settings" \
  --label "storage" \
  --milestone "v3.0.17"
```

### Phase3-0-22: 日付入力画面エラー表示機能

#### Feature Issue作成
```bash
gh issue create --title "日付入力画面エラー表示機能実装" \
  --body "## 概要\n日付入力画面でのエラー表示機能を実装\n\n## 要件\n- 入力値のバリデーション\n- エラーメッセージの表示\n- ユーザーフィードバック\n\n## 技術仕様\n- DateTimeInputStateの拡張\n- エラー表示ロジックの実装\n- UI/UXの改善\n\n## 完了条件\n- [ ] 機能実装完了\n- [ ] Unit Test追加\n- [ ] カバレッジ85%以上\n- [ ] 静的解析実行\n- [ ] 実機テスト合格\n\n## テストケース\n- [ ] 正常系テスト\n- [ ] 異常系テスト\n- [ ] 境界値テスト\n\n## 参照\n- development_checklist.md Phase3-0-22" \
  --label "feature" \
  --label "ui" \
  --label "input" \
  --milestone "v3.0.17"
```

### Phase3-0-23: 全主要コンポーネント完成

#### Epic Issue作成
```bash
gh issue create --title "全主要コンポーネント完成" \
  --body "## 概要\nすべての主要コンポーネント・全仕様網羅\n\n## 背景・目的\nAimatixプロダクトの基本機能を完成させる\n\n## 含まれる機能\n- [ ] MainDisplay完成\n- [ ] InputDisplay完成\n- [ ] AlarmDisplay完成\n- [ ] SettingsDisplay完成\n- [ ] TimeLogic完成\n- [ ] AlarmLogic完成\n- [ ] InputLogic完成\n- [ ] SettingsLogic完成\n- [ ] ButtonManager完成\n- [ ] DebounceManager完成\n- [ ] 各Adapter/Hardware層完成\n- [ ] すべての画面・機能・副作用コマンドが連携\n- [ ] 全仕様を満たす\n- [ ] 静的解析実行（Clang-Tidy）\n\n## 完了条件\n- [ ] すべての機能実装完了\n- [ ] 統合テストが通る\n- [ ] 静的解析が通る\n- [ ] カバレッジ85%以上\n- [ ] 実機テスト合格\n\n## 技術的考慮事項\n- 全コンポーネントの統合\n- 仕様の完全実装\n- 品質保証\n\n## 参照\n- development_checklist.md Phase3-0-23" \
  --label "epic" \
  --label "integration" \
  --milestone "v3.1.0"
```

### Phase3-0-24-25: WiFi設定・NTP自動同期機能

#### Epic Issue作成
```bash
gh issue create --title "WiFi設定・NTP自動同期機能実装" \
  --body "## 概要\nWiFi設定機能とNTP自動同期機能を実装\n\n## 背景・目的\nネットワーク機能と時刻同期機能を提供する\n\n## 含まれる機能\n- [ ] WiFi設定機能\n  - [ ] 設定画面に「WiFi Settings」項目を追加\n  - [ ] SSID/パスワード入力画面\n  - [ ] WiFi接続テスト機能\n  - [ ] EEPROMへの設定保存\n- [ ] NTP自動同期機能\n  - [ ] 起動時の自動NTP同期\n  - [ ] 定期的な時刻同期（24時間間隔）\n  - [ ] 手動設定との優先順位管理\n  - [ ] 同期失敗時のフォールバック\n- [ ] 静的解析実行（Clang-Tidy）\n\n## 完了条件\n- [ ] 機能実装完了\n- [ ] Unit Test追加\n- [ ] カバレッジ85%以上\n- [ ] 静的解析実行\n- [ ] 実機テスト合格\n\n## 技術的考慮事項\n- WiFi接続処理\n- NTP同期処理\n- エラーハンドリング\n\n## 参照\n- development_checklist.md Phase3-0-24, 3-0-25" \
  --label "epic" \
  --label "network" \
  --label "time" \
  --milestone "v3.1.0"
```

## Issue作成後の作業

### 1. 既存ドキュメントの更新
- [ ] `doc/project/development_checklist.md`にIssue番号を追加
- [ ] 関連ドキュメントの更新

### 2. 不要ファイルの整理
- [ ] Issue化された計画ファイルの削除検討
- [ ] 参照用として残すファイルの選定

### 3. 運用開始
- [ ] Cursor ChatでのIssue参照習慣化
- [ ] Conventional Commits・GitHub Flowの運用開始

## 参考

- [Issue管理ガイド](../guide/issue_management.md)
- [Conventional Commits](../guide/conventional_commits.md)
- [GitHub Flow](../guide/github_flow.md) 