#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
M5Stack集中タイマー 純粋ロジックテスト実行スクリプト（Phase 0.6改善版）
カスタムテストフレームワーク対応
"""

import os
import sys
import subprocess
import json
from datetime import datetime

class TestResult:
    def __init__(self, test_name, success, output, error_code=0):
        self.test_name = test_name
        self.success = success
        self.output = output
        self.error_code = error_code
        self.timestamp = datetime.now().isoformat()

def run_command(command, description):
    """コマンドを実行し、結果を返す"""
    print(f"実行中: {description}")
    print(f"コマンド: {command}")
    
    try:
        # Windows環境でのUnicode文字エンコーディング問題を解決
        if os.name == 'nt':  # Windows
            result = subprocess.run(command, shell=True, capture_output=True, text=True, timeout=30, encoding='utf-8', errors='replace')
        else:
            result = subprocess.run(command, shell=True, capture_output=True, text=True, timeout=30)
        
        success = result.returncode == 0
        output = result.stdout + result.stderr
        
        if success:
            print(f"✓ {description} 成功")
        else:
            print(f"✗ {description} 失敗")
            print(f"エラー: {result.stderr}")
        
        return success, output, result.returncode
    except subprocess.TimeoutExpired:
        print(f"✗ {description} タイムアウト")
        return False, "タイムアウト", -1
    except Exception as e:
        print(f"✗ {description} エラー: {e}")
        return False, str(e), -1

def compile_test(test_name, source_file):
    """テストをコンパイル"""
    unity_src = ".pio/libdeps/native/Unity/src/unity.c"
    mock_src = "test/mocks/mock_m5stack.cpp"
    
    # テストに応じて必要なソースファイルを追加
    additional_sources = []
    
    if test_name == "time_logic":
        additional_sources = ["src/time_logic.cpp"]
    elif test_name == "alarm_logic":
        additional_sources = ["src/time_logic.cpp", "src/alarm.cpp", "src/settings.cpp"]
    elif test_name == "input_logic":
        additional_sources = ["src/time_logic.cpp"]  # 純粋ロジックテストなのでinput.cppは不要
    elif test_name == "settings_logic":
        additional_sources = ["src/time_logic.cpp", "src/settings.cpp"]
    elif test_name == "warning_messages":
        additional_sources = []  # 純粋ロジックテストなのでui.cppは不要
    
    # カスタムテストフレームワークを含むコンパイルコマンド
    all_sources = [source_file, unity_src, mock_src] + additional_sources
    compile_cmd = f"g++ -I.pio/libdeps/native/Unity/src -Ilib -Itest/mocks -Isrc -Itest -std=c++11 -o {test_name} {' '.join(all_sources)}"
    
    return run_command(compile_cmd, f"{test_name}のコンパイル")

def run_test(test_name):
    """テストを実行"""
    run_cmd = f"{test_name}.exe"
    return run_command(run_cmd, f"{test_name}の実行")

def main():
    print("=== M5Stack集中タイマー 純粋ロジックテスト（Phase 0.6改善版） ===")
    
    # テストファイルの定義
    tests = [
        {
            "name": "time_logic",
            "source": "test/test_time_logic_simple.cpp",
            "dependencies": ["src/time_logic.cpp"]
        },
        {
            "name": "alarm_logic", 
            "source": "test/test_alarm_logic_simple.cpp",
            "dependencies": ["src/time_logic.cpp"]
        },
        {
            "name": "input_logic",
            "source": "test/test_input_logic_simple.cpp", 
            "dependencies": ["src/time_logic.cpp"]
        },
        {
            "name": "settings_logic",
            "source": "test/test_settings_logic_simple.cpp",
            "dependencies": ["src/time_logic.cpp"]
        },
        {
            "name": "warning_messages",
            "source": "test/test_warning_messages_simple.cpp",
            "dependencies": ["src/time_logic.cpp"]
        }
    ]
    
    results = []
    success_count = 0
    
    print(f"テストファイル数: {len(tests)}\n")
    
    for test in tests:
        test_name = test["name"]
        source_file = test["source"]
        
        print(f"--- {test_name} テスト ---")
        
        # コンパイル
        compile_success, compile_output, compile_code = compile_test(test_name, source_file)
        
        if compile_success:
            # 実行
            run_success, run_output, run_code = run_test(test_name)
            
            # 結果を記録
            result = TestResult(
                test_name=test_name,
                success=run_success,
                output=compile_output + "\n" + run_output,
                error_code=run_code
            )
            results.append(result)
            
            if run_success:
                success_count += 1
                print(f"✓ {test_name}の実行 成功")
            else:
                print(f"✗ {test_name}の実行 失敗 (終了コード: {run_code})")
        else:
            # コンパイル失敗
            result = TestResult(
                test_name=test_name,
                success=False,
                output=compile_output,
                error_code=compile_code
            )
            results.append(result)
            print(f"✗ {test_name}のコンパイル 失敗")
        
        print()
    
    # 結果サマリー
    print("=== テスト結果 ===")
    print(f"成功: {success_count}/{len(tests)}")
    
    if success_count == len(tests):
        print("✓ 全テストが成功しました！")
    else:
        print("⚠️  一部のテストが失敗しました。詳細を確認してください。")
    
    # 結果をJSONファイルに保存
    results_data = []
    for result in results:
        results_data.append({
            "test_name": result.test_name,
            "success": result.success,
            "output": result.output,
            "error_code": result.error_code,
            "timestamp": result.timestamp
        })
    
    with open("test_results.json", "w", encoding="utf-8") as f:
        json.dump(results_data, f, ensure_ascii=False, indent=2)
    
    print("\n📊 テスト結果を test_results.json に保存しました")
    
    return success_count == len(tests)

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1) 