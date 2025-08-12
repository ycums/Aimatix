# カバレッジ計測システム技術仕様書

## 1. 概要

### 1.1 仕様範囲
- DIパターンに対応したカバレッジ計測システムの技術仕様
- gcov/gcovrを使用したコードカバレッジ計測
- PlatformIO統合による自動化
- 統合レポート生成と品質ゲート判定

### 1.2 技術スタック
- **カバレッジ計測**: gcov/gcovr
- **ビルドシステム**: PlatformIO
- **テストフレームワーク**: Unity
- **レポート生成**: Python + gcovr
- **CI/CD**: GitHub Actions

## 2. カバレッジ計測スクリプト仕様

### 2.1 メインスクリプト: `scripts/test_coverage.py`

#### 2.1.1 クラス設計
```python
import subprocess
import json
import os
import sys
from datetime import datetime
from typing import Dict, List, Optional

class CoverageMeasurementError(Exception):
    """カバレッジ計測エラー"""
    pass

class BuildError(CoverageMeasurementError):
    """ビルドエラー"""
    pass

class TestError(CoverageMeasurementError):
    """テストエラー"""
    pass

class ReportError(CoverageMeasurementError):
    """レポート生成エラー"""
    pass

class CoverageMeasurementSystem:
    """カバレッジ計測システムのメインクラス"""
    
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
        self.report_generator = CoverageReportGenerator()
        self.monitor = CoverageMonitor(self.config)
    
    def load_config(self, config_path: str) -> dict:
        """
        設定ファイル読み込み
        Args:
            config_path: 設定ファイルパス
        Returns:
            設定データ
        """
        try:
            with open(config_path, 'r', encoding='utf-8') as f:
                return json.load(f)
        except FileNotFoundError:
            print(f"警告: 設定ファイル {config_path} が見つかりません。デフォルト設定を使用します。")
            return self.get_default_config()
        except json.JSONDecodeError as e:
            raise CoverageMeasurementError(f"設定ファイルのJSON形式エラー: {e}")
    
    def get_default_config(self) -> dict:
        """
        デフォルト設定を取得
        Returns:
            デフォルト設定
        """
        return {
            "coverage_measurement": {
                "enabled": True,
                "environments": {
                    "native": {
                        "enabled": True,
                        "test_filter": "pure",
                        "output_dir": "coverage_reports/native"
                    },
                    "unit-test-esp32": {
                        "enabled": True,
                        "test_filter": "integration",
                        "output_dir": "coverage_reports/esp32"
                    }
                }
            },
            "quality_gate": {
                "enabled": True,
                "threshold": 80.0,
                "strict_mode": False
            },
            "exclusions": {
                "directories": [
                    "test/",
                    "lib/Arduino.*",
                    "lib/M5Stack.*",
                    "lib/WiFi.*",
                    "coverage_reports/"
                ],
                "files": [
                    "*.h",
                    "test_*.cpp",
                    "mock_*.cpp",
                    "mock_*.h"
                ]
            },
            "notifications": {
                "enabled": True,
                "channels": {
                    "console": {
                        "enabled": True,
                        "level": "info"
                    },
                    "file": {
                        "enabled": True,
                        "path": "coverage_notifications.log",
                        "level": "warning"
                    }
                }
            },
            "history": {
                "enabled": True,
                "max_entries": 10,
                "file_path": "coverage_history.json"
            }
        }
    
    def run_full_coverage_measurement(self) -> dict:
        """
        全体的なカバレッジ計測を実行
        Returns:
            統合カバレッジデータ
        """
        try:
            # 1. 純粋ロジックカバレッジ計測
            if self.config.get('coverage_measurement', {}).get('environments', {}).get('native', {}).get('enabled', True):
                self.pure_coverage = self.measure_pure_logic_coverage()
            
            # 2. M5Stack依存実装カバレッジ計測
            if self.config.get('coverage_measurement', {}).get('environments', {}).get('unit-test-esp32', {}).get('enabled', True):
                self.m5stack_coverage = self.measure_m5stack_coverage()
            
            # 3. 統合カバレッジ計算
            self.integration_coverage = self.calculate_integrated_coverage()
            
            # 4. レポート生成
            self.generate_reports()
            
            # 5. 品質ゲート判定
            quality_result = self.check_quality_gate()
            
            # 6. 履歴記録
            if self.config.get('history', {}).get('enabled', True):
                self.monitor.log_coverage_history({
                    'pure_coverage': self.pure_coverage,
                    'm5stack_coverage': self.m5stack_coverage,
                    'integration_coverage': self.integration_coverage,
                    'quality_gate': quality_result
                })
            
            return {
                'pure_coverage': self.pure_coverage,
                'm5stack_coverage': self.m5stack_coverage,
                'integration_coverage': self.integration_coverage,
                'quality_gate': quality_result
            }
        except CoverageMeasurementError as e:
            self.monitor.send_notification(f"カバレッジ計測エラー: {e}", "error")
            raise
        except Exception as e:
            self.monitor.send_notification(f"予期しないエラー: {e}", "error")
            raise CoverageMeasurementError(f"予期しないエラー: {e}")
    
    def measure_pure_logic_coverage(self) -> dict:
        """
        純粋ロジックのカバレッジ計測
        Returns:
            カバレッジデータ
        """
        try:
            self.monitor.send_notification("純粋ロジックカバレッジ計測開始", "info")
            
            # PlatformIOコマンド実行
            result = subprocess.run([
                'pio', 'test', '-e', 'native', '--coverage'
            ], capture_output=True, text=True, check=True)
            
            # gcovデータ解析
            coverage_data = self.parse_gcov_data('native')
            
            self.monitor.send_notification(f"純粋ロジックカバレッジ計測完了: {coverage_data['total_coverage']:.1f}%", "info")
            
            return {
                'environment': 'native',
                'coverage_percentage': coverage_data['total_coverage'],
                'line_coverage': coverage_data['line_coverage'],
                'branch_coverage': coverage_data['branch_coverage'],
                'function_coverage': coverage_data['function_coverage'],
                'files': coverage_data['files']
            }
        except subprocess.CalledProcessError as e:
            raise BuildError(f"純粋ロジックカバレッジ計測失敗: {e.stderr}")
    
    def measure_m5stack_coverage(self) -> dict:
        """
        M5Stack依存実装のカバレッジ計測
        Returns:
            カバレッジデータ
        """
        try:
            self.monitor.send_notification("M5Stack依存実装カバレッジ計測開始", "info")
            
            # PlatformIOコマンド実行
            result = subprocess.run([
                'pio', 'test', '-e', 'unit-test-esp32', '--coverage'
            ], capture_output=True, text=True, check=True)
            
            # gcovデータ解析
            coverage_data = self.parse_gcov_data('unit-test-esp32')
            
            self.monitor.send_notification(f"M5Stack依存実装カバレッジ計測完了: {coverage_data['total_coverage']:.1f}%", "info")
            
            return {
                'environment': 'unit-test-esp32',
                'coverage_percentage': coverage_data['total_coverage'],
                'line_coverage': coverage_data['line_coverage'],
                'branch_coverage': coverage_data['branch_coverage'],
                'function_coverage': coverage_data['function_coverage'],
                'files': coverage_data['files']
            }
        except subprocess.CalledProcessError as e:
            raise BuildError(f"M5Stack依存実装カバレッジ計測失敗: {e.stderr}")
    
    def parse_gcov_data(self, environment: str) -> dict:
        """
        gcovデータ解析
        Args:
            environment: 環境名
        Returns:
            解析されたカバレッジデータ
        """
        try:
            # gcovrを使用してカバレッジデータ解析
            result = subprocess.run([
                'gcovr',
                '--json',
                '--root', '.',
                '--exclude', 'test/',
                '--exclude', 'lib/Arduino.*',
                '--exclude', 'lib/M5Stack.*',
                '--exclude', 'lib/WiFi.*',
            ], capture_output=True, text=True, check=True)
            
            coverage_data = json.loads(result.stdout)
            
            # カバレッジ計算
            total_lines = 0
            covered_lines = 0
            total_branches = 0
            covered_branches = 0
            total_functions = 0
            covered_functions = 0
            
            for file_data in coverage_data.get('files', []):
                total_lines += file_data.get('line_total', 0)
                covered_lines += file_data.get('line_covered', 0)
                total_branches += file_data.get('branch_total', 0)
                covered_branches += file_data.get('branch_covered', 0)
                total_functions += file_data.get('function_total', 0)
                covered_functions += file_data.get('function_covered', 0)
            
            return {
                'total_coverage': (covered_lines / total_lines * 100) if total_lines > 0 else 0,
                'line_coverage': {
                    'total': total_lines,
                    'covered': covered_lines,
                    'percentage': (covered_lines / total_lines * 100) if total_lines > 0 else 0
                },
                'branch_coverage': {
                    'total': total_branches,
                    'covered': covered_branches,
                    'percentage': (covered_branches / total_branches * 100) if total_branches > 0 else 0
                },
                'function_coverage': {
                    'total': total_functions,
                    'covered': covered_functions,
                    'percentage': (covered_functions / total_functions * 100) if total_functions > 0 else 0
                },
                'files': coverage_data.get('files', [])
            }
        except subprocess.CalledProcessError as e:
            raise ReportError(f"gcovデータ解析失敗: {e.stderr}")
        except json.JSONDecodeError as e:
            raise ReportError(f"gcovデータJSON解析失敗: {e}")
    
    def calculate_integrated_coverage(self) -> dict:
        """
        統合カバレッジ計算
        Returns:
            統合カバレッジデータ
        """
        if not self.pure_coverage or not self.m5stack_coverage:
            raise CoverageMeasurementError("個別カバレッジデータが不足")
        
        # 重複除外ロジック
        integrated_files = self.merge_coverage_files(
            self.pure_coverage['files'],
            self.m5stack_coverage['files']
        )
        
        # 統合カバレッジ計算
        total_lines = 0
        covered_lines = 0
        total_branches = 0
        covered_branches = 0
        total_functions = 0
        covered_functions = 0
        
        for file_data in integrated_files.values():
            total_lines += file_data['total_lines']
            covered_lines += file_data['covered_lines']
            total_branches += file_data['total_branches']
            covered_branches += file_data['covered_branches']
            total_functions += file_data['total_functions']
            covered_functions += file_data['covered_functions']
        
        return {
            'environment': 'integrated',
            'coverage_percentage': (covered_lines / total_lines * 100) if total_lines > 0 else 0,
            'line_coverage': {
                'total': total_lines,
                'covered': covered_lines,
                'percentage': (covered_lines / total_lines * 100) if total_lines > 0 else 0
            },
            'branch_coverage': {
                'total': total_branches,
                'covered': covered_branches,
                'percentage': (covered_branches / total_branches * 100) if total_branches > 0 else 0
            },
            'function_coverage': {
                'total': total_functions,
                'covered': covered_functions,
                'percentage': (covered_functions / total_functions * 100) if total_functions > 0 else 0
            },
            'files': integrated_files
        }
    
    def merge_coverage_files(self, pure_files: List[dict], m5stack_files: List[dict]) -> Dict[str, dict]:
        """
        カバレッジファイルの統合
        Args:
            pure_files: 純粋ロジックファイル
            m5stack_files: M5Stack依存ファイル
        Returns:
            統合されたファイルデータ
        """
        merged_files = {}
        
        # 純粋ロジックファイルを追加
        for file_data in pure_files:
            file_path = file_data.get('file', '')
            merged_files[file_path] = {
                'total_lines': file_data.get('line_total', 0),
                'covered_lines': file_data.get('line_covered', 0),
                'total_branches': file_data.get('branch_total', 0),
                'covered_branches': file_data.get('branch_covered', 0),
                'total_functions': file_data.get('function_total', 0),
                'covered_functions': file_data.get('function_covered', 0)
            }
        
        # M5Stack依存ファイルを追加（重複時は上書き）
        for file_data in m5stack_files:
            file_path = file_data.get('file', '')
            merged_files[file_path] = {
                'total_lines': file_data.get('line_total', 0),
                'covered_lines': file_data.get('line_covered', 0),
                'total_branches': file_data.get('branch_total', 0),
                'covered_branches': file_data.get('branch_covered', 0),
                'total_functions': file_data.get('function_total', 0),
                'covered_functions': file_data.get('function_covered', 0)
            }
        
        return merged_files
    
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
        
        result = {
            'passed': passed,
            'threshold': threshold,
            'actual_coverage': coverage,
            'message': f"カバレッジ {coverage:.1f}% {'達成' if passed else '未達成'} (閾値: {threshold}%)"
        }
        
        if passed:
            self.monitor.send_notification(result['message'], "info")
        else:
            self.monitor.send_notification(result['message'], "warning")
        
        return result
    
    def generate_reports(self):
        """レポート生成"""
        try:
            if not self.integration_coverage:
                raise CoverageMeasurementError("統合カバレッジデータが不足")
            
            self.report_generator.generate_html_report(self.integration_coverage)
            self.report_generator.generate_xml_report(self.integration_coverage)
            self.report_generator.generate_json_report(self.integration_coverage)
            
            self.monitor.send_notification("レポート生成完了", "info")
        except Exception as e:
            raise ReportError(f"レポート生成失敗: {e}")
```

