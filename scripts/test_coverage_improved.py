#!/usr/bin/env python3
"""
M5Stack集中タイマー テストカバレッジ測定スクリプト（Phase 0.7改善版）

使用方法:
    python scripts/test_coverage_improved.py [test_name]

機能:
    - gcovを使用したカバレッジ測定
    - カバレッジ結果の可視化
    - カバレッジレポートの生成
    - エラーハンドリングの強化
    - 80%目標との詳細比較
"""

import os
import sys
import subprocess
import json
import re
import time
from pathlib import Path
from datetime import datetime
from typing import List, Dict, Optional, Tuple

class CoverageResult:
    """カバレッジ結果を管理するクラス"""
    def __init__(self, filename: str, lines_total: int, lines_covered: int, 
                 functions_total: int, functions_covered: int):
        self.filename = filename
        self.lines_total = lines_total
        self.lines_covered = lines_covered
        self.functions_total = functions_total
        self.functions_covered = functions_covered
        
    @property
    def line_coverage(self) -> float:
        return (self.lines_covered / self.lines_total * 100) if self.lines_total > 0 else 0
    
    @property
    def function_coverage(self) -> float:
        return (self.functions_covered / self.functions_total * 100) if self.functions_total > 0 else 0
    
    @property
    def uncovered_lines(self) -> int:
        return self.lines_total - self.lines_covered
    
    @property
    def uncovered_functions(self) -> int:
        return self.functions_total - self.functions_covered

class TestExecutionResult:
    """テスト実行結果を管理するクラス"""
    def __init__(self, test_name: str, success: bool, execution_time: float, 
                 error_message: str = ""):
        self.test_name = test_name
        self.success = success
        self.execution_time = execution_time
        self.error_message = error_message

