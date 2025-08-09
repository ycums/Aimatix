# GitHub Flow

AimatixプロジェクトでのGitHub Flowの運用ルールを定義します。

## 概要

GitHub Flowは、シンプルで効率的なブランチ戦略です。mainブランチを常にデプロイ可能な状態に保ち、機能開発はブランチで行います。

## ブランチ戦略

### mainブランチ
- 常にデプロイ可能な状態を維持
- 直接コミットは禁止
- すべての変更はPull Request経由

### 機能ブランチ
- 機能単位でブランチを作成
- 命名規則: `feature/issue-123-description`
- 例: `feature/issue-16-settings-ui`

### バグ修正ブランチ
- バグ修正用ブランチ
- 命名規則: `fix/issue-123-description`
- 例: `fix/issue-15-alarm-display-bug`

### リファクタリングブランチ
- リファクタリング用ブランチ
- 命名規則: `refactor/issue-123-description`
- 例: `refactor/issue-14-time-logic-improvement`

## 開発フロー

### 1. Issue作成
```bash
# 新機能のIssue作成
gh issue create --title "設定画面の項目選択UI実装" \
  --body "## 概要\n設定画面で項目選択と値変更のUI/UXを実装\n\n## 要件\n- 項目選択モードと値変更モードの切り替え\n- 設定保存/復元は未実装（UIのみ）" \
  --label "enhancement" \
  --label "ui"

# バグ修正のIssue作成
gh issue create --title "アラーム一覧の2度目以降表示バグ修正" \
  --body "## バグ内容\nアラーム一覧が2度目以降正しく表示されない\n\n## 再現手順\n1. アラームを追加\n2. アラーム管理画面を開く\n3. メイン画面に戻る\n4. 再度アラーム管理画面を開く" \
  --label "bug"
```

### 2. ブランチ作成
```bash
# Issue番号を取得
ISSUE_NUMBER=$(gh issue list --json number,title --jq '.[0].number')

# ブランチを作成
git checkout -b feature/issue-$ISSUE_NUMBER-settings-ui

# または、ghコマンドでブランチ作成
gh issue view $ISSUE_NUMBER --json title --jq '.title' | xargs -I {} git checkout -b "feature/issue-$ISSUE_NUMBER-{}"
```

### 3. 開発・コミット
```bash
# 開発作業
# ...

# Conventional Commitsでコミット
git add .
git commit -m "feat(settings): 設定画面の項目選択UI実装

- 項目選択モードと値変更モードの切り替え
- 設定保存/復元は未実装（UIのみ）
- 静的解析実行済み

Closes #$ISSUE_NUMBER"
```

### 4. プッシュ・Pull Request作成
```bash
# ブランチをプッシュ
git push origin feature/issue-$ISSUE_NUMBER-settings-ui

# Pull Request作成（安全テンプレ via scripts/gh_pr_safe.sh）
TITLE="feat: 設定画面の項目選択UI実装" \
BASE_BRANCH="main" \
BODY_SOURCE="./.github/pr_bodies/feat_settings_ui.md" \
bash scripts/gh_pr_safe.sh
```

### 5. レビュー・マージ
```bash
# レビュー依頼
gh pr review --approve

# マージ（レビュー完了後）
gh pr merge --squash
```

### 6. ブランチ削除
```bash
# ローカルブランチ削除
git checkout main
git pull origin main
git branch -d feature/issue-$ISSUE_NUMBER-settings-ui

# リモートブランチ削除
git push origin --delete feature/issue-$ISSUE_NUMBER-settings-ui
```

## 自動化

### IssueとPRの自動リンク
```bash
# PR作成時にIssueを自動リンク（本文はファイルで用意）
ISSUE_TITLE="$(gh issue view --json title --jq .title)"
ISSUE_NUMBER="$(gh issue view --json number --jq .number)"
BODY_FILE="$(mktemp -t gh-pr-body.XXXXXX || echo ./pr_body.md)"
cat > "$BODY_FILE" << EOF
Closes #$ISSUE_NUMBER
EOF
TITLE="feat: $ISSUE_TITLE" BASE_BRANCH="main" BODY_SOURCE="$BODY_FILE" bash scripts/gh_pr_safe.sh
```

### ブランチ名の自動生成
```bash
# Issue番号とタイトルからブランチ名を生成
ISSUE_NUMBER=$(gh issue view --json number --jq .number)
ISSUE_TITLE=$(gh issue view --json title --jq .title | sed 's/[^a-zA-Z0-9]/-/g')
BRANCH_NAME="feature/issue-$ISSUE_NUMBER-$ISSUE_TITLE"
git checkout -b "$BRANCH_NAME"
```

## 品質ゲート

### Pull Request作成前のチェック
```bash
# テスト実行
pio test -e native

# 静的解析実行
pio check

# カバレッジ確認
python scripts/test_coverage.py
```

### マージ前のチェック
- [ ] レビュー完了
- [ ] テスト通過
- [ ] 静的解析通過
- [ ] カバレッジ85%以上
- [ ] 実機テスト合格

## 緊急修正

### ホットフィックス
```bash
# 緊急修正用ブランチ
git checkout -b hotfix/critical-bug-fix

# 修正・コミット
git add .
git commit -m "fix: 緊急修正の説明"

# プッシュ・PR作成
git push origin hotfix/critical-bug-fix
TITLE="fix: 緊急修正" \
BODY_SOURCE="./.github/pr_bodies/hotfix_template.md" \
bash scripts/gh_pr_safe.sh
```

## 参考

- [GitHub Flow](https://guides.github.com/introduction/flow/)
- [GitHub CLI](https://cli.github.com/)
- [Conventional Commits](./conventional_commits.md) 