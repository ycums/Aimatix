# カバレッジ計測システム運用ガイド

## 1. 概要

### 1.1 目的
- 個人開発プロジェクト向けのカバレッジ計測システムの運用方法を説明
- 開発中の継続的な品質監視とリリース前の品質保証
- 効率的な開発フローとの統合

### 1.2 対象読者
- **個人開発者**: 日常的な開発と品質管理
- **品質保証**: リリース前の品質確認
- **運用担当者**: システムメンテナンス（将来的）

## 2. 個人開発向け基本運用

### 2.1 開発フロー統合

#### 2.1.1 開発中の継続実行
```bash
# 開発中の軽量カバレッジ計測（推奨）
python scripts/test_coverage.py --quick

# 機能完成時の詳細計測
python scripts/test_coverage.py --full

# リリース前の完全計測
python scripts/test_coverage.py --release
```

#### 2.1.2 開発サイクル統合
```bash
# 1. 機能開発開始時
python scripts/test_coverage.py --baseline
pio check -e native  # 静的解析実行

# 2. 開発中の継続監視
python scripts/test_coverage.py --quick --watch
pio check -e native  # 静的解析実行

# 3. 機能完成時の品質確認
python scripts/test_coverage.py --full
pio check -e native  # 静的解析実行

# 4. リリース前の最終確認
python scripts/test_coverage.py --release --strict
pio check -e native  # 静的解析実行
```

### 2.2 実行モード詳細

#### 2.2.1 クイックモード（開発中）
```bash
# 軽量で高速な実行
python scripts/test_coverage.py --quick

# 特徴
# - 純粋ロジックのみ計測
# - 基本的なレポート生成
# - 実行時間: 1-2分
# - メモリ使用量: 最小限
```

#### 2.2.2 フルモード（機能完成時）
```bash
# 完全なカバレッジ計測
python scripts/test_coverage.py --full

# 特徴
# - 全環境のカバレッジ計測
# - 詳細なレポート生成
# - 品質ゲート判定
# - 実行時間: 5-10分
```

#### 2.2.3 リリースモード（リリース前）
```bash
# リリース品質の確認
python scripts/test_coverage.py --release

# 特徴
# - 厳格な品質ゲート
# - 履歴データの記録
# - 完全なレポート生成
# - 実行時間: 10-15分
```

### 2.3 自動化設定

#### 2.3.1 開発環境での自動実行
```bash
# ファイル変更監視（開発中）
fswatch -o . | xargs -n1 -I{} python scripts/test_coverage.py --quick
fswatch -o . | xargs -n1 -I{} pio check -e native  # 静的解析実行

# 定期実行（日次）
crontab -e
# 毎日午前9時に実行
0 9 * * * cd /path/to/project && python scripts/test_coverage.py --full && pio check -e native

# 週次実行（週末）
0 18 * * 6 cd /path/to/project && python scripts/test_coverage.py --release && pio check -e native
```

#### 2.3.2 IDE統合
```json
// .vscode/tasks.json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Quick Coverage",
            "type": "shell",
            "command": "python",
            "args": ["scripts/test_coverage.py", "--quick"],
            "group": "test",
            "presentation": {
                "echo": true,
                "reveal": "always",
                "focus": false,
                "panel": "shared"
            }
        },
        {
            "label": "Full Coverage",
            "type": "shell",
            "command": "python",
            "args": ["scripts/test_coverage.py", "--full"],
            "group": "test",
            "presentation": {
                "echo": true,
                "reveal": "always",
                "focus": false,
                "panel": "shared"
            }
        }
    ]
}
```

## 3. 環境準備

### 3.1 必要なツール
```bash
# Python環境
python --version  # 3.8以上

# PlatformIO
pio --version

# gcov/gcovr
gcovr --version

# 必要なPythonパッケージ
pip install platformio gcovr

# 開発用ツール（オプション）
pip install fswatch  # ファイル変更監視
```

### 3.2 設定ファイル確認
```bash
# 設定ファイルの存在確認
ls -la coverage_config.json

# 設定内容確認
cat coverage_config.json

# 設定ファイルの生成（初回のみ）
python scripts/test_coverage.py --init-config
```

## 4. 開発フロー別実行パターン

### 4.1 日常開発フロー

#### 4.1.1 開発開始時
```bash
# 1. ベースライン計測
python scripts/test_coverage.py --baseline

# 2. 開発環境確認
python scripts/test_coverage.py --check-env
```

#### 4.1.2 開発中
```bash
# 1. 軽量カバレッジ計測（頻繁に実行）
python scripts/test_coverage.py --quick

# 2. 特定機能のカバレッジ確認
python scripts/test_coverage.py --environment native --filter "time_logic"

# 3. 変更ファイルのみ計測
python scripts/test_coverage.py --changed-files
```

