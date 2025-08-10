# Aimatix 開発者ガイド

## 1. 概要

このガイドでは、Aimatixプロジェクトの開発環境セットアップ、ビルド・テスト方法、デバッグ方法について説明します。

## 2. 開発環境セットアップ

### 2.1 必要なソフトウェア

#### 2.1.1 必須ソフトウェア
- **PlatformIO**: ビルドシステム
- **Visual Studio Code**: 推奨エディタ
- **Git**: バージョン管理

#### 2.1.2 推奨プラグイン
- **PlatformIO IDE**: PlatformIO統合
- **C/C++**: C/C++言語サポート
- **GitLens**: Git統合

### 2.2 プロジェクト構造
```
Aimatix/
├── lib/
│   └── libaimatix/
│       ├── include/          # インターフェース定義
│       └── src/              # 純粋ロジック
├── src/                      # M5Stack依存実装
├── test/                     # テストファイル
│   ├── pure/                 # 純粋ロジックテスト
│   └── mock/                 # モックファイル
├── doc/                      # ドキュメント
├── .cursor/                  # Cursor Rules
│   └── rules/               # AI用ルールファイル
├── .clang-tidy              # Clang-Tidy設定
└── platformio.ini           # PlatformIO設定
```

## 3. ビルド・テスト方法

### 3.1 基本的なビルド

#### 3.1.1 M5Stack ESP32向けビルド（Fire + Core v1.1対応）
```bash
# ビルド
pio run

# アップロード（自動検出）
pio run -t upload

# 特定ポート指定アップロード
pio run -t upload --upload-port COM3  # Fire
pio run -t upload --upload-port COM5  # Core v1.1

# シリアルモニター
pio run -t monitor
```

#### 3.1.2 Native環境向けビルド（テスト用）
```bash
# ビルド
pio run -e native

# テスト実行
pio test -e native
```

### 3.2 テスト実行

#### 3.2.1 全テスト実行
```bash
# 全テスト実行
pio test -e native

# 特定テスト実行
pio test -e native -f pure/test_button_manager_pure
```

注意点: printf デバッグを行うときは `-v` オプションが必要です。

#### 3.2.2 テストカバレッジ測定
```bash
# カバレッジ測定
python scripts/test_coverage.py
```

#### 3.2.3 静的解析実行
```bash
# 静的解析実行
pio check -e native

# 特定の重要度の警告のみ表示
pio check -e native --severity=high

# 特定の重要度の警告のみ表示
pio check -e native --severity=high
```

### 3.3 クリーンビルド
```bash
# 全環境クリーン
pio run -t clean

# 特定環境クリーン
pio run -e m5stack-core2 -t clean
```

## 4. M5Unified対応の技術的注意事項

### 4.1 色の型統一
M5Unified/M5GFXでは色の型が`uint16_t`（16ビット）であるため、以下の点に注意してください：

#### 4.1.1 インターフェース設計
```cpp
// 正しい実装例
class IDisplay {
public:
    virtual void setTextColor(uint16_t color, uint16_t bgColor) = 0;
    virtual void fillRect(int x, int y, int w, int h, uint16_t color) = 0;
    virtual void drawLine(int x0, int y0, int x1, int y1, uint16_t color) = 0;
};
```

#### 4.1.2 色定数の使用
```cpp
// ui_constants.h
#define AMBER_COLOR 0xFB20  // uint16_t形式
#define TFT_BLACK 0x0000    // uint16_t形式

// 使用例
display.setTextColor(AMBER_COLOR, TFT_BLACK);
```

### 4.2 型キャストの回避
内部での型変換を避け、直接的な型一致を実現してください：

```cpp
// 推奨: 直接的な型一致
void setTextColor(uint16_t color, uint16_t bgColor) override {
    M5.Display.setTextColor(color, bgColor);
}

// 非推奨: 型キャスト
void setTextColor(uint16_t color, uint16_t bgColor) override {
    M5.Display.setTextColor((uint16_t)color, (uint16_t)bgColor);
}
```

