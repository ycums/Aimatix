@echo off
REM Windowsネイティブビルドスクリプト
REM WSLでのシリアルポート問題を回避するためのWindowsネイティブビルド

echo ========================================
echo Aimatix - Windowsネイティブビルド
echo ========================================

REM 引数チェック
if "%1"=="" (
    echo 使用方法: build_windows.bat [build^|upload^|monitor^|test^|clean]
    echo.
    echo コマンド:
    echo   build   - プロジェクトをビルド
    echo   upload  - M5Stack Fireにアップロード
    echo   monitor - シリアルモニターを開始
    echo   test    - ユニットテストを実行
    echo   clean   - ビルドファイルをクリーン
    echo.
    goto :end
)

REM コマンド実行
if "%1"=="build" (
    echo ビルドを開始...
    platformio run
) else if "%1"=="upload" (
    echo アップロードを開始...
    platformio run --target upload
) else if "%1"=="monitor" (
    echo シリアルモニターを開始...
    platformio device monitor
) else if "%1"=="test" (
    echo ユニットテストを実行...
    platformio test
) else if "%1"=="clean" (
    echo ビルドファイルをクリーン...
    platformio run --target clean
) else (
    echo エラー: 不明なコマンド '%1'
    goto :end
)

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo コマンドが正常に完了しました
    echo ========================================
) else (
    echo.
    echo ========================================
    echo エラーが発生しました (終了コード: %ERRORLEVEL%)
    echo ========================================
)

:end
pause 