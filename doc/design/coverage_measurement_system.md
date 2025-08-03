# カバレッジ計測システム設計書

## 1. 概要

### 1.1 目的
- DIパターンに対応した新しいカバレッジ計測システムの構築
- 純粋ロジックとM5Stack依存実装の分離計測
- 統合的なカバレッジレポートの生成
- 品質ゲート基準の監視と品質保証

詳細な品質ゲート基準については [品質ゲート基準](../operation/quality_gates.md) を参照してください。

### 1.2 背景
- DIパターン導入により以前のカバレッジ計測仕組みが破壊
- 新しいアーキテクチャに対応したカバレッジ計測が必要
- 品質保証の基盤としてカバレッジ計測の復旧が重要

## 2. システムアーキテクチャ

### 2.1 全体構成

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Pure Logic    │    │ M5Stack Dep.    │    │   Integration   │
│   Coverage      │    │   Coverage      │    │    Coverage     │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         └───────────────────────┼───────────────────────┘
                                 │
                    ┌─────────────────┐
                    │ Coverage Report │
                    │   Generator     │
                    └─────────────────┘
                                 │
                    ┌─────────────────┐
                    │ Quality Gate    │
                    │   Monitor       │
                    └─────────────────┘
```

### 2.2 レイヤー分離

#### 2.2.1 純粋ロジックレイヤー
- **対象**: `lib/libaimatix/src/`
- **環境**: native環境
- **テスト**: `test/pure/`
- **カバレッジ**: gcov/gcovr
- **実行**: `pio test -e native --coverage`

#### 2.2.2 M5Stack依存レイヤー
- **対象**: `src/`
- **環境**: unit-test-esp32環境
- **テスト**: `test/integration/`
- **カバレッジ**: gcov/gcovr
- **実行**: `pio test -e unit-test-esp32 --coverage`

#### 2.2.3 統合レイヤー
- **対象**: 全体システム
- **環境**: native環境
- **テスト**: 純粋ロジックテスト
- **カバレッジ**: コードカバレッジ（自動計測）
- **方法**: gcov/gcovrによる自動計測
- **実行**: `pio test -e native`

### 2.3 データフロー

```
1. 純粋ロジックテスト実行
   ↓
2. gcovデータ生成 (.gcda/.gcno)
   ↓
3. M5Stack依存テスト実行
   ↓
4. gcovデータ生成 (.gcda/.gcno)
   ↓
5. 統合テスト実行
   ↓
6. カバレッジデータ統合
   ↓
7. レポート生成 (HTML/XML/JSON)
   ↓
8. 品質ゲート判定
   ↓
9. 結果通知
```

## 3. 技術実装

### 3.1 カバレッジ計測ツール

#### 3.1.1 gcov/gcovr
- **用途**: コードカバレッジ計測
- **出力**: .gcda/.gcnoファイル
- **レポート**: HTML/XML形式
- **統合**: PlatformIOとの自動統合

#### 3.1.2 PlatformIO統合
- **環境**: native, unit-test-esp32
- **設定**: platformio.ini
- **実行**: `pio test --coverage`
- **出力**: 自動的にHTMLレポート生成

### 3.2 カバレッジ計測スクリプト

#### 3.2.1 メインスクリプト
```python
# scripts/test_coverage.py
import subprocess
import json
import os
from datetime import datetime

