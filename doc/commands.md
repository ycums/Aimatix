```bash
# ビルド・アップロード
pio run -t upload # upload
pio device list # シリアルポートの確認
pio device monitor --baud 115200

# テスト実行
pio test -e native -f test_button_manager_pure #テストを実行
pio test -e native # native環境でテスト実行

# 静的解析
pio check -e native # Clang-Tidy静的解析実行
pio check -e native --severity=high # 高重要度警告のみ表示
pio check -e native --severity=high # 高重要度警告のみ表示

# カバレッジ測定
python scripts/test_coverage.py --quick # クイックカバレッジ測定
python scripts/test_coverage.py --full # 完全カバレッジ測定

# GitHub PR 安全作成（Windows/msys2 安定運用）
# 環境変数 TITLE/BASE_BRANCH を必要に応じて指定可能
# 例: TITLE="ci: update docs" BASE_BRANCH=main bash scripts/gh_pr_safe.sh ./pr_body.md
TITLE="ci: update" bash scripts/gh_pr_safe.sh # デフォルトテンプレで本文作成
```

> CI matrix test: docs-only, no labels (case A)
> CI matrix test: docs-only, no-hw + automerge (case C)
