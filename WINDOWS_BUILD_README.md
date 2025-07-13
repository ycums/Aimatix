# Windowsネイティブビルド設定

## 概要

WSLでのシリアルポート問題を回避するため、WindowsネイティブでPlatformIOとGCCを使用するように設定を変更しました。

## 設定内容

### 1. PlatformIO設定 (`platformio.ini`)

- **PlatformIO実行パス**: `C:\Users\youch\.platformio\penv\Scripts\platformio.exe`
- **GCCパス**: `c:\msys64\ucrt64\bin\gcc.exe`
- **シリアルポート**: `COM3` (デフォルト)
- **Windowsネイティブツールチェーン**: ESP32用のネイティブツールチェーンを使用

### 2. ツールチェーン設定スクリプト (`scripts/setup_toolchain.py`)

- PlatformIOとGCCのパスを自動設定
- 環境変数の自動設定
- シリアルポートの自動設定

### 3. ビルドスクリプト

#### バッチファイル (`build_windows.bat`)
```cmd
# 基本的なビルド
build_windows.bat build

# アップロード
build_windows.bat upload

# シリアルモニター
build_windows.bat monitor

# テスト実行
build_windows.bat test

# クリーン
build_windows.bat clean
```

#### PowerShellスクリプト (`build_windows.ps1`)
```powershell
# 基本的なビルド
.\build_windows.ps1 build

# アップロード（ポート指定）
.\build_windows.ps1 upload -Port COM4

# シリアルモニター
.\build_windows.ps1 monitor

# テスト実行
.\build_windows.ps1 test

# フルビルド（クリーン→ビルド→テスト）
.\build_windows.ps1 full

# 詳細出力付き
.\build_windows.ps1 build -Verbose
```

## 必要なソフトウェア

### 1. PlatformIO
```cmd
# インストール確認
C:\Users\youch\.platformio\penv\Scripts\platformio.exe --version
```

### 2. MSYS2 (GCC)
```cmd
# インストール確認
c:\msys64\ucrt64\bin\gcc.exe --version
```

## 使用方法

### 1. 初回セットアップ
```cmd
# プロジェクトディレクトリに移動
cd /path/to/Aimatix

# 依存関係のインストール
build_windows.bat build
```

### 2. 日常的な開発
```cmd
# ビルド
build_windows.bat build

# アップロード
build_windows.bat upload

# シリアルモニターでデバッグ
build_windows.bat monitor
```

### 3. テスト実行
```cmd
# ユニットテスト
build_windows.bat test
```

## トラブルシューティング

### 1. PlatformIOが見つからない
```
エラー: PlatformIOが見つかりません
```
**解決方法**: PlatformIOをインストールまたはパスを確認
```cmd
pip install platformio
```

### 2. GCCが見つからない
```
警告: GCCが見つかりません
```
**解決方法**: MSYS2をインストール
1. [MSYS2公式サイト](https://www.msys2.org/)からダウンロード
2. インストール後、`c:\msys64\ucrt64\bin\gcc.exe`が存在することを確認

### 3. シリアルポートエラー
```
エラー: シリアルポートにアクセスできません
```
**解決方法**: 
1. デバイスマネージャーでCOMポート番号を確認
2. `platformio.ini`の`upload_port`と`monitor_port`を修正
3. 他のアプリケーションがシリアルポートを使用していないことを確認

### 4. 権限エラー
```
エラー: アクセスが拒否されました
```
**解決方法**: PowerShellを管理者として実行

## 設定のカスタマイズ

### シリアルポートの変更
`platformio.ini`を編集:
```ini
upload_port = COM4
monitor_port = COM4
```

### GCCパスの変更
`scripts/setup_toolchain.py`を編集:
```python
gcc_path = "c:\\your\\custom\\path\\gcc.exe"
```

### PlatformIOパスの変更
`platformio.ini`を編集:
```ini
platformio_exe = C:\your\custom\path\platformio.exe
```

## パフォーマンス向上のヒント

1. **SSD使用**: ビルドディレクトリをSSDに配置
2. **メモリ増設**: 大容量プロジェクトのビルド時間短縮
3. **並列ビルド**: PlatformIOの並列ビルド機能を活用
4. **キャッシュ活用**: `.pio/cache`ディレクトリを保持

## 注意事項

- WSLとの混在使用は避けてください
- シリアルポートは一度に一つのアプリケーションのみが使用可能
- ビルド中は他の重い処理を避けてください
- 定期的に`clean`コマンドでキャッシュをクリアしてください 