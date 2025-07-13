# Windowsネイティブビルドスクリプト (PowerShell版)
# WSLでのシリアルポート問題を回避するためのWindowsネイティブビルド

param(
    [Parameter(Position=0)]
    [ValidateSet("build", "upload", "monitor", "test", "clean", "full")]
    [string]$Command = "build",
    
    [Parameter()]
    [string]$Port = "COM3",
    
    [Parameter()]
    [switch]$Verbose
)

# スクリプト情報
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Aimatix - Windowsネイティブビルド (PowerShell)" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

Write-Host "シリアルポート: $Port" -ForegroundColor Yellow

# コマンド実行関数
function Invoke-PlatformIO {
    param(
        [string]$Arguments,
        [string]$Description
    )
    
    Write-Host "`n$Description..." -ForegroundColor Green
    Write-Host "実行コマンド: platformio $Arguments" -ForegroundColor Gray
    
    if ($Verbose) {
        platformio $Arguments.Split(' ')
    } else {
        platformio $Arguments.Split(' ') 2>&1 | Tee-Object -FilePath "build_log.txt"
    }
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "✓ $Description が正常に完了しました" -ForegroundColor Green
    } else {
        Write-Error "✗ $Description でエラーが発生しました (終了コード: $LASTEXITCODE)"
        exit $LASTEXITCODE
    }
}

# メイン処理
try {
    switch ($Command) {
        "build" {
            Invoke-PlatformIO "run" "プロジェクトのビルド"
        }
        "upload" {
            Invoke-PlatformIO "run --target upload --upload-port $Port" "M5Stack Fireへのアップロード"
        }
        "monitor" {
            Write-Host "`nシリアルモニターを開始..." -ForegroundColor Green
            Write-Host "終了するには Ctrl+C を押してください" -ForegroundColor Yellow
            platformio device monitor --port $Port
        }
        "test" {
            Invoke-PlatformIO "test" "ユニットテストの実行"
        }
        "clean" {
            Invoke-PlatformIO "run --target clean" "ビルドファイルのクリーン"
        }
        "full" {
            Write-Host "`nフルビルドプロセスを開始..." -ForegroundColor Green
            Invoke-PlatformIO "run --target clean" "ビルドファイルのクリーン"
            Invoke-PlatformIO "run" "プロジェクトのビルド"
            Invoke-PlatformIO "test" "ユニットテストの実行"
        }
    }
    
    Write-Host "`n========================================" -ForegroundColor Cyan
    Write-Host "すべての処理が正常に完了しました" -ForegroundColor Green
    Write-Host "========================================" -ForegroundColor Cyan
    
} catch {
    Write-Error "予期しないエラーが発生しました: $_"
    exit 1
} 