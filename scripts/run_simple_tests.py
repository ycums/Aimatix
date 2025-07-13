#!/usr/bin/env python3
"""
M5Stack集中タイマーの純粋ロジックテスト実行スクリプト
Windows環境でのUnityテスト実行
"""

import os
import sys
import subprocess
from pathlib import Path

def get_compiler():
    """利用可能なコンパイラを取得"""
    # g++を優先、なければclang++を試す
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

def get_unity_path():
    """Unityライブラリのパスを取得"""
    # PlatformIOのUnityライブラリパスを確認
    pio_unity_path = Path(".pio/libdeps/native/Unity/src")
    if pio_unity_path.exists():
        return str(pio_unity_path)
    
    # システムのUnityライブラリを確認
    system_paths = [
        "/usr/include/unity",
        "/usr/local/include/unity",
        "C:/unity/include",
        "C:/Program Files/unity/include"
    ]
    
    for path in system_paths:
        if Path(path).exists():
            return path
    
    return None

def run_command(command, description):
    """コマンドを実行"""
    print(f"実行中: {description}")
    print(f"コマンド: {command}")
    
    result = subprocess.run(command, shell=True, capture_output=True, text=True, encoding='utf-8', errors='replace')
    
    if result.stdout:
        print("出力:")
        print(result.stdout)
    
    if result.stderr:
        print("エラー:")
        print(result.stderr)
    
    if result.returncode != 0:
        print(f"✗ {description} 失敗 (終了コード: {result.returncode})")
        return False
    
    print(f"✓ {description} 成功")
    return True

def compile_and_run_test(test_file, test_name):
    """テストファイルをコンパイルして実行"""
    unity_path = get_unity_path()
    if not unity_path:
        print(f"✗ Unityライブラリが見つかりません")
        return False
    
    compiler = get_compiler()
    if not compiler:
        return False
    
    test_path = Path("test") / test_file
    output_name = f"test_{test_name}"
    
    # Unityライブラリのソースファイルを探す
    unity_src = None
    for unity_file in ["unity.c", "unity.cpp"]:
        unity_src_path = Path(unity_path) / unity_file
        if unity_src_path.exists():
            unity_src = str(unity_src_path)
            break
    
    if not unity_src:
        print(f"✗ Unityソースファイルが見つかりません: {unity_path}")
        return False
    
    # コンパイルコマンド（src/time_logic.cppも明示的に追加）
    compile_cmd = f"{compiler} -I{unity_path} -Ilib -Itest/mocks -Isrc -std=c++11 -o {output_name} {test_path} {unity_src} src/time_logic.cpp test/mocks/mock_m5stack.cpp"
    
    if not run_command(compile_cmd, f"{test_name}のコンパイル"):
        return False
    
    # 実行
    if os.name == 'nt':
        run_cmd = f'{output_name}.exe'
    else:
        run_cmd = f'./{output_name}'
    if not run_command(run_cmd, f"{test_name}の実行"):
        return False
    
    # 実行ファイルを削除
    try:
        os.remove(output_name)
    except FileNotFoundError:
        pass
    
    return True

def run_all_tests():
    """全テストを実行"""
    test_files = [
        ("test_time_logic_simple.cpp", "time_logic"),
        ("test_alarm_logic_simple.cpp", "alarm_logic"),
        ("test_input_logic_simple.cpp", "input_logic"),
        ("test_settings_logic_simple.cpp", "settings_logic"),
        ("test_warning_messages_simple.cpp", "warning_messages")
    ]
    
    print("=== M5Stack集中タイマー 純粋ロジックテスト ===")
    print(f"テストファイル数: {len(test_files)}")
    print()
    
    success_count = 0
    total_count = len(test_files)
    
    for test_file, test_name in test_files:
        print(f"\n--- {test_name} テスト ---")
        if compile_and_run_test(test_file, test_name):
            success_count += 1
        print()
    
    print("=== テスト結果 ===")
    print(f"成功: {success_count}/{total_count}")
    
    if success_count == total_count:
        print("🎉 全テスト成功！")
        return True
    else:
        print("❌ 一部のテストが失敗しました")
        return False

def main():
    """メイン関数"""
    # 作業ディレクトリを確認
    if not Path("src").exists() or not Path("test").exists():
        print("エラー: src/またはtest/ディレクトリが見つかりません")
        print("プロジェクトルートディレクトリで実行してください")
        return False
    
    return run_all_tests()

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1) 