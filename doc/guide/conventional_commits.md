# Conventional Commits

Aimatixプロジェクトでのコミットメッセージ形式を定義します。

## 基本形式

```
<type>[optional scope]: <description>

[optional body]

[optional footer(s)]
```

## Type（必須）

### feat
新機能の追加
```
feat: 設定画面の項目選択UI実装
feat(settings): 設定保存機能を追加
```

### fix
バグ修正
```
fix: アラーム一覧の表示バグを修正
fix(input): 桁送り処理の不具合を修正
```

### docs
ドキュメントのみの変更
```
docs: READMEの更新
docs: API仕様書の追加
```

### style
コードの意味に影響しない変更（空白、フォーマット等）
```
style: コードフォーマットの統一
style: インデントの修正
```

### refactor
バグ修正や機能追加ではないコードの変更
```
refactor: TimeLogicクラスの構造を改善
refactor: テストコードの整理
```

### test
テストの追加や修正
```
test: 設定画面のUnit Test追加
test: アラーム機能の統合テスト追加
```

### chore
ビルドプロセスや補助ツールの変更
```
chore: PlatformIO設定の更新
chore: 依存関係の更新
```

## Scope（オプション）

コンポーネント名を指定
- `settings`: 設定関連
- `alarm`: アラーム関連
- `input`: 入力関連
- `display`: 表示関連
- `test`: テスト関連

## Description（必須）

- 命令形で記述（現在形）
- 50文字以内
- 句読点で終わらない

## Body（オプション）

詳細な説明が必要な場合

```
feat: 設定画面の項目選択UI実装

- 項目選択モードと値変更モードの切り替え
- 設定保存/復元は未実装（UIのみ）
- 静的解析実行済み
```

## Footer（オプション）

### Breaking Changes
破壊的変更がある場合
```
feat!: 設定画面のAPIを変更

BREAKING CHANGE: SettingsDisplayStateのインターフェースが変更されました
```

### Issue References
関連Issueがある場合
```
fix: アラーム一覧の表示バグを修正

Closes #123
Fixes #124
```

## 例

### 新機能追加
```
feat(settings): 設定画面の項目選択UI実装

- 項目選択モードと値変更モードの切り替え
- 設定保存/復元は未実装（UIのみ）
- 静的解析実行済み

Closes #123
```

### バグ修正
```
fix: アラーム一覧の2度目以降表示バグ修正

- アラーム一覧が2度目以降正しく表示されない問題を修正
- 初回表示時には正しく表示される
- 削除後の選択位置が適切に調整される

Fixes #124
```

### リファクタリング
```
refactor: TimeLogicクラスの構造を改善

- メソッドの責務を明確化
- テストカバレッジを向上
- 静的解析警告を解消

Closes #125
```

## 自動化

### コミットメッセージの自動生成
```bash
# 機能追加
git commit -m "feat: $(gh issue view --json title --jq .title)"

# バグ修正
git commit -m "fix: $(gh issue view --json title --jq .title)"
```

### コミットメッセージの検証
```bash
# commitlintの設定
npm install -g @commitlint/cli @commitlint/config-conventional
```

## 参考

- [Conventional Commits](https://www.conventionalcommits.org/)
- [Angular Commit Message Guidelines](https://github.com/angular/angular/blob/main/CONTRIBUTING.md#-commit-message-guidelines) 