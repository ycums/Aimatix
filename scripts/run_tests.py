#!/usr/bin/env python3
"""
M5Stack集中タイマー テスト実行スクリプト

使用方法:
    python scripts/run_tests.py [test_name]

テスト名:
    warning_messages  - 警告メッセージ機能のテスト
    alarm_validation  - アラームバリデーション機能のテスト
    button_manager    - ボタン管理機能のテスト
    all              - 全テストを実行
"""

import sys
import subprocess
import os
from pathlib import Path

def run_command(command, description):
    """コマンドを実行し、結果を表示"""
    print(f"\n=== {description} ===")
    print(f"実行コマンド: {command}")
    
    try:
        result = subprocess.run(command, shell=True, capture_output=True, text=True)
        
        if result.returncode == 0:
            print("✅ 成功")
            if result.stdout:
                print("出力:")
                print(result.stdout)
        else:
            print("❌ 失敗")
            if result.stderr:
                print("エラー:")
                print(result.stderr)
            if result.stdout:
                print("出力:")
                print(result.stdout)
        
        return result.returncode == 0
    except Exception as e:
        print(f"❌ 実行エラー: {e}")
        return False

def run_native_test(test_name):
    """Native環境でテストを実行"""
    command = f"pio test -e native -f {test_name}"
    return run_command(command, f"Native環境での{test_name}テスト")

def run_esp32_test(test_name):
    """ESP32環境でテストを実行"""
    command = f"pio test -e test-m5stack-fire -f {test_name}"
    return run_command(command, f"ESP32環境での{test_name}テスト")

def main():
    # プロジェクトルートディレクトリに移動
    project_root = Path(__file__).parent.parent
    os.chdir(project_root)
    
    # コマンドライン引数を取得
    if len(sys.argv) < 2:
        print(__doc__)
        return
    
    test_name = sys.argv[1].lower()
    
    print("M5Stack集中タイマー テスト実行")
    print("=" * 50)
    
    success_count = 0
    total_count = 0
    
    if test_name == "warning_messages":
        # 警告メッセージ機能のテスト
        total_count += 1
        if run_native_test("test_warning_messages"):
            success_count += 1
        
    elif test_name == "alarm_validation":
        # アラームバリデーション機能のテスト
        total_count += 1
        if run_native_test("test_alarm_validation"):
            success_count += 1
    
    elif test_name == "button_manager":
        # ボタン管理機能のテスト
        total_count += 1
        if run_native_test("test_button_manager_native"):
            success_count += 1
    
    elif test_name == "all":
        # 全テストを実行
        tests = [
            ("test_warning_messages", "警告メッセージ機能"),
            ("test_alarm_validation", "アラームバリデーション機能"),
            ("test_button_manager_native", "ボタン管理機能")
        ]
        
        for test_file, description in tests:
            total_count += 1
            print(f"\n--- {description}テスト ---")
            if run_native_test(test_file):
                success_count += 1
    
    else:
        print(f"❌ 不明なテスト名: {test_name}")
        print(__doc__)
        return
    
    # 結果サマリー
    print("\n" + "=" * 50)
    print("テスト結果サマリー")
    print(f"成功: {success_count}/{total_count}")
    
    if success_count == total_count:
        print("🎉 全テストが成功しました！")
    else:
        print("⚠️  一部のテストが失敗しました。")
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main()) 