class CoverageAnalyzer:
    """カバレッジ分析クラス"""
    
    def __init__(self, target_coverage: float = 80.0):
        self.target_coverage = target_coverage
        self.coverage_results: List[CoverageResult] = []
        self.test_results: List[TestExecutionResult] = []
        
    def get_compiler(self) -> Optional[str]:
        """利用可能なコンパイラを取得"""
        for compiler in ['g++', 'clang++']:
            try:
                result = subprocess.run([compiler, '--version'], 
                                      capture_output=True, text=True, timeout=10)
                if result.returncode == 0:
                    print(f"使用コンパイラ: {compiler}")
                    return compiler
            except (FileNotFoundError, subprocess.TimeoutExpired):
                continue
        
        print("エラー: g++またはclang++が見つかりません")
        return None
    
    def compile_with_coverage(self, test_file: str, test_name: str) -> bool:
        """カバレッジ測定用にコンパイル"""
        unity_path = Path(".pio/libdeps/native/Unity/src")
        if not unity_path.exists():
            print("エラー: Unityライブラリが見つかりません")
            return False
        
        compiler = self.get_compiler()
        if not compiler:
            return False
        
        test_path = Path("test") / test_file
        if not test_path.exists():
            print(f"エラー: テストファイルが見つかりません: {test_path}")
            return False
        
        output_name = f"test_{test_name}_coverage"
        
        # Unityライブラリのソースファイルを探す
        unity_src = None
        for unity_file in ["unity.c", "unity.cpp"]:
            unity_src_path = unity_path / unity_file
            if unity_src_path.exists():
                unity_src = str(unity_src_path)
                break
        
        if not unity_src:
            print(f"エラー: Unityソースファイルが見つかりません")
            return False
        
        # カバレッジ測定用のコンパイルオプション
        coverage_flags = "-fprofile-arcs -ftest-coverage -g -O0"
        compile_cmd = [
            compiler, coverage_flags, 
            f"-I{unity_path}", "-Ilib", "-Itest/mocks", "-Isrc", 
            "-std=c++11", "-o", output_name, 
            str(test_path), unity_src, 
            "src/time_logic.cpp", "test/mocks/mock_m5stack.cpp"
        ]
        
        print(f"コンパイル中: {' '.join(compile_cmd)}")
        
        try:
            result = subprocess.run(compile_cmd, capture_output=True, text=True, timeout=60)
            
            if result.returncode != 0:
                print(f"コンパイルエラー: {result.stderr}")
                return False
            
            print("コンパイル成功")
            return True
            
        except subprocess.TimeoutExpired:
            print("コンパイルがタイムアウトしました")
            return False
        except Exception as e:
            print(f"コンパイル中にエラーが発生しました: {e}")
            return False
    
    def run_test_with_coverage(self, test_name: str) -> TestExecutionResult:
        """カバレッジ測定付きでテストを実行"""
        output_name = f"test_{test_name}_coverage"
        
        if os.name == 'nt':
            run_cmd = f'{output_name}.exe'
        else:
            run_cmd = f'./{output_name}'
        
        print(f"テスト実行中: {run_cmd}")
        
        start_time = time.time()
        
        try:
            result = subprocess.run(run_cmd, shell=True, capture_output=True, 
                                  text=True, encoding='utf-8', errors='replace', timeout=30)
            
            execution_time = time.time() - start_time
            
            success = result.returncode == 0
            error_message = result.stderr if not success else ""
            
            return TestExecutionResult(test_name, success, execution_time, error_message)
            
        except subprocess.TimeoutExpired:
            execution_time = time.time() - start_time
            return TestExecutionResult(test_name, False, execution_time, "テストがタイムアウトしました")
        except Exception as e:
            execution_time = time.time() - start_time
            return TestExecutionResult(test_name, False, execution_time, f"テスト実行中にエラーが発生しました: {e}")
    
    def parse_gcov_file(self, gcov_file: Path) -> Optional[CoverageResult]:
        """gcovファイルを解析してカバレッジ情報を取得"""
        if not gcov_file.exists():
            return None
        
        try:
            with open(gcov_file, 'r', encoding='utf-8') as f:
                content = f.read()
            
            # ファイル名を抽出（Source: ... に対応）
            file_match = re.search(r'Source:(.+)', content)
            if not file_match:
                return None
            
            filename = file_match.group(1).strip()
            
            # 行カバレッジを解析
            lines_total = 0
            lines_covered = 0
            functions_total = 0
            functions_covered = 0
            
            for line in content.split('\n'):
                # 関数カバレッジ
                if 'function' in line and 'called' in line:
                    functions_total += 1
                    if '1' in line.split()[0]:
                        functions_covered += 1
                
                # 行カバレッジ
                if re.match(r'^\s*\d+:', line):
                    lines_total += 1
                    if line.startswith('1:'):
                        lines_covered += 1
            
            return CoverageResult(filename, lines_total, lines_covered, functions_total, functions_covered)
        
        except Exception as e:
            print(f"gcovファイル解析エラー: {e}")
            return None
    
    def collect_coverage_data(self) -> List[CoverageResult]:
        """カバレッジデータを収集"""
        coverage_results = []
        
        # .gcdaファイルを探す
        for gcda_file in Path(".").glob("*.gcda"):
            gcov_file = gcda_file.with_suffix(".gcov")
            
            # gcovファイルを生成
            try:
                subprocess.run(["gcov", str(gcda_file)], capture_output=True, timeout=30)
            except (subprocess.TimeoutExpired, FileNotFoundError):
                continue
            
            # gcovファイルを解析
            result = self.parse_gcov_file(gcov_file)
            if result:
                coverage_results.append(result)
        
        return coverage_results
    
    def generate_detailed_report(self) -> Dict:
        """詳細なカバレッジレポートを生成"""
        if not self.coverage_results:
            return {"error": "カバレッジデータが見つかりません"}
        
        total_lines = sum(r.lines_total for r in self.coverage_results)
        covered_lines = sum(r.lines_covered for r in self.coverage_results)
        total_functions = sum(r.functions_total for r in self.coverage_results)
        covered_functions = sum(r.functions_covered for r in self.coverage_results)
        
        overall_line_coverage = (covered_lines / total_lines * 100) if total_lines > 0 else 0
        overall_function_coverage = (covered_functions / total_functions * 100) if total_functions > 0 else 0
        
        # 目標達成状況の分析
        target_achieved = overall_line_coverage >= self.target_coverage
        gap_to_target = self.target_coverage - overall_line_coverage if not target_achieved else 0
        
        # 改善が必要なファイルの特定
        files_needing_improvement = [
            r for r in self.coverage_results 
            if r.line_coverage < self.target_coverage
        ]
        
        report = {
            "timestamp": datetime.now().isoformat(),
            "overall": {
                "line_coverage": overall_line_coverage,
                "function_coverage": overall_function_coverage,
                "total_lines": total_lines,
                "covered_lines": covered_lines,
                "total_functions": total_functions,
                "covered_functions": covered_functions,
                "target_coverage": self.target_coverage,
                "target_achieved": target_achieved,
                "gap_to_target": gap_to_target
            },
            "files": [
                {
                    "filename": r.filename,
                    "line_coverage": r.line_coverage,
                    "function_coverage": r.function_coverage,
                    "lines_total": r.lines_total,
                    "lines_covered": r.lines_covered,
                    "uncovered_lines": r.uncovered_lines,
                    "functions_total": r.functions_total,
                    "functions_covered": r.functions_covered,
                    "uncovered_functions": r.uncovered_functions,
                    "needs_improvement": r.line_coverage < self.target_coverage
                }
                for r in self.coverage_results
            ],
            "improvement_needed": [
                {
                    "filename": r.filename,
                    "current_coverage": r.line_coverage,
                    "gap": self.target_coverage - r.line_coverage,
                    "uncovered_lines": r.uncovered_lines
                }
                for r in files_needing_improvement
            ]
        }
        
        return report
    
    def print_report(self, report: Dict):
        """カバレッジレポートを表示"""
        if "error" in report:
            print(f"エラー: {report['error']}")
            return
        
        overall = report["overall"]
        
        print("\n" + "=" * 80)
        print("📊 テストカバレッジレポート（Phase 0.7改善版）")
        print("=" * 80)
        print(f"実行日時: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        print()
        
        # 全体サマリー
        print("全体サマリー:")
        print(f"  行カバレッジ: {overall['line_coverage']:.1f}% ({overall['covered_lines']}/{overall['total_lines']})")
        print(f"  関数カバレッジ: {overall['function_coverage']:.1f}% ({overall['covered_functions']}/{overall['total_functions']})")
        print(f"  目標カバレッジ: {overall['target_coverage']:.1f}%")
        
        if overall['target_achieved']:
            print(f"  🎉 カバレッジ目標を達成しました！")
        else:
            print(f"  ⚠️  目標まであと {overall['gap_to_target']:.1f}% 不足しています")
        
        print()
        
        # ファイル別詳細
        print("ファイル別詳細:")
        for file_info in report["files"]:
            status = "✅" if file_info["needs_improvement"] == False else "⚠️" if file_info["line_coverage"] >= 50 else "❌"
            print(f"  {status} {file_info['filename']}")
            print(f"    行カバレッジ: {file_info['line_coverage']:.1f}% ({file_info['lines_covered']}/{file_info['lines_total']})")
            print(f"    関数カバレッジ: {file_info['function_coverage']:.1f}% ({file_info['functions_covered']}/{file_info['functions_total']})")
            if file_info["uncovered_lines"] > 0:
                print(f"    未カバー行数: {file_info['uncovered_lines']}")
            print()
        
        # 改善が必要なファイルの詳細
        if report["improvement_needed"]:
            print("改善が必要なファイル:")
            for file_info in report["improvement_needed"]:
                print(f"  📝 {file_info['filename']}")
                print(f"    現在のカバレッジ: {file_info['current_coverage']:.1f}%")
                print(f"    目標までの不足: {file_info['gap']:.1f}%")
                print(f"    未カバー行数: {file_info['uncovered_lines']}")
                print()
        
        print("=" * 80)
    
    def save_report(self, report: Dict, filename: str = "coverage_report_improved.json"):
        """レポートをJSONファイルに保存"""
        try:
            with open(filename, 'w', encoding='utf-8') as f:
                json.dump(report, f, indent=2, ensure_ascii=False)
            print(f"レポートを保存しました: {filename}")
        except Exception as e:
            print(f"レポート保存エラー: {e}")
    
    def cleanup_files(self):
        """一時ファイルをクリーンアップ"""
        patterns = ["*.gcda", "*.gcno", "*.gcov", "test_*_coverage*"]
        
        for pattern in patterns:
            for file_path in Path(".").glob(pattern):
                try:
                    file_path.unlink()
                except Exception:
                    pass

def main():
    """メイン関数"""
    if len(sys.argv) < 2:
        print("使用方法: python scripts/test_coverage_improved.py <test_name>")
        print("例: python scripts/test_coverage_improved.py time_logic_simple")
        return
    
    test_name = sys.argv[1]
    test_file = f"{test_name}.cpp"
    
    print(f"=== カバレッジ測定開始（Phase 0.7改善版）===")
    print(f"テスト: {test_name}")
    print()
    
    analyzer = CoverageAnalyzer(target_coverage=80.0)
    
    # 1. コンパイル
    if not analyzer.compile_with_coverage(test_file, test_name):
        print("コンパイルに失敗しました")
        return
    
    # 2. テスト実行
    test_result = analyzer.run_test_with_coverage(test_name)
    analyzer.test_results.append(test_result)
    
    if not test_result.success:
        print(f"テスト実行に失敗しました: {test_result.error_message}")
        return
    
    print(f"テスト実行成功 (実行時間: {test_result.execution_time:.2f}秒)")
    
    # 3. カバレッジデータ収集
    print("カバレッジデータを収集中...")
    analyzer.coverage_results = analyzer.collect_coverage_data()
    
    if not analyzer.coverage_results:
        print("カバレッジデータが見つかりませんでした")
        return
    
    # 4. レポート生成
    report = analyzer.generate_detailed_report()
    analyzer.print_report(report)
    
    # 5. レポート保存
    analyzer.save_report(report)
    
    # 6. クリーンアップ
    analyzer.cleanup_files()
    
    print("=== カバレッジ測定完了 ===")

if __name__ == "__main__":
    main() 