class CoverageMeasurementSystem:
    def __init__(self, config_path: str = "coverage_config.json"):
        """
        初期化
        Args:
            config_path: 設定ファイルパス
        """
        self.config = self.load_config(config_path)
        self.pure_coverage = None
        self.m5stack_coverage = None
        self.integration_coverage = None
        
    def measure_pure_logic_coverage(self) -> dict:
        """
        純粋ロジックのカバレッジ計測
        Returns:
            カバレッジデータ
        """
        try:
            # PlatformIOコマンド実行
            result = subprocess.run([
                'pio', 'test', '-e', 'native', '--coverage'
            ], capture_output=True, text=True, check=True)
            
            # gcovデータ解析
            coverage_data = self.parse_gcov_data('native')
            
            return {
                'environment': 'native',
                'coverage_percentage': coverage_data['total_coverage'],
                'line_coverage': coverage_data['line_coverage'],
                'branch_coverage': coverage_data['branch_coverage'],
                'function_coverage': coverage_data['function_coverage'],
                'files': coverage_data['files']
            }
        except subprocess.CalledProcessError as e:
            raise CoverageMeasurementError(f"純粋ロジックカバレッジ計測失敗: {e}")
        
    def measure_m5stack_coverage(self) -> dict:
        """
        M5Stack依存実装のカバレッジ計測
        Returns:
            カバレッジデータ
        """
        try:
            # PlatformIOコマンド実行
            result = subprocess.run([
                'pio', 'test', '-e', 'unit-test-esp32', '--coverage'
            ], capture_output=True, text=True, check=True)
            
            # gcovデータ解析
            coverage_data = self.parse_gcov_data('unit-test-esp32')
            
            return {
                'environment': 'unit-test-esp32',
                'coverage_percentage': coverage_data['total_coverage'],
                'line_coverage': coverage_data['line_coverage'],
                'branch_coverage': coverage_data['branch_coverage'],
                'function_coverage': coverage_data['function_coverage'],
                'files': coverage_data['files']
            }
        except subprocess.CalledProcessError as e:
            raise CoverageMeasurementError(f"M5Stack依存実装カバレッジ計測失敗: {e}")
        
    def generate_integrated_report(self) -> dict:
        """
        統合カバレッジレポート生成
        Returns:
            統合レポートデータ
        """
        if not self.pure_coverage or not self.m5stack_coverage:
            raise CoverageMeasurementError("個別カバレッジデータが不足")
        
        # 統合カバレッジ計算
        self.integration_coverage = self.calculate_integrated_coverage()
        
        # レポート生成
        self.generate_reports()
        
        return self.integration_coverage
        
    def check_quality_gate(self) -> dict:
        """
        品質ゲート判定
        Returns:
            品質ゲート結果
        """
        if not self.integration_coverage:
            raise CoverageMeasurementError("統合カバレッジデータが不足")
        
        threshold = self.config.get('quality_gate', {}).get('threshold', 80.0)
        coverage = self.integration_coverage['coverage_percentage']
        
        passed = coverage >= threshold
        
        return {
            'passed': passed,
            'threshold': threshold,
            'actual_coverage': coverage,
            'message': f"カバレッジ {coverage:.1f}% {'達成' if passed else '未達成'} (閾値: {threshold}%)"
        }
```

#### 3.2.2 レポート生成
```python
class CoverageReportGenerator:
    def generate_html_report(self, coverage_data: dict, output_dir: str = "coverage_reports") -> str:
        """
        HTMLレポート生成
        Args:
            coverage_data: カバレッジデータ
            output_dir: 出力ディレクトリ
        Returns:
            生成されたHTMLファイルパス
        """
        os.makedirs(output_dir, exist_ok=True)
        
        # gcovrを使用してHTMLレポート生成
        html_file = os.path.join(output_dir, f"coverage_report_{datetime.now().strftime('%Y%m%d_%H%M%S')}.html")
        
        # gcovrコマンド実行
        subprocess.run([
            'gcovr',
            '--html',
            '--html-details',
            '--output', html_file,
            '--root', '.',
            '--exclude', 'test/',
            '--exclude', 'lib/Arduino.*',
            '--exclude', 'lib/M5Stack.*',
            '--exclude', 'lib/WiFi.*',
            '--exclude', 'lib/NTPClient.*'
        ], check=True)
        
        return html_file
        
    def generate_xml_report(self, coverage_data: dict, output_dir: str = "coverage_reports") -> str:
        """
        XMLレポート生成（CI/CD用）
        Args:
            coverage_data: カバレッジデータ
            output_dir: 出力ディレクトリ
        Returns:
            生成されたXMLファイルパス
        """
        os.makedirs(output_dir, exist_ok=True)
        
        xml_file = os.path.join(output_dir, f"coverage_report_{datetime.now().strftime('%Y%m%d_%H%M%S')}.xml")
        
        # gcovrを使用してXMLレポート生成
        subprocess.run([
            'gcovr',
            '--xml',
            '--output', xml_file,
            '--root', '.',
            '--exclude', 'test/',
            '--exclude', 'lib/Arduino.*',
            '--exclude', 'lib/M5Stack.*',
            '--exclude', 'lib/WiFi.*',
            '--exclude', 'lib/NTPClient.*'
        ], check=True)
        
        return xml_file
        
    def generate_json_report(self, coverage_data: dict, output_dir: str = "coverage_reports") -> str:
        """
        JSONレポート生成（API用）
        Args:
            coverage_data: カバレッジデータ
            output_dir: 出力ディレクトリ
        Returns:
            生成されたJSONファイルパス
        """
        os.makedirs(output_dir, exist_ok=True)
        
        json_file = os.path.join(output_dir, f"coverage_report_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json")
        
        # カバレッジデータをJSON形式で保存
        with open(json_file, 'w', encoding='utf-8') as f:
            json.dump(coverage_data, f, indent=2, ensure_ascii=False)
        
        return json_file
