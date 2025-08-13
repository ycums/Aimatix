# Issue Management

AimatixプロジェクトでのIssue管理ルールを定義します。

## Issue Template

### Epic Issue
大きな機能やフェーズの実装に使用
- 複数の機能を含む
- 長期間の開発が必要
- 複数の子Issueに分割可能

### Feature Issue
新機能の実装に使用
- 単一の機能実装
- 明確な完了条件
- テストケースが定義可能

### Bug Issue
バグ修正に使用
- 再現手順が明確
- 期待動作と実際の動作が定義
- 修正方針が明確

### Enhancement Issue
既存機能の改善に使用
- 現状の問題が明確
- 改善案が具体的
- 影響範囲が限定

### Refactor Issue
コードのリファクタリングに使用
- リファクタリング対象が明確
- 改善方針が具体的
- 影響範囲が明確

## Issue作成ルール

### 1. タイトル
- 簡潔で分かりやすい
- 動詞で始める（実装、修正、改善等）
- 50文字以内

### 2. 説明
- テンプレートに従って記述
- 具体的で実装可能な内容
- 完了条件を明確に

### 3. ラベル
- `epic`: 大きな機能・フェーズ
- `feature`: 新機能実装
- `bug`: バグ修正
- `enhancement`: 機能改善
- `refactor`: リファクタリング
- `ui`: UI関連
- `settings`: 設定関連
- `alarm`: アラーム関連
- `input`: 入力関連
- `test`: テスト関連

### 4. マイルストーン
- リリース予定に応じて設定
- v3.0.16: 設定画面UI実装
- v3.0.17: 設定保存・復元機能
- v3.1.0: 全機能完成

## Issue作成例

### Epic Issue作成
```bash
gh issue create --title "設定画面の項目選択・値変更UI/UX実装" \
  --body "## 概要\n設定画面で項目選択と値変更のUI/UXを実装\n\n## 背景・目的\nユーザーが設定値を直感的に変更できるUIを提供する\n\n## 含まれる機能\n- [ ] 項目選択モード実装\n- [ ] 値変更モード実装\n- [ ] モード切り替え実装\n\n## 完了条件\n- [ ] すべての子Issueが完了\n- [ ] 統合テストが通る\n- [ ] 静的解析が通る\n- [ ] カバレッジ85%以上\n\n## 技術的考慮事項\n- SettingsDisplayStateの拡張\n- 状態遷移の実装\n- UI/UXの改善\n\n## 参照\n- development_checklist.md Phase3-0-16" \
  --label "epic" \
  --label "ui" \
  --label "settings" \
  --milestone "v3.0.16"
```

### Feature Issue作成
```bash
gh issue create --title "設定画面の項目選択モード実装" \
  --body "## 概要\n設定画面で項目選択モードを実装\n\n## 要件\n- 設定項目の一覧表示\n- 項目の選択・移動\n- 選択項目のハイライト表示\n\n## 技術仕様\n- SettingsDisplayStateの拡張\n- ボタン操作の実装\n- 表示ロジックの実装\n\n## 完了条件\n- [ ] 機能実装完了\n- [ ] Unit Test追加\n- [ ] カバレッジ85%以上\n- [ ] 静的解析実行\n- [ ] 実機テスト合格\n\n## テストケース\n- [ ] 項目選択の正常系\n- [ ] 端での移動制限\n- [ ] 選択状態の表示\n\n## 参照\n- Epic Issue #123" \
  --label "feature" \
  --label "ui" \
  --label "settings"
```

### Bug Issue作成
```bash
gh issue create --title "アラーム一覧の2度目以降表示バグ修正" \
  --body "## バグの概要\nアラーム一覧が2度目以降正しく表示されない\n\n## 再現手順\n1. アラームを追加\n2. アラーム管理画面を開く\n3. メイン画面に戻る\n4. 再度アラーム管理画面を開く\n\n## 期待される動作\n常に正しくアラーム一覧が表示される\n\n## 実際の動作\nアラーム一覧に0項目表示される\n\n## 環境情報\n- PlatformIO環境: native\n- テスト環境: 実機\n- 実機環境: M5Stack Fire\n\n## 修正方針\n- AlarmDisplayStateの状態管理を修正\n- 画面遷移時の初期化処理を改善\n\n## 完了条件\n- [ ] バグ修正完了\n- [ ] 再現テスト追加\n- [ ] 回帰テスト実行\n- [ ] 静的解析実行\n\n## 参照\n- development_checklist.md Phase3-0-15" \
  --label "bug" \
  --label "alarm" \
  --label "display"
```

## Issue管理フロー

### 1. Issue作成
```bash
# 新規Issue作成
gh issue create --title "タイトル" --body "内容" --label "ラベル"

# 既存Issueの確認
gh issue list --state open

# Issue詳細の確認
gh issue view 123
```

### 2. Issue更新
```bash
# Issueの編集
gh issue edit 123 --title "新しいタイトル" --body "新しい内容"

# ラベルの追加
gh issue edit 123 --add-label "ui"

# ラベルの削除
gh issue edit 123 --remove-label "bug"
```

### 3. Issueクローズ
```bash
# Issueをクローズ
gh issue close 123 --reason "completed"

# クローズ理由
# completed: 完了
# not_planned: 計画外
# duplicate: 重複
```

## Cursor Chatでの活用

### Issue参照
```
"Issue #123の設定画面UI実装を進める。
関連するファイルを検索して、実装方針を確認して"
```

### Issue作成
```
"DateTimeInputStateの部分入力ロジックでバグが発生している。
修正が必要な箇所を特定して、テストケースも追加したい"
```

### Issue更新
```
"Issue #123の実装が完了した。
テストケースを追加して、静的解析も実行して"
```

## 品質管理

### Issue作成時のチェックリスト
- [ ] タイトルが簡潔で分かりやすい
- [ ] 説明が具体的で実装可能
- [ ] 完了条件が明確
- [ ] 適切なラベルが設定
- [ ] マイルストーンが設定
- [ ] 関連Issueがリンク

### Issue更新時のチェックリスト
- [ ] 進捗状況が正確
- [ ] 新しい情報が追加
- [ ] 完了条件が更新
- [ ] 関連Issueが更新

## 参考

- [GitHub Issues](https://docs.github.com/en/issues)
- [GitHub CLI](https://cli.github.com/)
- [Conventional Commits](./conventional_commits.md)
- [GitHub Flow](./github_flow.md) 