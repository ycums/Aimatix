#!/usr/bin/env python3
"""
M5Stack集中タイマー テスト実行スクリプト（Phase 0.4改善版）

使用方法:
    python scripts/run_tests.py [test_name]

テスト名:
    warning_messages  - 警告メッセージ機能のテスト
    alarm_validation  - アラームバリデーション機能のテスト
    button_manager    - ボタン管理機能のテスト
    simple           - 純粋ロジックテスト（推奨）
    pure             - 新規作成した純粋ロジックテスト
    all              - 全テストを実行
"""

import sys
import subprocess
import os
import json
import time
from pathlib import Path
from datetime import datetime

class TestResult:
    """テスト結果を管理するクラス"""
    def __init__(self, name, success, duration, output, error_output):
        self.name = name
        self.success = success
        self.duration = duration
        self.output = output
        self.error_output = error_output
        self.timestamp = datetime.now()

def run_command(command, description, timeout=60):
    """コマンドを実行し、結果を表示"""
    print(f"\n=== {description} ===")
    print(f"実行コマンド: {command}")
    
    start_time = time.time()
    
    try:
        result = subprocess.run(command, shell=True, capture_output=True, text=True, timeout=timeout, encoding='utf-8', errors='replace')
        duration = time.time() - start_time
        
        if result.returncode == 0:
            print("✅ 成功")
            if result.stdout:
                try:
                    print("出力:")
                    print(result.stdout)
                except UnicodeDecodeError:
                    print("[出力デコードエラー] (一部の文字が表示できません)")
        else:
            print("❌ 失敗")
            if result.stderr:
                try:
                    print("エラー:")
                    print(result.stderr)
                except UnicodeDecodeError:
                    print("[エラーデコードエラー] (一部の文字が表示できません)")
            if result.stdout:
                try:
                    print("出力:")
                    print(result.stdout)
                except UnicodeDecodeError:
                    print("[出力デコードエラー] (一部の文字が表示できません)")
        
        return TestResult(description, result.returncode == 0, duration, result.stdout, result.stderr)
    except subprocess.TimeoutExpired:
        print(f"❌ タイムアウト ({timeout}秒)")
        return TestResult(description, False, timeout, "", "タイムアウト")
    except Exception as e:
        print(f"❌ 実行エラー: {e}")
        return TestResult(description, False, 0, "", str(e))

def run_native_test(test_name):
    """Native環境でテストを実行"""
    command = f"pio test -e native -f {test_name}"
    return run_command(command, f"Native環境での{test_name}テスト")

def run_simple_test():
    """純粋ロジックテストを実行"""
    command = "python scripts/run_simple_tests.py"
    return run_command(command, "純粋ロジックテスト")

def run_pure_logic_tests():
    """新規作成した純粋ロジックテストを実行"""
    tests = [
        ("test_input_logic_pure", "InputLogic純粋ロジック"),
        ("test_settings_logic_pure", "SettingsLogic純粋ロジック"),
        ("test_alarm_logic_pure", "AlarmLogic純粋ロジック"),
        ("test_warning_messages_pure", "WarningMessages純粋ロジック"),
        ("test_button_manager_pure", "ButtonManager純粋ロジック"),
        ("test_debounce_manager_pure", "DebounceManager純粋ロジック")
    ]
    
    results = []
    for test_file, description in tests:
        # mock_m5stack.cppをリンクしない
        command = f"g++ -I.pio/libdeps/native/Unity/src -Ilib -Isrc -Itest -std=c++11 -o {test_file} test/{test_file}.cpp .pio/libdeps/native/Unity/src/unity.c"
        compile_result = run_command(command, f"{description}のコンパイル")
        if compile_result.success:
            run_cmd = f"{test_file}.exe"
            run_result = run_command(run_cmd, f"{description}の実行")
            results.append(run_result)
        else:
            results.append(compile_result)
    
    return results

