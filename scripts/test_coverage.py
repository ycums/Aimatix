#!/usr/bin/env python3
"""
M5Stack集中タイマー テストカバレッジ測定スクリプト（Phase 0.4）

使用方法:
    python scripts/test_coverage.py [test_name]

機能:
    - gcovを使用したカバレッジ測定
    - カバレッジ結果の可視化
    - カバレッジレポートの生成
"""

import os
import sys
import subprocess
import json
import re
from pathlib import Path
from datetime import datetime

class CoverageResult:
    """カバレッジ結果を管理するクラス"""
    def __init__(self, filename, lines_total, lines_covered, functions_total, functions_covered):
        self.filename = filename
        self.lines_total = lines_total
        self.lines_covered = lines_covered
        self.functions_total = functions_total
        self.functions_covered = functions_covered
        
    @property
    def line_coverage(self):
        return (self.lines_covered / self.lines_total * 100) if self.lines_total > 0 else 0
    
    @property
    def function_coverage(self):
        return (self.functions_covered / self.functions_total * 100) if self.functions_total > 0 else 0

def get_compiler():
    """利用可能なコンパイラを取得"""
    for compiler in ['g++', 'clang++']:
        try:
            result = subprocess.run([compiler, '--version'], 
                                  capture_output=True, text=True)
            if result.returncode == 0:
                print(f"使用コンパイラ: {compiler}")
                return compiler
        except FileNotFoundError:
            continue
    
    print("エラー: g++またはclang++が見つかりません")
    return None

def compile_with_coverage(test_file, test_name):
    """カバレッジ測定用にコンパイル"""
    unity_path = Path(".pio/libdeps/native/Unity/src")
    if not unity_path.exists():
        print("エラー: Unityライブラリが見つかりません")
        return False
    
    compiler = get_compiler()
    if not compiler:
        return False
    
    test_path = Path("test") / test_file
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
    compile_cmd = f"{compiler} {coverage_flags} -I{unity_path} -Ilib -Itest/mocks -Isrc -std=c++11 -o {output_name} {test_path} {unity_src} src/time_logic.cpp test/mocks/mock_m5stack.cpp"
    
    print(f"コンパイル中: {compile_cmd}")
    result = subprocess.run(compile_cmd, shell=True, capture_output=True, text=True)
    
    if result.returncode != 0:
        print(f"コンパイルエラー: {result.stderr}")
        return False
    
    return True

def run_test_with_coverage(test_name):
    """カバレッジ測定付きでテストを実行"""
    output_name = f"test_{test_name}_coverage"
    
    if os.name == 'nt':
        run_cmd = f'{output_name}.exe'
    else:
        run_cmd = f'./{output_name}'
    
    print(f"テスト実行中: {run_cmd}")
    result = subprocess.run(run_cmd, shell=True, capture_output=True, text=True, encoding='utf-8', errors='replace')
    
    return result.returncode == 0

def parse_gcov_file(gcov_file):
    """gcovファイルを解析してカバレッジ情報を取得"""
    coverage_data = {}
    
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