```

### 3.3 PlatformIO設定

#### 3.3.1 native環境（統一設定）
```ini
[env:native]
platform = native
build_flags = 
    -fprofile-arcs
    -ftest-coverage
    -O0
    -g
    -DUNITY_INCLUDE_CONFIG_H
lib_ldf_mode = deep+
test_framework = unity
test_build_src = yes
test_filter = pure
test_coverage = yes
test_coverage_output = html
test_coverage_output_dir = coverage_reports/native
```

#### 3.3.2 unit-test-esp32環境（統一設定）
```ini
[env:unit-test-esp32]
platform = espressif32
board = esp32dev
framework = arduino
build_flags = 
    -fprofile-arcs
    -ftest-coverage
    -O0
    -g
    -DUNITY_INCLUDE_CONFIG_H
lib_ldf_mode = deep+
test_framework = unity
test_build_src = yes
test_filter = integration
test_coverage = yes
test_coverage_output = html
test_coverage_output_dir = coverage_reports/esp32
```

#### 3.3.3 native環境（純粋ロジックテスト用）
```ini
[env:native]
platform = native
build_flags = 
    -DUNITY_INCLUDE_DOUBLE
    -DUNITY_DOUBLE_PRECISION=1e-12
    -DTEST_MODE
    -std=c++11
    -fprofile-arcs
    -ftest-coverage
    -lgcov
lib_deps =
    throwtheswitch/Unity @ ^2.5.2
build_unflags = -std=gnu++11
```

## 4. 運用設計

### 4.1 CI/CD統合

#### 4.1.1 GitHub Actions
```yaml
name: Coverage Measurement
on: [push, pull_request]

jobs:
  coverage:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Setup Python
        uses: actions/setup-python@v2
        with:
          python-version: 3.8
      - name: Install PlatformIO
        run: pip install platformio gcovr
      - name: Run Coverage Tests
        run: python scripts/test_coverage.py
      - name: Upload Coverage Report
        uses: codecov/codecov-action@v1
```

#### 4.1.2 品質ゲート
- **カバレッジ閾値**: 85%以上
- **失敗条件**: カバレッジ85%未満
- **通知**: Slack/Email通知
- **設定**: `coverage_config.json`で管理

### 4.2 監視と通知

#### 4.2.1 カバレッジ監視
```python
class CoverageMonitor:
    def __init__(self, config: dict):
        """
        初期化
        Args:
            config: 監視設定
        """
        self.config = config
        self.history_file = "coverage_history.json"
        
    def check_coverage_threshold(self, coverage_data: dict) -> dict:
        """
        カバレッジ閾値チェック
        Args:
            coverage_data: カバレッジデータ
        Returns:
            チェック結果
        """
        threshold = self.config.get('quality_gate', {}).get('threshold', 80.0)
        current_coverage = coverage_data['integration_coverage']['coverage_percentage']
        
        result = {
            'threshold': threshold,
            'current_coverage': current_coverage,
            'passed': current_coverage >= threshold,
            'trend': self.calculate_trend(current_coverage)
        }
        
        return result
        
    def send_notification(self, message: str, level: str = "info"):
        """
        通知送信
        Args:
            message: 通知メッセージ
            level: 通知レベル（info, warning, error）
        """
        # コンソール出力
        print(f"[{level.upper()}] {message}")
        
        # ファイル出力
        if self.config.get('notifications', {}).get('file', {}).get('enabled', False):
            self.log_to_file(message, level)
        
    def log_coverage_history(self, coverage_data: dict):
        """
        カバレッジ履歴記録
        Args:
            coverage_data: カバレッジデータ
        """
        history = self.load_history()
        
        entry = {
            'timestamp': datetime.now().isoformat(),
            'coverage': coverage_data['integration_coverage']['coverage_percentage'],
            'pure_coverage': coverage_data['pure_coverage']['coverage_percentage'],
            'm5stack_coverage': coverage_data['m5stack_coverage']['coverage_percentage'],
            'quality_gate': coverage_data['quality_gate']['passed']
        }
        
        history.append(entry)
        
        # 最新10件のみ保持
        if len(history) > 10:
            history = history[-10:]
        
        self.save_history(history)