#### 4.1.3 機能完成時
```bash
# 1. 完全カバレッジ計測
python scripts/test_coverage.py --full

# 2. 品質ゲート確認
python scripts/test_coverage.py --quality-gate-only

# 3. レポート生成
python scripts/test_coverage.py --report-only
```

### 4.2 リリース準備フロー

#### 4.2.1 リリース前確認
```bash
# 1. リリース品質チェック
python scripts/test_coverage.py --release

# 2. 履歴データ確認
cat coverage_history.json | tail -5

# 3. 品質トレンド確認
python scripts/test_coverage.py --trend-analysis
```

#### 4.2.2 リリース後
```bash
# 1. リリース記録
python scripts/test_coverage.py --record-release

# 2. ベースライン更新
python scripts/test_coverage.py --update-baseline
```

## 5. 結果確認と活用

### 5.1 レポート確認

#### 5.1.1 開発中の確認
```bash
# クイックレポート確認
open coverage_reports/quick_coverage_report.html

# コンソール出力確認
python scripts/test_coverage.py --quick --console-only
```

#### 5.1.2 リリース前の確認
```bash
# 完全レポート確認
open coverage_reports/coverage_report_YYYYMMDD_HHMMSS.html

# 品質ゲート結果確認
cat coverage_reports/quality_gate_result.json

# 履歴確認
cat coverage_history.json | tail -1
```

### 5.2 品質監視

#### 5.2.1 カバレッジ推移監視
```bash
# 週次推移確認
python scripts/test_coverage.py --trend-weekly

# 月次推移確認
python scripts/test_coverage.py --trend-monthly

# 品質ゲート履歴確認
python scripts/test_coverage.py --quality-history
```

#### 5.2.2 未カバー領域の特定
```bash
# 未カバー領域の詳細確認
python scripts/test_coverage.py --uncovered-details

# 優先度の高い未カバー領域
python scripts/test_coverage.py --uncovered-priority
```

## 6. トラブルシューティング

### 6.1 よくある問題

#### 6.1.1 実行時間が長い
```bash
# クイックモードを使用
python scripts/test_coverage.py --quick

# 特定環境のみ実行
python scripts/test_coverage.py --environment native

# 並列実行の無効化
python scripts/test_coverage.py --no-parallel
```

#### 6.1.2 メモリ不足
```bash
# メモリ制限設定
export COVERAGE_MEMORY_LIMIT=512MB

# 軽量モード実行
python scripts/test_coverage.py --lightweight
```

#### 6.1.3 設定ファイルエラー
```bash
# 設定ファイル検証
python scripts/test_coverage.py --validate-config

# デフォルト設定で実行
python scripts/test_coverage.py --use-defaults
```

### 6.2 開発環境固有の問題

#### 6.2.1 Windows環境
```bash
# Windows固有の設定
set COVERAGE_CACHE_DIR=%TEMP%\coverage_cache

# PowerShellでの実行
powershell -Command "python scripts/test_coverage.py --quick"
```

#### 6.2.2 macOS環境
```bash
# macOS固有の設定
export COVERAGE_CACHE_DIR=/tmp/coverage_cache

# fswatchの使用
fswatch -o . | xargs -n1 -I{} python scripts/test_coverage.py --quick
```

## 7. 最適化

### 7.1 開発効率の最適化

#### 7.1.1 実行時間の最適化
```json
{
  "performance": {
    "quick_mode": {
      "enabled": true,
      "environments": ["native"],
      "timeout": 120
    },
    "full_mode": {
      "enabled": true,
      "environments": ["native", "unit-test-esp32"],
      "timeout": 600
    }
  }
}
```

#### 7.1.2 メモリ使用量の最適化
```json
{
  "performance": {
    "memory_limit": "512MB",
    "cache_enabled": true,
    "cache_size": "100MB"
  }
}
```

### 7.2 品質管理の最適化

#### 7.2.1 閾値の段階的設定
```json
{
  "quality_gate": {
    "development": {
      "threshold": 70.0,
      "strict_mode": false
    },
    "release": {
      "threshold": 80.0,
      "strict_mode": true
    }
  }
}
```

## 8. 将来の拡張

### 8.1 リモートリポジトリ統合（将来的）

#### 8.1.1 GitHub統合
```yaml
# .github/workflows/coverage.yml（将来的）
name: Coverage Measurement
on: [push]

jobs:
  coverage:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Run coverage measurement
        run: python scripts/test_coverage.py --release
```

#### 8.1.2 履歴管理
```bash
# リモート履歴の同期
python scripts/test_coverage.py --sync-history

# リモートレポートのアップロード
python scripts/test_coverage.py --upload-reports
```

### 8.2 チーム開発対応（将来的）

#### 8.2.1 プルリクエスト統合
```yaml
# PR時の自動実行
on: [pull_request]
jobs:
  coverage:
    steps:
      - name: Coverage check
        run: python scripts/test_coverage.py --pr-check
```