### 2.2 レポート生成クラス: `CoverageReportGenerator`

#### 2.2.1 HTMLレポート生成
```python
class CoverageReportGenerator:
    """カバレッジレポート生成クラス"""
    
    def generate_html_report(self, coverage_data: dict, output_dir: str = "coverage_reports") -> str:
        """
        HTMLレポート生成
        Args:
            coverage_data: カバレッジデータ
            output_dir: 出力ディレクトリ
        Returns:
            生成されたHTMLファイルパス
        """
        try:
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
            ], check=True)
            
            return html_file
        except subprocess.CalledProcessError as e:
            raise ReportError(f"HTMLレポート生成失敗: {e}")
    
    def generate_xml_report(self, coverage_data: dict, output_dir: str = "coverage_reports") -> str:
        """
        XMLレポート生成（CI/CD用）
        Args:
            coverage_data: カバレッジデータ
            output_dir: 出力ディレクトリ
        Returns:
            生成されたXMLファイルパス
        """
        try:
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
        except subprocess.CalledProcessError as e:
            raise ReportError(f"XMLレポート生成失敗: {e}")
    
    def generate_json_report(self, coverage_data: dict, output_dir: str = "coverage_reports") -> str:
        """
        JSONレポート生成（API用）
        Args:
            coverage_data: カバレッジデータ
            output_dir: 出力ディレクトリ
        Returns:
            生成されたJSONファイルパス
        """
        try:
            os.makedirs(output_dir, exist_ok=True)
            
            json_file = os.path.join(output_dir, f"coverage_report_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json")
            
            # カバレッジデータをJSON形式で保存
            with open(json_file, 'w', encoding='utf-8') as f:
                json.dump(coverage_data, f, indent=2, ensure_ascii=False)
            
            return json_file
        except Exception as e:
            raise ReportError(f"JSONレポート生成失敗: {e}")
```