```

#### 4.2.2 通知チャネル
- **Console**: リアルタイム出力
- **File**: ログファイル出力
- **Slack**: リアルタイム通知（設定時）
- **Email**: 日次レポート（設定時）

### 4.3 レポート管理

#### 4.3.1 レポート保存
- **場所**: `coverage_reports/`
- **形式**: HTML, XML, JSON
- **履歴**: 最新10件保持
- **自動クリーンアップ**: 30日以上前のレポート削除

#### 4.3.2 レポート表示
- **HTML**: ブラウザでの詳細表示
- **XML**: CI/CD統合用
- **JSON**: API用データ形式

## 5. 実装計画

### 5.1 Phase 1: 基盤構築
1. **カバレッジ計測スクリプト作成**
   - `scripts/test_coverage.py`
   - 基本的なカバレッジ計測機能
   - エラーハンドリング実装

2. **PlatformIO設定更新**
   - native環境のカバレッジ設定
   - unit-test-esp32環境のカバレッジ設定
   - 統合テスト環境の設定

3. **レポート生成機能**
   - HTMLレポート生成
   - XMLレポート生成
   - JSONレポート生成
   - 基本的な品質ゲート判定

### 5.2 Phase 2: 統合機能
1. **統合カバレッジ計測**
   - 純粋ロジックとM5Stack依存実装の統合
   - 重複除外機能
   - 統合レポート生成

2. **CI/CD統合**
   - GitHub Actions設定
   - 自動カバレッジ計測
   - 品質ゲート統合

3. **通知機能**
   - コンソール通知
   - ファイルログ
   - 品質ゲート通知

### 5.3 Phase 3: 高度機能
1. **詳細分析**
   - カバレッジ推移分析
   - 未カバー領域の特定
   - トレンド分析

2. **最適化**
   - カバレッジ計測速度向上
   - レポート生成最適化
   - メモリ使用量最適化

3. **拡張機能**
   - カスタムレポート
   - 詳細な分析機能
   - 予測機能

## 6. 成功指標

### 6.1 機能指標
- [ ] 純粋ロジックカバレッジ85%以上
- [ ] M5Stack依存実装カバレッジ80%以上
- [ ] 統合カバレッジ80%以上
- [ ] レポート生成成功率100%

### 6.2 品質指標
- [ ] カバレッジ計測精度±1%
- [ ] レポート生成時間5分以内
- [ ] CI/CD統合成功率100%
- [ ] 通知配信成功率100%

### 6.3 運用指標
- [ ] カバレッジ計測実行時間10分以内
- [ ] レポート保存容量1GB以下
- [ ] システム稼働率99%以上
- [ ] 障害復旧時間30分以内

## 7. リスク管理

### 7.1 技術リスク
- **gcov互換性**: ESP32環境でのgcov制限
- **メモリ不足**: カバレッジデータ生成時のメモリ不足
- **パフォーマンス**: カバレッジ計測によるビルド時間増加

### 7.2 運用リスク
- **CI/CD障害**: GitHub Actionsの障害
- **通知失敗**: 通知機能の失敗
- **データ損失**: カバレッジデータの損失

### 7.3 対策
- **代替手段**: 複数のカバレッジ計測ツール
- **バックアップ**: カバレッジデータの定期バックアップ
- **監視強化**: システム稼働状況の監視
- **エラーハンドリング**: 包括的なエラー処理

## 8. 今後の拡張

### 8.1 機能拡張
- **ブランチ別カバレッジ**: ブランチごとのカバレッジ比較
- **差分カバレッジ**: 変更部分のカバレッジ分析
- **カバレッジ予測**: 将来のカバレッジ予測

### 8.2 統合拡張
- **IDE統合**: VSCode/CLion統合
- **PR統合**: プルリクエスト時の自動分析
- **ダッシュボード**: リアルタイムダッシュボード

### 8.3 分析拡張
- **機械学習**: カバレッジパターン分析
- **最適化提案**: テスト最適化提案
- **品質予測**: 品質指標の予測 