## 9. 参考資料

### 9.1 関連ドキュメント
- [カバレッジ計測システム設計書](../design/coverage_measurement_system.md)
- [カバレッジ計測システム技術仕様書](../spec/coverage_measurement_spec.md)
- [テスト戦略](../test_strategy/README.md)

### 9.2 外部リソース
- [gcov公式ドキュメント](https://gcc.gnu.org/onlinedocs/gcc/Gcov.html)
- [gcovr公式ドキュメント](https://gcovr.com/)
- [PlatformIOテストドキュメント](https://docs.platformio.org/en/latest/advanced/unit-testing/index.html)

### 9.3 サポート
- **技術サポート**: 開発チーム
- **ドキュメント**: プロジェクトWiki

## 10. AI Agent開発フロー遵守ガイド

### 10.1 AI Agent必須遵守事項

#### 10.1.1 コード変更時の品質保証フロー
```bash
# 変更前の現状把握（必須）
python scripts/test_coverage.py --quick

# 変更中の段階的品質確認
python scripts/test_coverage.py --environment native --filter "変更モジュール名"

# 変更後の完全品質確認（必須）
python scripts/test_coverage.py --full
```

#### 10.1.2 開発フェーズ別必須実行コマンド
```bash
# 開発開始時（必須）
python scripts/test_coverage.py --baseline
python scripts/test_coverage.py --check-env

# 開発中（頻繁に実行）
python scripts/test_coverage.py --quick
python scripts/test_coverage.py --changed-files

# 機能完成時（必須）
python scripts/test_coverage.py --full
python scripts/test_coverage.py --quality-gate-only

# リリース前（必須）
python scripts/test_coverage.py --release --strict
python scripts/test_coverage.py --trend-analysis
```

#### 10.1.3 品質ゲートチェック基準
- **開発中**: 70%カバレッジ未達 → 警告表示
- **機能完成時**: 80%カバレッジ未達 → 実装継続不可
- **リリース前**: 85%カバレッジ未達 → リリース不可

### 10.2 AI Agent品質保証ワークフロー

#### 10.2.1 新機能実装時の品質保証
1. **設計段階**: テスト戦略策定、カバレッジ目標設定
2. **実装段階**: 段階的テスト実行、品質維持
3. **完成段階**: 完全カバレッジ計測、品質ゲート確認
4. **統合段階**: 統合テスト実行、回帰テスト確認

#### 10.2.2 バグ修正時の品質保証
1. **問題特定**: カバレッジ計測で問題領域特定
2. **修正実装**: 修正内容のテストケース追加
3. **修正確認**: 修正箇所のカバレッジ確認
4. **回帰テスト**: 既存機能への影響確認

#### 10.2.3 リファクタリング時の品質保証
1. **事前計測**: 変更前のカバレッジ計測実行
2. **段階的変更**: 小さな変更単位でのテスト実行
3. **動作確認**: 既存テストの動作確認
4. **最終確認**: 完全なカバレッジ計測実行

### 10.3 AI Agent品質保証チェックリスト

#### 10.3.1 日常開発品質チェックリスト
- [ ] 開発開始時にベースライン計測実行
- [ ] コード変更後にクイックカバレッジ計測実行
- [ ] 新機能追加時にテストケース追加
- [ ] 機能完成時に完全カバレッジ計測実行
- [ ] 品質ゲート通過確認

#### 10.3.2 リリース準備品質チェックリスト
- [ ] リリース品質カバレッジ計測実行
- [ ] 品質ゲート厳格モード通過確認
- [ ] 履歴データ記録確認
- [ ] 品質トレンド分析確認
- [ ] リリース記録作成

#### 10.3.3 トラブルシューティング品質チェックリスト
- [ ] カバレッジ計測失敗時の原因特定
- [ ] 品質ゲート未達時の改善計画策定
- [ ] パフォーマンス問題時の最適化実行
- [ ] 環境固有問題の解決確認

### 10.4 AI Agent自動品質保証ルール

#### 10.4.1 自動実行ルール
- **コード変更検知時**: 自動的にクイックカバレッジ計測実行
- **コミット前**: 品質ゲートチェック実行
- **プルリクエスト時**: 完全カバレッジ計測実行
- **リリース前**: 厳格品質ゲートチェック実行

#### 10.4.2 品質保証ルール
- **カバレッジ低下**: 即座に警告表示、改善計画策定
- **品質ゲート未達**: 実装継続不可、改善必須
- **テスト失敗**: 即座に修正、再テスト実行
- **パフォーマンス劣化**: 最適化実行、基準回復確認

#### 10.4.3 ドキュメント更新ルール
- **設計変更時**: 関連ドキュメントの更新
- **品質基準変更時**: ガイドラインの更新
- **新機能追加時**: 運用ガイドの更新
- **問題解決時**: トラブルシューティングガイドの更新 