### 2.3 監視クラス: `CoverageMonitor`

#### 2.3.1 カバレッジ監視
```python
class CoverageMonitor:
    """カバレッジ監視クラス"""
    
    def __init__(self, config: dict):
        """
        初期化
        Args:
            config: 監視設定
        """
        self.config = config
        self.history_file = config.get('history', {}).get('file_path', 'coverage_history.json')
        self.notification_config = config.get('notifications', {})
    
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
    
    def calculate_trend(self, current_coverage: float) -> str:
        """
        カバレッジトレンド計算
        Args:
            current_coverage: 現在のカバレッジ
        Returns:
            トレンド文字列
        """
        history = self.load_history()
        if len(history) < 2:
            return "unknown"
        
        previous_coverage = history[-2]['coverage']
        if current_coverage > previous_coverage:
            return "up"
        elif current_coverage < previous_coverage:
            return "down"
        else:
            return "stable"
    
    def send_notification(self, message: str, level: str = "info"):
        """
        通知送信
        Args:
            message: 通知メッセージ
            level: 通知レベル（info, warning, error）
        """
        # コンソール出力
        if self.notification_config.get('console', {}).get('enabled', True):
            console_level = self.notification_config.get('console', {}).get('level', 'info')
            if self.should_send_notification(level, console_level):
                print(f"[{level.upper()}] {message}")
        
        # ファイル出力
        if self.notification_config.get('file', {}).get('enabled', False):
            file_level = self.notification_config.get('file', {}).get('level', 'warning')
            if self.should_send_notification(level, file_level):
                self.log_to_file(message, level)
    
    def should_send_notification(self, message_level: str, config_level: str) -> bool:
        """
        通知送信判定
        Args:
            message_level: メッセージレベル
            config_level: 設定レベル
        Returns:
            送信するかどうか
        """
        levels = {'info': 1, 'warning': 2, 'error': 3}
        return levels.get(message_level, 1) >= levels.get(config_level, 1)
    
    def log_to_file(self, message: str, level: str):
        """
        ファイルログ出力
        Args:
            message: メッセージ
            level: レベル
        """
        try:
            log_file = self.notification_config.get('file', {}).get('path', 'coverage_notifications.log')
            with open(log_file, 'a', encoding='utf-8') as f:
                timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
                f.write(f"[{timestamp}] [{level.upper()}] {message}\n")
        except Exception as e:
            print(f"ログファイル出力失敗: {e}")
    
    def log_coverage_history(self, coverage_data: dict):
        """
        カバレッジ履歴記録
        Args:
            coverage_data: カバレッジデータ
        """
        try:
            history = self.load_history()
            
            entry = {
                'timestamp': datetime.now().isoformat(),
                'coverage': coverage_data['integration_coverage']['coverage_percentage'],
                'pure_coverage': coverage_data['pure_coverage']['coverage_percentage'],
                'm5stack_coverage': coverage_data['m5stack_coverage']['coverage_percentage'],
                'quality_gate': coverage_data['quality_gate']['passed']
            }
            
            history.append(entry)
            
            # 最新N件のみ保持
            max_entries = self.config.get('history', {}).get('max_entries', 10)
            if len(history) > max_entries:
                history = history[-max_entries:]
            
            self.save_history(history)
        except Exception as e:
            print(f"履歴記録失敗: {e}")
    
    def load_history(self) -> List[dict]:
        """
        履歴読み込み
        Returns:
            履歴データ
        """
        try:
            if os.path.exists(self.history_file):
                with open(self.history_file, 'r', encoding='utf-8') as f:
                    return json.load(f)
            return []
        except Exception as e:
            print(f"履歴読み込み失敗: {e}")
            return []
    
    def save_history(self, history: List[dict]):
        """
        履歴保存
        Args:
            history: 履歴データ
        """
        try:
            with open(self.history_file, 'w', encoding='utf-8') as f:
                json.dump(history, f, indent=2, ensure_ascii=False)
        except Exception as e:
            print(f"履歴保存失敗: {e}")
```

