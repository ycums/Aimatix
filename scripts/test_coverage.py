#!/usr/bin/env python3
"""
Aimatix カバレッジ計測システム
DIパターンに対応した新しいカバレッジ計測システム

使用方法:
    python scripts/test_coverage.py --quick      # 開発中の軽量計測
    python scripts/test_coverage.py --full       # 機能完成時の詳細計測
    python scripts/test_coverage.py --release    # リリース前の完全計測
    python scripts/test_coverage.py --baseline   # ベースライン計測
    python scripts/test_coverage.py --check-env  # 環境確認
    python scripts/test_coverage.py --clean      # 古いレポートファイル削除
"""

import subprocess
import json
import os
import sys
import argparse
from datetime import datetime
from typing import Dict, List, Optional
from pathlib import Path

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
        self.monitor = CoverageMonitor(self.config)
        self.report_generator = CoverageReportGenerator(self.config)
        
        # カバレッジデータ
        self.pure_coverage = None
        self.m5stack_coverage = None
        self.integration_coverage = None
        
        # 履歴ファイルパス
        self.history_file = self.config.get('history', {}).get('file_path', 'coverage_history.json')
    
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
        except UnicodeDecodeError as e:
            print(f"警告: 設定ファイル {config_path} のエンコーディングエラー: {e}")
            print("デフォルト設定を使用します。")
            return self.get_default_config()
    
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
                    "lib/NTPClient.*",
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
            },
            "cleanup": {
                "enabled": True,
                "keep_days": 7,
                "max_files": 50
            }
        }
    
    def run_quick_coverage_measurement(self) -> dict:
        """
        クイックカバレッジ計測（開発中）
        Returns:
            カバレッジ計測結果
        """
        self.monitor.send_notification("クイックカバレッジ計測開始", "info")
        
        try:
            # 純粋ロジックカバレッジ計測
            pure_coverage = self.measure_pure_logic_coverage()
            self.pure_coverage = pure_coverage  # インスタンス変数に設定
            
            # 統合カバレッジ計算
            integration_coverage = self.calculate_integrated_coverage()
            
            # 品質ゲート判定
            quality_gate = self.check_quality_gate()
            
            # レポート生成
            self.generate_reports()
            
            # 自動クリーンアップ（設定が有効な場合）
            if self.config.get('cleanup', {}).get('enabled', True):
                self.clean_old_reports()
            
            result = {
                'mode': 'quick',
                'pure_coverage': pure_coverage,
                'integration_coverage': integration_coverage,
                'quality_gate': quality_gate,
                'timestamp': datetime.now().isoformat()
            }
            
            # 履歴記録
            self.log_coverage_history(result)
            
            self.monitor.send_notification("クイックカバレッジ計測完了", "info")
            return result
            
        except Exception as e:
            self.monitor.send_notification(f"クイックカバレッジ計測エラー: {e}", "error")
            raise
    
    def run_full_coverage_measurement(self) -> dict:
        """
        フルカバレッジ計測（機能完成時）
        Returns:
            カバレッジ計測結果
        """
        self.monitor.send_notification("フルカバレッジ計測開始", "info")
        
        try:
            # 純粋ロジックカバレッジ計測
            pure_coverage = self.measure_pure_logic_coverage()
            self.pure_coverage = pure_coverage  # インスタンス変数に設定
            
            # M5Stack依存実装カバレッジ計測
            m5stack_coverage = None
            try:
                m5stack_coverage = self.measure_m5stack_coverage()
                self.m5stack_coverage = m5stack_coverage  # インスタンス変数に設定
            except Exception as e:
                self.monitor.send_notification(f"M5Stack依存実装カバレッジ計測失敗: {e}", "warning")
            
            # 統合カバレッジ計算
            integration_coverage = self.calculate_integrated_coverage()
            
            # 品質ゲート判定
            quality_gate = self.check_quality_gate()
            
            # 詳細レポート生成
            self.generate_reports()
            
            # 自動クリーンアップ（設定が有効な場合）
            if self.config.get('cleanup', {}).get('enabled', True):
                self.clean_old_reports()
            
            result = {
                'mode': 'full',
                'pure_coverage': pure_coverage,
                'm5stack_coverage': m5stack_coverage,
                'integration_coverage': integration_coverage,
                'quality_gate': quality_gate,
                'timestamp': datetime.now().isoformat()
            }
            
            # 履歴記録
            self.log_coverage_history(result)
            
            self.monitor.send_notification("フルカバレッジ計測完了", "info")
            return result
            
        except Exception as e:
            self.monitor.send_notification(f"フルカバレッジ計測エラー: {e}", "error")
            raise
    
    def run_release_coverage_measurement(self) -> dict:
        """
        リリースカバレッジ計測（リリース前）
        Returns:
            カバレッジ計測結果
        """
        self.monitor.send_notification("リリースカバレッジ計測開始", "info")
        
        try:
            # 厳格モードを有効化
            self.config['quality_gate']['strict_mode'] = True
            
            # フルカバレッジ計測を実行
            result = self.run_full_coverage_measurement()
            result['mode'] = 'release'
            
            # 厳格モードでの品質ゲート再判定
            quality_gate = self.check_quality_gate()
            result['quality_gate'] = quality_gate
            
            # 自動クリーンアップ（設定が有効な場合）
            if self.config.get('cleanup', {}).get('enabled', True):
                self.clean_old_reports()
            
            self.monitor.send_notification("リリースカバレッジ計測完了", "info")
            return result
            
        except Exception as e:
            self.monitor.send_notification(f"リリースカバレッジ計測エラー: {e}", "error")
            raise
    
    def run_baseline_measurement(self) -> dict:
        """
        ベースライン計測
        Returns:
            ベースラインカバレッジデータ
        """
        try:
            self.monitor.send_notification("ベースライン計測開始", "info")
            
            # 現在のカバレッジをベースラインとして記録
            result = self.run_quick_coverage_measurement()
            
            # ベースラインファイルに保存
            baseline_file = "coverage_baseline.json"
            with open(baseline_file, 'w', encoding='utf-8') as f:
                json.dump({
                    'timestamp': datetime.now().isoformat(),
                    'baseline_coverage': result['integration_coverage']['coverage_percentage'],
                    'pure_coverage': result['pure_coverage']['coverage_percentage'] if result['pure_coverage'] else 0.0
                }, f, indent=2, ensure_ascii=False)
            
            self.monitor.send_notification(f"ベースライン計測完了: {result['integration_coverage']['coverage_percentage']:.1f}%", "info")
            
            return result
        except Exception as e:
            self.monitor.send_notification(f"ベースライン計測エラー: {e}", "error")
            raise
    
    def check_environment(self) -> dict:
        """
        環境確認
        Returns:
            環境確認結果
        """
        try:
            self.monitor.send_notification("環境確認開始", "info")
            
            results = {
                'platformio': False,
                'gcovr': False,
                'python': False,
                'config_file': False,
                'test_directories': False
            }
            
            # PlatformIO確認
            try:
                result = subprocess.run(['pio', '--version'], capture_output=True, text=True, check=True)
                results['platformio'] = True
                self.monitor.send_notification(f"PlatformIO確認: OK ({result.stdout.strip()})", "info")
            except Exception as e:
                self.monitor.send_notification(f"PlatformIO確認: NG ({e})", "error")
            
            # gcovr確認
            try:
                result = subprocess.run(['gcovr', '--version'], capture_output=True, text=True, check=True)
                results['gcovr'] = True
                self.monitor.send_notification(f"gcovr確認: OK", "info")
            except Exception as e:
                self.monitor.send_notification(f"gcovr確認: NG ({e})", "error")
            
            # Python確認
            try:
                results['python'] = True
                self.monitor.send_notification(f"Python確認: OK ({sys.version})", "info")
            except Exception as e:
                self.monitor.send_notification(f"Python確認: NG ({e})", "error")
            
            # 設定ファイル確認
            try:
                if os.path.exists("coverage_config.json"):
                    results['config_file'] = True
                    self.monitor.send_notification("設定ファイル確認: OK", "info")
                else:
                    self.monitor.send_notification("設定ファイル確認: NG (ファイルなし)", "warning")
            except Exception as e:
                self.monitor.send_notification(f"設定ファイル確認: NG ({e})", "error")
            
            # テストディレクトリ確認
            try:
                if os.path.exists("test/pure") and os.path.exists("test/integration"):
                    results['test_directories'] = True
                    self.monitor.send_notification("テストディレクトリ確認: OK", "info")
                else:
                    self.monitor.send_notification("テストディレクトリ確認: NG (ディレクトリなし)", "warning")
            except Exception as e:
                self.monitor.send_notification(f"テストディレクトリ確認: NG ({e})", "error")
            
            # 全体結果
            all_ok = all(results.values())
            if all_ok:
                self.monitor.send_notification("環境確認: 全てOK", "info")
            else:
                self.monitor.send_notification("環境確認: 一部NG", "warning")
            
            return results
        except Exception as e:
            self.monitor.send_notification(f"環境確認エラー: {e}", "error")
            raise
    
    def measure_pure_logic_coverage(self) -> dict:
        """
        純粋ロジックのカバレッジ計測
        Returns:
            カバレッジデータ
        """
        try:
            self.monitor.send_notification("純粋ロジックカバレッジ計測開始", "info")
            
            # PlatformIOテスト実行（カバレッジ設定はplatformio.iniで有効化）
            result = subprocess.run([
                'pio', 'test', '-e', 'native'
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
            
            # PlatformIOテスト実行（カバレッジ設定はplatformio.iniで有効化）
            result = subprocess.run([
                'pio', 'test', '-e', 'unit-test-esp32'
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
            self.monitor.send_notification(f"M5Stack依存実装カバレッジ計測失敗: {e.stderr}", "warning")
            # エラーの場合は0%カバレッジを返す
            return {
                'environment': 'unit-test-esp32',
                'coverage_percentage': 0.0,
                'line_coverage': {'total': 0, 'covered': 0, 'percentage': 0.0},
                'branch_coverage': {'total': 0, 'covered': 0, 'percentage': 0.0},
                'function_coverage': {'total': 0, 'covered': 0, 'percentage': 0.0},
                'files': []
            }
        except Exception as e:
            self.monitor.send_notification(f"M5Stack依存実装カバレッジ計測エラー: {e}", "warning")
            # エラーの場合は0%カバレッジを返す
            return {
                'environment': 'unit-test-esp32',
                'coverage_percentage': 0.0,
                'line_coverage': {'total': 0, 'covered': 0, 'percentage': 0.0},
                'branch_coverage': {'total': 0, 'covered': 0, 'percentage': 0.0},
                'function_coverage': {'total': 0, 'covered': 0, 'percentage': 0.0},
                'files': []
            }
    
    def parse_gcov_data(self, environment: str) -> dict:
        """
        gcovデータ解析
        Args:
            environment: 環境名
        Returns:
            解析されたカバレッジデータ
        """
        try:
            # gcovrを使用してカバレッジデータ解析（プロジェクトルートから実行）
            result = subprocess.run([
                'gcovr',
                '--json',
                '--root', '.',
                '--exclude', 'test/',
                '--exclude', 'lib/Arduino.*',
                '--exclude', 'lib/M5Stack.*',
                '--exclude', 'lib/WiFi.*',
                '--exclude', 'lib/NTPClient.*',
                '--exclude', 'coverage_reports/',
                '--exclude', '.pio/libdeps/',
                '--gcov-ignore-errors=no_working_dir_found',
                '--gcov-ignore-errors=source_not_found'
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
                # 各行の実行回数を集計
                file_total_lines = 0
                file_covered_lines = 0
                
                for line_data in file_data.get('lines', []):
                    file_total_lines += 1
                    if line_data.get('count', 0) > 0:
                        file_covered_lines += 1
                
                total_lines += file_total_lines
                covered_lines += file_covered_lines
                
                # 分岐と関数の集計
                total_branches += file_data.get('branch_total', 0)
                covered_branches += file_data.get('branch_covered', 0)
                total_functions += file_data.get('function_total', 0)
                covered_functions += file_data.get('function_covered', 0)
            
            # ファイルが見つからない場合は、手動でgcovファイルを解析
            if not coverage_data.get('files'):
                self.monitor.send_notification("gcovrでファイルが見つかりません。手動解析を実行します。", "warning")
                return self.parse_gcov_files_manually(environment)
            
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
            self.monitor.send_notification(f"gcovr実行エラー: {e.stderr}", "warning")
            return self.parse_gcov_files_manually(environment)
        except json.JSONDecodeError as e:
            self.monitor.send_notification(f"gcovデータJSON解析失敗: {e}", "warning")
            return self.parse_gcov_files_manually(environment)
        except Exception as e:
            self.monitor.send_notification(f"gcovデータ解析エラー: {e}", "warning")
            return self.parse_gcov_files_manually(environment)
    
    def parse_gcov_files_manually(self, environment: str) -> dict:
        """
        手動でgcovファイルを解析
        Args:
            environment: 環境名
        Returns:
            解析されたカバレッジデータ
        """
        try:
            # gcovrを使用して簡易的なカバレッジ解析（プロジェクトルートから実行）
            result = subprocess.run([
                'gcovr',
                '--print-summary',
                '--root', '.',
                '--exclude', 'test/',
                '--exclude', 'lib/Arduino.*',
                '--exclude', 'lib/M5Stack.*',
                '--exclude', 'lib/WiFi.*',
                '--exclude', 'lib/NTPClient.*',
                '--exclude', 'coverage_reports/',
                '--exclude', '.pio/libdeps/',
                '--gcov-ignore-errors=no_working_dir_found',
                '--gcov-ignore-errors=source_not_found'
            ], capture_output=True, text=True, check=True)
            
            # 出力からカバレッジ率を抽出
            lines = result.stdout.split('\n')
            coverage_percentage = 0.0
            
            for line in lines:
                if 'TOTAL' in line and '%' in line:
                    try:
                        # 行からパーセンテージを抽出
                        parts = line.split()
                        for part in parts:
                            if part.endswith('%'):
                                coverage_percentage = float(part[:-1])
                                break
                    except (ValueError, IndexError):
                        continue
            
            return {
                'total_coverage': coverage_percentage,
                'line_coverage': {
                    'total': 0,
                    'covered': 0,
                    'percentage': coverage_percentage
                },
                'branch_coverage': {
                    'total': 0,
                    'covered': 0,
                    'percentage': 0.0
                },
                'function_coverage': {
                    'total': 0,
                    'covered': 0,
                    'percentage': 0.0
                },
                'files': []
            }
        except subprocess.CalledProcessError as e:
            self.monitor.send_notification(f"手動gcov解析失敗: {e.stderr}", "warning")
            return {
                'total_coverage': 0.0,
                'line_coverage': {'total': 0, 'covered': 0, 'percentage': 0.0},
                'branch_coverage': {'total': 0, 'covered': 0, 'percentage': 0.0},
                'function_coverage': {'total': 0, 'covered': 0, 'percentage': 0.0},
                'files': []
            }
        except Exception as e:
            self.monitor.send_notification(f"手動gcov解析エラー: {e}", "warning")
            return {
                'total_coverage': 0.0,
                'line_coverage': {'total': 0, 'covered': 0, 'percentage': 0.0},
                'branch_coverage': {'total': 0, 'covered': 0, 'percentage': 0.0},
                'function_coverage': {'total': 0, 'covered': 0, 'percentage': 0.0},
                'files': []
            }
    
    def calculate_integrated_coverage(self) -> dict:
        """
        統合カバレッジ計算
        Returns:
            統合カバレッジデータ
        """
        # 純粋ロジックカバレッジがない場合はエラー
        if not self.pure_coverage:
            raise CoverageMeasurementError("純粋ロジックカバレッジデータが不足")
        
        # M5Stack依存実装カバレッジがない場合は純粋ロジックのみで計算
        if not self.m5stack_coverage:
            self.monitor.send_notification("M5Stack依存実装カバレッジデータなし、純粋ロジックのみで統合計算", "warning")
            
            integrated_coverage = {
                'environment': 'integrated',
                'coverage_percentage': self.pure_coverage['coverage_percentage'],
                'line_coverage': self.pure_coverage['line_coverage'],
                'branch_coverage': self.pure_coverage['branch_coverage'],
                'function_coverage': self.pure_coverage['function_coverage'],
                'files': self.pure_coverage['files']
            }
        else:
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
            
            integrated_coverage = {
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
        
        # インスタンス変数に設定
        self.integration_coverage = integrated_coverage
        
        return integrated_coverage
    
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
        
        # 厳格モードの場合は閾値を85%に設定
        base_threshold = self.config.get('quality_gate', {}).get('threshold', 80.0)
        strict_mode = self.config.get('quality_gate', {}).get('strict_mode', False)
        threshold = 85.0 if strict_mode else base_threshold
        
        coverage = self.integration_coverage['coverage_percentage']
        
        passed = coverage >= threshold
        
        mode_text = "厳格モード" if strict_mode else "通常モード"
        result = {
            'passed': passed,
            'threshold': threshold,
            'actual_coverage': coverage,
            'strict_mode': strict_mode,
            'message': f"カバレッジ {coverage:.1f}% {'達成' if passed else '未達成'} (閾値: {threshold}%, {mode_text})"
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
            
            # レポート生成を試行（エラーが発生しても続行）
            try:
                self.report_generator.generate_html_report(self.integration_coverage)
            except Exception as e:
                self.monitor.send_notification(f"HTMLレポート生成失敗: {e}", "warning")
            
            try:
                self.report_generator.generate_xml_report(self.integration_coverage)
            except Exception as e:
                self.monitor.send_notification(f"XMLレポート生成失敗: {e}", "warning")
            
            try:
                self.report_generator.generate_json_report(self.integration_coverage)
            except Exception as e:
                self.monitor.send_notification(f"JSONレポート生成失敗: {e}", "warning")
            
            self.monitor.send_notification("レポート生成完了", "info")
        except Exception as e:
            self.monitor.send_notification(f"レポート生成エラー: {e}", "warning")

    def get_report_stats(self) -> dict:
        """
        レポートディレクトリの統計情報を取得
        Returns:
            統計情報
        """
        try:
            output_dir = self.config.get('reports', {}).get('output_dir', 'coverage_reports')
            if not os.path.exists(output_dir):
                return {
                    'total_files': 0,
                    'total_size_mb': 0,
                    'html_files': 0,
                    'xml_files': 0,
                    'json_files': 0,
                    'oldest_file_days': 0,
                    'newest_file_days': 0
                }
            
            files = []
            total_size = 0
            html_count = 0
            xml_count = 0
            json_count = 0
            
            now = datetime.now()
            
            for filename in os.listdir(output_dir):
                file_path = os.path.join(output_dir, filename)
                if os.path.isfile(file_path):
                    stat = os.stat(file_path)
                    created_time = datetime.fromtimestamp(stat.st_ctime)
                    age_days = (now - created_time).days
                    
                    files.append({
                        'filename': filename,
                        'size': stat.st_size,
                        'age_days': age_days
                    })
                    
                    total_size += stat.st_size
                    
                    if filename.endswith('.html'):
                        html_count += 1
                    elif filename.endswith('.xml'):
                        xml_count += 1
                    elif filename.endswith('.json'):
                        json_count += 1
            
            if files:
                oldest_days = max(f['age_days'] for f in files)
                newest_days = min(f['age_days'] for f in files)
            else:
                oldest_days = 0
                newest_days = 0
            
            return {
                'total_files': len(files),
                'total_size_mb': total_size / (1024 * 1024),
                'html_files': html_count,
                'xml_files': xml_count,
                'json_files': json_count,
                'oldest_file_days': oldest_days,
                'newest_file_days': newest_days
            }
            
        except Exception as e:
            self.monitor.send_notification(f"統計情報取得エラー: {e}", "warning")
            return {}
    
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
    
    def clean_old_reports(self, keep_days: int = None, max_files: int = None):
        """
        古いレポートファイルを削除
        Args:
            keep_days: 保持する日数（Noneの場合は設定ファイルから取得）
            max_files: 最大保持ファイル数（Noneの場合は設定ファイルから取得）
        """
        try:
            output_dir = self.config.get('reports', {}).get('output_dir', 'coverage_reports')
            if not os.path.exists(output_dir):
                self.monitor.send_notification(f"レポートディレクトリが存在しません: {output_dir}", "info")
                return
            
            # 設定から値を取得（CLI引数が優先）
            if keep_days is None:
                keep_days = self.config.get('cleanup', {}).get('keep_days', 7)
            if max_files is None:
                max_files = self.config.get('cleanup', {}).get('max_files', 50)
            
            self.monitor.send_notification(f"クリーンアップ設定: keep_days={keep_days}, max_files={max_files}", "info")
            
            # 現在時刻
            now = datetime.now()
            
            # レポートファイルを取得
            report_files = []
            for filename in os.listdir(output_dir):
                file_path = os.path.join(output_dir, filename)
                if os.path.isfile(file_path):
                    # ファイルの作成時刻を取得
                    stat = os.stat(file_path)
                    created_time = datetime.fromtimestamp(stat.st_ctime)
                    age_days = (now - created_time).days
                    
                    report_files.append({
                        'path': file_path,
                        'filename': filename,
                        'created_time': created_time,
                        'age_days': age_days,
                        'size': stat.st_size
                    })
            
            # 作成時刻でソート（古い順）
            report_files.sort(key=lambda x: x['created_time'])
            
            deleted_count = 0
            deleted_size = 0
            
            # 日数ベースの削除
            for file_info in report_files:
                # keep_days=0の場合は全てのファイルを削除対象にする
                should_delete = False
                if keep_days == 0:
                    should_delete = True
                else:
                    should_delete = file_info['age_days'] > keep_days
                
                if should_delete:
                    try:
                        os.remove(file_info['path'])
                        deleted_count += 1
                        deleted_size += file_info['size']
                        self.monitor.send_notification(f"古いファイルを削除: {file_info['filename']} ({file_info['age_days']}日前)", "info")
                    except Exception as e:
                        self.monitor.send_notification(f"ファイル削除エラー: {file_info['filename']} - {e}", "warning")
            
            # ファイル数ベースの削除（日数削除後）
            remaining_files = [f for f in report_files if os.path.exists(f['path'])]
            if max_files > 0 and len(remaining_files) > max_files:
                files_to_delete = remaining_files[:-max_files]  # 古いファイルから削除
                for file_info in files_to_delete:
                    try:
                        os.remove(file_info['path'])
                        deleted_count += 1
                        deleted_size += file_info['size']
                        self.monitor.send_notification(f"ファイル数制限で削除: {file_info['filename']}", "info")
                    except Exception as e:
                        self.monitor.send_notification(f"ファイル削除エラー: {file_info['filename']} - {e}", "warning")
            
            # 結果を表示
            if deleted_count > 0:
                size_mb = deleted_size / (1024 * 1024)
                self.monitor.send_notification(f"クリーンアップ完了: {deleted_count}ファイル削除, {size_mb:.1f}MB解放", "info")
            else:
                self.monitor.send_notification("削除対象のファイルはありませんでした", "info")
                
        except Exception as e:
            self.monitor.send_notification(f"クリーンアップエラー: {e}", "error")
    
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
                'pure_coverage': coverage_data['pure_coverage']['coverage_percentage'] if coverage_data.get('pure_coverage') else 0.0,
                'm5stack_coverage': coverage_data['m5stack_coverage']['coverage_percentage'] if coverage_data.get('m5stack_coverage') else 0.0,
                'quality_gate': coverage_data['quality_gate']['passed']
            }
            
            history.append(entry)
            
            # 最新N件のみ保持
            max_entries = self.config.get('history', {}).get('max_entries', 100)
            if len(history) > max_entries:
                history = history[-max_entries:]
            
            self.save_history(history)
            self.monitor.send_notification("カバレッジ履歴を記録しました", "info")
        except Exception as e:
            self.monitor.send_notification(f"履歴記録エラー: {e}", "warning")

class CoverageReportGenerator:
    """カバレッジレポート生成クラス"""
    
    def __init__(self, config: dict):
        """
        初期化
        Args:
            config: 設定データ
        """
        self.config = config
        self.output_dir = self.config.get('reports', {}).get('output_dir', 'coverage_reports')
    
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
                '--exclude', 'lib/NTPClient.*',
                '--exclude', '.pio/libdeps/',
                '--gcov-ignore-errors=no_working_dir_found',
                '--gcov-ignore-errors=source_not_found'
            ], check=True)
            
            return html_file
        except subprocess.CalledProcessError as e:
            raise ReportError(f"HTMLレポート生成失敗: {e}")
        except Exception as e:
            raise ReportError(f"HTMLレポート生成エラー: {e}")
    
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
                '--exclude', 'lib/NTPClient.*',
                '--exclude', '.pio/libdeps/',
                '--gcov-ignore-errors=no_working_dir_found',
                '--gcov-ignore-errors=source_not_found'
            ], check=True)
            
            return xml_file
        except subprocess.CalledProcessError as e:
            raise ReportError(f"XMLレポート生成失敗: {e}")
        except Exception as e:
            raise ReportError(f"XMLレポート生成エラー: {e}")
    
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
                'pure_coverage': coverage_data['pure_coverage']['coverage_percentage'] if coverage_data.get('pure_coverage') else 0.0,
                'm5stack_coverage': coverage_data['m5stack_coverage']['coverage_percentage'] if coverage_data.get('m5stack_coverage') else 0.0,
                'quality_gate': coverage_data['quality_gate']['passed']
            }
            
            history.append(entry)
            
            # 最新N件のみ保持
            max_entries = self.config.get('history', {}).get('max_entries', 100)
            if len(history) > max_entries:
                history = history[-max_entries:]
            
            self.save_history(history)
            self.monitor.send_notification("カバレッジ履歴を記録しました", "info")
        except Exception as e:
            self.monitor.send_notification(f"履歴記録エラー: {e}", "warning")
    
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

def main():
    """メイン関数"""
    
    parser = argparse.ArgumentParser(description="Aimatix カバレッジ計測システム")
    parser.add_argument("--quick", action="store_true", help="クイックモード（開発中）")
    parser.add_argument("--full", action="store_true", help="フルモード（機能完成時）")
    parser.add_argument("--release", action="store_true", help="リリースモード（リリース前）")
    parser.add_argument("--baseline", action="store_true", help="ベースライン計測")
    parser.add_argument("--check-env", action="store_true", help="環境設定確認")
    parser.add_argument("--config", default="coverage_config.json", help="設定ファイルパス")
    parser.add_argument("--verbose", action="store_true", help="詳細出力")
    parser.add_argument("--environment", choices=["native", "unit-test-esp32"], help="特定環境のカバレッジ計測")
    parser.add_argument("--report-only", action="store_true", help="レポート生成のみ")
    parser.add_argument("--quality-gate-only", action="store_true", help="品質ゲートチェックのみ")
    parser.add_argument("--output-dir", default="coverage_reports", help="出力ディレクトリ")
    parser.add_argument("--debug", action="store_true", help="デバッグモード")
    parser.add_argument("--clean", action="store_true", help="古いレポートファイル削除")
    parser.add_argument("--stats", action="store_true", help="レポートディレクトリの統計情報表示")
    parser.add_argument("--keep-days", type=int, help="保持する日数（--cleanと併用）")
    parser.add_argument("--max-files", type=int, help="最大保持ファイル数（--cleanと併用）")
    
    args = parser.parse_args()
    
    try:
        # システム初期化
        system = CoverageMeasurementSystem(args.config)
        
        # 実行モードの決定
        if args.check_env:
            print("環境確認開始...")
            result = system.check_environment()
            print("環境確認完了")
            return
        
        if args.baseline:
            print("ベースライン計測開始...")
            result = system.run_baseline_measurement()
        elif args.quick:
            print("クイックモード開始...")
            result = system.run_quick_coverage_measurement()
        elif args.full:
            print("フルモード開始...")
            result = system.run_full_coverage_measurement()
        elif args.release:
            print("リリースモード開始...")
            result = system.run_release_coverage_measurement()
        elif args.environment:
            print(f"{args.environment}環境のカバレッジ計測開始...")
            if args.environment == "native":
                result = system.measure_pure_logic_coverage()
            else:
                result = system.measure_m5stack_coverage()
        elif args.report_only:
            print("レポート生成のみ実行...")
            system.generate_reports()
            print("レポート生成完了")
            return
        elif args.quality_gate_only:
            print("品質ゲートチェックのみ実行...")
            if not system.integration_coverage:
                print("エラー: カバレッジデータが不足しています。先にカバレッジ計測を実行してください。")
                sys.exit(1)
            result = system.check_quality_gate()
            print(f"品質ゲート結果: {result['message']}")
            return
        elif args.clean:
            print("古いレポートファイルの削除を開始します...")
            keep_days = args.keep_days if args.keep_days is not None else None
            max_files = args.max_files if args.max_files is not None else None
            system.clean_old_reports(keep_days=keep_days, max_files=max_files)
            print("古いレポートファイルの削除が完了しました。")
            return
        elif args.stats:
            print("レポートディレクトリの統計情報を表示します...")
            stats = system.get_report_stats()
            print(f"合計ファイル数: {stats['total_files']}")
            print(f"合計サイズ: {stats['total_size_mb']:.2f}MB")
            print(f"HTMLファイル数: {stats['html_files']}")
            print(f"XMLファイル数: {stats['xml_files']}")
            print(f"JSONファイル数: {stats['json_files']}")
            print(f"最古ファイル: {stats['oldest_file_days']}日前")
            print(f"最新ファイル: {stats['newest_file_days']}日前")
            print("統計情報表示が完了しました。")
            return
        else:
            # デフォルトはクイックモード
            print("クイックモード開始...")
            result = system.run_quick_coverage_measurement()
        
        # 結果出力
        print("\n" + "="*60)
        print("カバレッジ計測結果")
        print("="*60)
        
        if result.get('pure_coverage'):
            pure_cov = result['pure_coverage']
            print(f"純粋ロジックカバレッジ: {pure_cov['coverage_percentage']:.1f}%")
            print(f"  - 行カバレッジ: {pure_cov['line_coverage']['covered']}/{pure_cov['line_coverage']['total']} ({pure_cov['line_coverage']['percentage']:.1f}%)")
            print(f"  - 分岐カバレッジ: {pure_cov['branch_coverage']['covered']}/{pure_cov['branch_coverage']['total']} ({pure_cov['branch_coverage']['percentage']:.1f}%)")
            print(f"  - 関数カバレッジ: {pure_cov['function_coverage']['covered']}/{pure_cov['function_coverage']['total']} ({pure_cov['function_coverage']['percentage']:.1f}%)")
        
        if result.get('m5stack_coverage'):
            m5stack_cov = result['m5stack_coverage']
            print(f"M5Stack依存実装カバレッジ: {m5stack_cov['coverage_percentage']:.1f}%")
            print(f"  - 行カバレッジ: {m5stack_cov['line_coverage']['covered']}/{m5stack_cov['line_coverage']['total']} ({m5stack_cov['line_coverage']['percentage']:.1f}%)")
            print(f"  - 分岐カバレッジ: {m5stack_cov['branch_coverage']['covered']}/{m5stack_cov['branch_coverage']['total']} ({m5stack_cov['branch_coverage']['percentage']:.1f}%)")
            print(f"  - 関数カバレッジ: {m5stack_cov['function_coverage']['covered']}/{m5stack_cov['function_coverage']['total']} ({m5stack_cov['function_coverage']['percentage']:.1f}%)")
        
        if result.get('integration_coverage'):
            int_cov = result['integration_coverage']
            print(f"統合カバレッジ: {int_cov['coverage_percentage']:.1f}%")
            print(f"  - 行カバレッジ: {int_cov['line_coverage']['covered']}/{int_cov['line_coverage']['total']} ({int_cov['line_coverage']['percentage']:.1f}%)")
            print(f"  - 分岐カバレッジ: {int_cov['branch_coverage']['covered']}/{int_cov['branch_coverage']['total']} ({int_cov['branch_coverage']['percentage']:.1f}%)")
            print(f"  - 関数カバレッジ: {int_cov['function_coverage']['covered']}/{int_cov['function_coverage']['total']} ({int_cov['function_coverage']['percentage']:.1f}%)")
        
        if result.get('quality_gate'):
            qg = result['quality_gate']
            status = "✅ 通過" if qg['passed'] else "❌ 未通過"
            print(f"\n品質ゲート: {status}")
            print(f"  - 閾値: {qg['threshold']}%")
            print(f"  - 実際: {qg['actual_coverage']:.1f}%")
            print(f"  - メッセージ: {qg['message']}")
        
        print("\n" + "="*60)
        print("カバレッジ計測完了")
        print("="*60)
        
        # 品質ゲート未通過の場合はエラー終了
        if result.get('quality_gate') and not result['quality_gate']['passed']:
            if args.release or system.config.get('quality_gate', {}).get('strict_mode', False):
                print("エラー: 品質ゲート未通過のため、処理を終了します。")
                sys.exit(1)
            else:
                print("警告: 品質ゲート未通過ですが、処理を継続します。")
        
    except CoverageMeasurementError as e:
        print(f"カバレッジ計測エラー: {e}")
        sys.exit(1)
    except KeyboardInterrupt:
        print("\nユーザーによって中断されました。")
        sys.exit(1)
    except Exception as e:
        print(f"予期しないエラー: {e}")
        if args.debug:
            import traceback
            traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main() 