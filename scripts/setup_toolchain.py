#!/usr/bin/env python3
"""
Windowsネイティブツールチェーン設定スクリプト
PlatformIOとGCCのパスを指定されたパスに設定
"""

import os
import sys
from platformio.managers.core import get_core_package_dir
from platformio.managers.platform import PlatformManager

def setup_windows_toolchain(env):
    """Windowsネイティブツールチェーンの設定"""
    
    # PlatformIO実行パス設定
    import os
    platformio_exe = os.path.join(os.path.expanduser("~"), ".platformio", "penv", "Scripts", "platformio.exe")
    if os.path.exists(platformio_exe):
        env['PLATFORMIO_EXE'] = platformio_exe
        print(f"PlatformIO実行パスを設定: {platformio_exe}")
    
    # GCCパス設定
    gcc_path = "c:\\msys64\\ucrt64\\bin\\gcc.exe"
    if os.path.exists(gcc_path):
        # 環境変数にGCCパスを追加
        gcc_dir = os.path.dirname(gcc_path)
        if 'PATH' in env:
            env['PATH'] = gcc_dir + os.pathsep + env['PATH']
        else:
            env['PATH'] = gcc_dir
        
        # GCC関連ツールのパスも設定
        env['CC'] = gcc_path
        env['CXX'] = gcc_path.replace('gcc.exe', 'g++.exe')
        env['AR'] = gcc_path.replace('gcc.exe', 'ar.exe')
        env['LD'] = gcc_path.replace('gcc.exe', 'ld.exe')
        
        print(f"GCCパスを設定: {gcc_path}")
        print(f"GCCディレクトリをPATHに追加: {gcc_dir}")
    else:
        print(f"警告: GCCパスが見つかりません: {gcc_path}")
    
    # シリアルポート設定（Windowsネイティブ）
    if 'upload_port' not in env:
        env['upload_port'] = 'COM3'
    if 'monitor_port' not in env:
        env['monitor_port'] = 'COM3'
    
    print("Windowsネイティブツールチェーン設定完了")

def apply_toolchain_settings(env):
    """ツールチェーン設定を適用"""
    setup_windows_toolchain(env)
    return env 