## 3. PlatformIO設定仕様

### 3.1 native環境設定（統一設定）

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

### 3.2 unit-test-esp32環境設定（統一設定）

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

### 3.3 native環境設定（純粋ロジックテスト用）

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

## 4. 設定ファイル仕様

### 4.1 カバレッジ設定ファイル: `coverage_config.json`（数値の正典）

#### 4.1.1 統一設定形式
```json
{
  "coverage_measurement": {
    "enabled": true,
    "environments": {
      "native": {
        "enabled": true,
        "test_filter": "pure",
        "output_dir": "coverage_reports/native"
      },
      "unit-test-esp32": {
        "enabled": true,
        "test_filter": "integration",
        "output_dir": "coverage_reports/esp32"
      }
    }
  },
  "quality_gate": {
    "enabled": true,
    "threshold": 80.0,
    "strict_mode": false
  },
  "exclusions": {
    "directories": [
      "test/",
      "lib/Arduino.*",
      "lib/M5Stack.*",
      "lib/WiFi.*",
      "coverage_reports/"
    ],
    "files": [
      "*.h",
      "test_*.cpp",
      "mock_*.cpp",
      "mock_*.h"
    ]
  },
  "notifications": {
    "enabled": true,
    "channels": {
      "console": {
        "enabled": true,
        "level": "info"
      },
      "file": {
        "enabled": true,
        "path": "coverage_notifications.log",
        "level": "warning"
      }
    }
  },
  "history": {
    "enabled": true,
    "max_entries": 10,
    "file_path": "coverage_history.json"
  }
}
```