### 4.3 M5Unified APIの使用
```cpp
// 正しいM5Unified API使用例
#include <M5Unified.h>
#include <M5GFX.h>

// ディスプレイ操作
M5.Display.fillScreen(TFT_BLACK);
M5.Display.setTextColor(AMBER_COLOR, TFT_BLACK);
M5.Display.drawString("Hello", 10, 10);

// スプライト描画
M5Canvas canvas(&M5.Display);
canvas.createSprite(100, 20);
canvas.fillSprite(TFT_BLACK);
canvas.drawRect(0, 0, 100, 20, AMBER_COLOR);
canvas.pushSprite(10, 50);
canvas.deleteSprite();
```

## 5. デバッグ方法

### 5.1 シリアルデバッグ
```cpp
// デバッグ出力
Serial.println("Debug message");
Serial.printf("Value: %d\n", value);
```

### 5.2 色のデバッグ
```cpp
// 色値の確認
Serial.printf("AMBER_COLOR: 0x%04X\n", AMBER_COLOR);
Serial.printf("TFT_BLACK: 0x%04X\n", TFT_BLACK);
```

## 6. トラブルシューティング

### 6.1 色が正しく表示されない場合
1. **色の型確認**: `uint16_t`を使用しているか確認
2. **色定数の確認**: `AMBER_COLOR`、`TFT_BLACK`が正しく定義されているか確認
3. **型キャストの確認**: 不要な型キャストがないか確認

### 6.2 コンパイルエラーの場合
1. **型の不一致**: 色パラメータが`uint16_t`になっているか確認
2. **インターフェースの確認**: `IDisplay.h`の型定義を確認
3. **テストファイルの確認**: テストファイルも`uint16_t`に統一されているか確認

### 6.3 アップロードエラーの場合
1. **ポートの確認**: `pio device list`でポートを確認
2. **デバイスの再接続**: USBケーブルを再接続
3. **再アップロード**: 複数回試行

## 7. コーディング規約

### 7.1 ファイル命名規則
- **クラス名をスネークケースに変換してファイル名とする**
- 例: `AlarmDisplayState` → `alarm_display_state.cpp`

### 7.2 アーキテクチャ原則
- **純粋ロジックは Arduino.h を直接インポートしない**
- **ハードウェア依存はアダプターインターフェースで分離**
- **DIパターンを使用して依存性を注入**
- **静的メソッド宣言**: `static type ...();`形式を使用
- **重複分岐の統合**: 同じ処理を複数箇所で重複しない
- **疑わしいincludeの禁止**: `.cpp`ファイルの直接includeは禁止

### 7.3 色の扱い
- **色の型統一**: すべての色パラメータは`uint16_t`を使用
- **色定数の統一**: `ui_constants.h`で定義された色定数を使用
- **型キャスト回避**: 内部での型変換を避ける

## 8. 品質保証

### 8.1 テスト原則
- **TDDアプローチ**: テストファースト
- **純粋ロジックのみをテスト対象とする**
- **モックを使用してハードウェア依存を分離**

### 8.2 静的解析
- **Clang-Tidy**: 継続的な品質チェック
- **品質ゲート**: 中重要度警告19件以下

### 8.3 テストカバレッジ
- **目標**: 85%以上
- **測定**: `python scripts/test_coverage.py` 

## 9. CI自動化（Quality Gate 概要）

- 本プロジェクトの公式CIは `/.github/workflows/quality-gate.yml` に定義されています。
- PRトリガで `python scripts/quality_gate.py` を実行し、カバレッジと静的解析を実施します。
- 結果確認方法（Job Summary/アーティファクトの内容・名称）は運用ガイド `doc/operation/quality_gates.md` を参照してください。

### 9.1 ローカル再現手順
```bash
pio run -e native
python scripts/quality_gate.py
```