def save_test_results(results, filename="test_results.json"):
    """テスト結果をJSONファイルに保存"""
    data = {
        "timestamp": datetime.now().isoformat(),
        "total_tests": len(results),
        "successful_tests": sum(1 for r in results if r.success),
        "failed_tests": sum(1 for r in results if not r.success),
        "results": [
            {
                "name": r.name,
                "success": r.success,
                "duration": r.duration,
                "timestamp": r.timestamp.isoformat(),
                "output": r.output,
                "error_output": r.error_output
            }
            for r in results
        ]
    }
    
    with open(filename, 'w', encoding='utf-8') as f:
        json.dump(data, f, ensure_ascii=False, indent=2)
    
    print(f"\n📊 テスト結果を {filename} に保存しました")

def generate_test_report(results):
    """テスト結果レポートを生成"""
    total = len(results)
    successful = sum(1 for r in results if r.success)
    failed = total - successful
    success_rate = (successful / total * 100) if total > 0 else 0
    
    print("\n" + "=" * 60)
    print("📋 テスト実行レポート")
    print("=" * 60)
    print(f"実行日時: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    print(f"総テスト数: {total}")
    print(f"成功: {successful}")
    print(f"失敗: {failed}")
    print(f"成功率: {success_rate:.1f}%")
    print()
    
    # 詳細結果
    print("詳細結果:")
    for result in results:
        status = "✅" if result.success else "❌"
        duration = f"{result.duration:.2f}s"
        print(f"  {status} {result.name} ({duration})")
    
    print()
    
    # 失敗したテストの詳細
    failed_tests = [r for r in results if not r.success]
    if failed_tests:
        print("失敗したテストの詳細:")
        for result in failed_tests:
            print(f"\n❌ {result.name}")
            if result.error_output:
                print(f"  エラー: {result.error_output.strip()}")
    
    return success_rate >= 80  # 80%以上で成功とみなす

def main():
    # プロジェクトルートディレクトリに移動
    project_root = Path(__file__).parent.parent
    os.chdir(project_root)
    
    # コマンドライン引数を取得
    if len(sys.argv) < 2:
        print(__doc__)
        return
    
    test_name = sys.argv[1].lower()
    
    print("M5Stack集中タイマー テスト実行（Phase 0.4改善版）")
    print("=" * 60)
    
    results = []
    
    if test_name == "warning_messages":
        # 警告メッセージ機能のテスト
        results.append(run_native_test("test_warning_messages"))
        
    elif test_name == "alarm_validation":
        # アラームバリデーション機能のテスト
        results.append(run_native_test("test_alarm_validation"))
    
    elif test_name == "button_manager":
        # ボタン管理機能のテスト
        results.append(run_native_test("test_button_manager_native"))
    
    elif test_name == "simple":
        # 純粋ロジックテスト（推奨）
        results.append(run_simple_test())
    
    elif test_name == "pure":
        # 新規作成した純粋ロジックテスト
        pure_results = run_pure_logic_tests()
        results.extend(pure_results)
    
    elif test_name == "all":
        # 全テストを実行
        tests = [
            ("test_warning_messages", "警告メッセージ機能"),
            ("test_alarm_validation", "アラームバリデーション機能"),
            ("test_button_manager_native", "ボタン管理機能")
        ]
        
        for test_file, description in tests:
            results.append(run_native_test(test_file))
        
        # 純粋ロジックテストも追加
        results.append(run_simple_test())
        
        # 新規作成した純粋ロジックテストも追加
        pure_results = run_pure_logic_tests()
        results.extend(pure_results)
    
    else:
        print(f"❌ 不明なテスト名: {test_name}")
        print(__doc__)
        return
    
    # 結果の保存とレポート生成
    save_test_results(results)
    success = generate_test_report(results)
    
    if success:
        print("🎉 テスト実行が完了しました！")
        return 0
    else:
        print("⚠️  一部のテストが失敗しました。詳細を確認してください。")
        return 1

if __name__ == "__main__":
    sys.exit(main()) 