## 5. API仕様

### 5.1 コマンドラインAPI

#### 5.1.1 基本コマンド
```bash
# 全体的なカバレッジ計測
python scripts/test_coverage.py

# 特定環境のカバレッジ計測
python scripts/test_coverage.py --environment native
python scripts/test_coverage.py --environment unit-test-esp32

# レポート生成のみ
python scripts/test_coverage.py --report-only

# 品質ゲートチェックのみ
python scripts/test_coverage.py --quality-gate-only
```

#### 5.1.2 オプション
```bash
# ヘルプ表示
python scripts/test_coverage.py --help

# 設定ファイル指定
python scripts/test_coverage.py --config custom_config.json

# 出力ディレクトリ指定
python scripts/test_coverage.py --output-dir custom_reports

# 詳細出力
python scripts/test_coverage.py --verbose

# デバッグモード
python scripts/test_coverage.py --debug
```

### 5.2 Python API（抜粋）

#### 5.2.1 基本使用例
```python
from scripts.test_coverage import CoverageMeasurementSystem

# システム初期化
coverage_system = CoverageMeasurementSystem()

# 全体的なカバレッジ計測実行
result = coverage_system.run_full_coverage_measurement()

# 結果表示
print(f"統合カバレッジ: {result['integration_coverage']['coverage_percentage']:.1f}%")
print(f"品質ゲート: {'通過' if result['quality_gate']['passed'] else '未通過'}")
```