def generate_coverage_report(coverage_results):
    """カバレッジレポートを生成"""
    if not coverage_results:
        print("カバレッジデータが見つかりません")
        return
    
    total_lines = sum(r.lines_total for r in coverage_results)
    covered_lines = sum(r.lines_covered for r in coverage_results)
    total_functions = sum(r.functions_total for r in coverage_results)
    covered_functions = sum(r.functions_covered for r in coverage_results)
    
    overall_line_coverage = (covered_lines / total_lines * 100) if total_lines > 0 else 0
    overall_function_coverage = (covered_functions / total_functions * 100) if total_functions > 0 else 0
    
    print("\n" + "=" * 80)
    print("📊 テストカバレッジレポート")
    print("=" * 80)
    print(f"実行日時: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    print()
    
    # 全体サマリー
    print("全体サマリー:")
    print(f"  行カバレッジ: {overall_line_coverage:.1f}% ({covered_lines}/{total_lines})")
    print(f"  関数カバレッジ: {overall_function_coverage:.1f}% ({covered_functions}/{total_functions})")
    print()
    
    # ファイル別詳細
    print("ファイル別詳細:")
    for result in coverage_results:
        status = "✅" if result.line_coverage >= 80 else "⚠️" if result.line_coverage >= 50 else "❌"
        print(f"  {status} {result.filename}")
        print(f"    行カバレッジ: {result.line_coverage:.1f}% ({result.lines_covered}/{result.lines_total})")
        print(f"    関数カバレッジ: {result.function_coverage:.1f}% ({result.functions_covered}/{result.functions_total})")
        print()
    
    # カバレッジ目標との比較
    target_coverage = 80
    if overall_line_coverage >= target_coverage:
        print(f"🎉 カバレッジ目標({target_coverage}%)を達成しました！")
    else:
        print(f"⚠️  カバレッジ目標({target_coverage}%)に達していません。追加テストが必要です。")
    
    return overall_line_coverage >= target_coverage

def save_coverage_results(coverage_results, filename="coverage_results.json"):
    """カバレッジ結果をJSONファイルに保存"""
    data = {
        "timestamp": datetime.now().isoformat(),
        "files": [
            {
                "filename": r.filename,
                "lines_total": r.lines_total,
                "lines_covered": r.lines_covered,
                "line_coverage": r.line_coverage,
                "functions_total": r.functions_total,
                "functions_covered": r.functions_covered,
                "function_coverage": r.function_coverage
            }
            for r in coverage_results
        ],
        "summary": {
            "total_lines": sum(r.lines_total for r in coverage_results),
            "covered_lines": sum(r.lines_covered for r in coverage_results),
            "total_functions": sum(r.functions_total for r in coverage_results),
            "covered_functions": sum(r.functions_covered for r in coverage_results),
            "overall_line_coverage": (sum(r.lines_covered for r in coverage_results) / sum(r.lines_total for r in coverage_results) * 100) if sum(r.lines_total for r in coverage_results) > 0 else 0,
            "overall_function_coverage": (sum(r.functions_covered for r in coverage_results) / sum(r.functions_total for r in coverage_results) * 100) if sum(r.functions_total for r in coverage_results) > 0 else 0
        }
    }
    
    with open(filename, 'w', encoding='utf-8') as f:
        json.dump(data, f, ensure_ascii=False, indent=2)
    
    print(f"\n📊 カバレッジ結果を {filename} に保存しました")

def cleanup_gcov_files():
    """gcov関連ファイルをクリーンアップ"""
    for ext in ['.gcov', '.gcda', '.gcno']:
        for file in Path('.').glob(f'*{ext}'):
            try:
                file.unlink()
            except:
                pass

def main():
    """メイン関数"""
    if len(sys.argv) < 2:
        print(__doc__)
        return
    
    test_name = sys.argv[1].lower()
    
    # テストファイルのマッピング
    test_files = {
        "time_logic": "test_time_logic_simple.cpp",
        "alarm_logic": "test_alarm_logic_simple.cpp",
        "input_logic": "test_input_logic_simple.cpp",
        "settings_logic": "test_settings_logic_simple.cpp",
        "warning_messages": "test_warning_messages_simple.cpp"
    }
    
    if test_name not in test_files:
        print(f"エラー: 不明なテスト名: {test_name}")
        print("利用可能なテスト:", ", ".join(test_files.keys()))
        return
    
    test_file = test_files[test_name]
    
    print(f"=== {test_name} カバレッジ測定 ===")
    
    # 既存のgcovファイルをクリーンアップ
    cleanup_gcov_files()
    
    # カバレッジ測定用にコンパイル
    if not compile_with_coverage(test_file, test_name):
        print("コンパイルに失敗しました")
        return
    
    # テスト実行
    if not run_test_with_coverage(test_name):
        print("テスト実行に失敗しました")
        return
    
    # gcovコマンドで.gcovファイルを生成
    print("gcovファイル生成中...")
    # 個別のソースファイルに対してgcovを実行
    source_files = ['time_logic', 'mock_m5stack']
    for source_file in source_files:
        subprocess.run(['gcov', f'test_{test_name}_coverage-{source_file}'], shell=True, capture_output=True, text=True)
    
    # gcovファイルを解析
    coverage_results = []
    for gcov_file in Path('.').glob('*.gcov'):
        if str(gcov_file) == 'time_logic.cpp.gcov':
            print('--- time_logic.cpp.gcov の内容 ---')
            with open(gcov_file, 'r', encoding='utf-8') as f:
                for i, line in enumerate(f):
                    print(line.rstrip())
                    if i > 40:
                        break
            print('--- ここまで ---')
        print(f"gcovファイル発見: {gcov_file}")
        result = parse_gcov_file(gcov_file)
        if result:
            coverage_results.append(result)
            print(f"カバレッジデータ解析成功: {result.filename}")
    
    # レポート生成
    success = generate_coverage_report(coverage_results)
    
    # 結果保存
    save_coverage_results(coverage_results)
    
    # クリーンアップ（実行ファイルとgcov関連ファイルを削除）
    try:
        os.remove(f"test_{test_name}_coverage.exe")
    except FileNotFoundError:
        pass
    
    # デバッグ用：gcovファイルの存在確認
    print("クリーンアップ前のgcovファイル:")
    for gcov_file in Path('.').glob('*.gcov'):
        print(f"  {gcov_file}")
    
    cleanup_gcov_files()
    
    if success:
        print("🎉 カバレッジ測定が完了しました！")
        return 0
    else:
        print("⚠️  カバレッジ目標に達していません。")
        return 1

if __name__ == "__main__":
    sys.exit(main()) 