#### 5.2.2 個別機能使用例
```python
# 純粋ロジックカバレッジ計測
pure_coverage = coverage_system.measure_pure_logic_coverage()

# M5Stack依存実装カバレッジ計測
m5stack_coverage = coverage_system.measure_m5stack_coverage()

# レポート生成
coverage_system.generate_reports()

# 品質ゲート判定
quality_result = coverage_system.check_quality_gate()
```

## 6. エラーハンドリング

### 6.1 エラー種類

#### 6.1.1 基本エラークラス
```python
class CoverageMeasurementError(Exception):
    """カバレッジ計測エラー"""
    pass

class BuildError(CoverageMeasurementError):
    """ビルドエラー"""
    pass

class TestError(CoverageMeasurementError):
    """テストエラー"""
    pass

class ReportError(CoverageMeasurementError):
    """レポート生成エラー"""
    pass

class ConfigError(CoverageMeasurementError):
    """設定エラー"""
    pass
```

#### 6.1.2 エラーケース
- **BuildError**: PlatformIOビルド失敗
- **TestError**: テスト実行失敗
- **ReportError**: レポート生成失敗
- **ConfigError**: 設定ファイル読み込み失敗
- **CoverageMeasurementError**: その他のカバレッジ計測エラー

### 6.2 エラー処理

#### 6.2.1 エラーハンドリング例
```python
try:
    result = coverage_system.run_full_coverage_measurement()
except BuildError as e:
    print(f"ビルドエラー: {e}")
    sys.exit(1)
except TestError as e:
    print(f"テストエラー: {e}")
    sys.exit(1)
except ReportError as e:
    print(f"レポート生成エラー: {e}")
    sys.exit(1)
except ConfigError as e:
    print(f"設定エラー: {e}")
    sys.exit(1)
except CoverageMeasurementError as e:
    print(f"カバレッジ計測エラー: {e}")
    sys.exit(1)
except Exception as e:
    print(f"予期しないエラー: {e}")
    sys.exit(1)
```

#### 6.2.2 復旧手順
```python
def handle_error(error: Exception) -> bool:
    """
    エラー処理と復旧
    Args:
        error: 発生したエラー
    Returns:
        復旧成功時True
    """
    if isinstance(error, BuildError):
        # ビルドエラーの復旧
        return handle_build_error(error)
    elif isinstance(error, TestError):
        # テストエラーの復旧
        return handle_test_error(error)
    elif isinstance(error, ReportError):
        # レポートエラーの復旧
        return handle_report_error(error)
    else:
        # その他のエラー
        return False

def handle_build_error(error: BuildError) -> bool:
    """ビルドエラー復旧"""
    try:
        # キャッシュクリア
        subprocess.run(['pio', 'run', '-t', 'clean'], check=True)
        return True
    except:
        return False
```

## 7. パフォーマンス仕様

### 7.1 実行時間目標
- **純粋ロジックカバレッジ計測**: 3分以内
- **M5Stack依存実装カバレッジ計測**: 5分以内
- **統合カバレッジ計算**: 1分以内
- **レポート生成**: 2分以内
- **全体実行時間**: 10分以内

### 7.2 メモリ使用量目標
- **最大メモリ使用量**: 1GB以下
- **一時ファイルサイズ**: 100MB以下
- **レポートファイルサイズ**: 10MB以下

### 7.3 並列処理
- **並列実行**: 有効
- **最大ワーカー数**: 4
- **タイムアウト**: 5分

## 8. セキュリティ仕様

### 8.1 ファイルアクセス
- **読み取り専用**: 設定ファイル、ソースコード
- **書き込み専用**: レポートディレクトリ、ログファイル
- **実行専用**: 一時ディレクトリ

### 8.2 ネットワークアクセス
- **制限**: ローカル環境のみ
- **外部接続**: 通知機能のみ（設定時）

### 8.3 データ保護
- **機密情報**: 設定ファイル内の認証情報
- **ログ情報**: カバレッジデータ、エラーログ
- **履歴データ**: カバレッジ履歴 