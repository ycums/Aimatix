# Clang-Tidy静的解析ガイド

## 概要

このプロジェクトでは、[Clang-Tidy](https://docs.platformio.org/en/stable/advanced/static-code-analysis/tools/clang-tidy.html)を使用して静的コード解析を行い、コードの品質向上とバグの早期発見を図っています。

## 設定内容

### platformio.ini設定

各環境に以下の設定を追加しています：

```ini
check_tool = clangtidy
check_flags = 
    clangtidy: --checks=-*,bugprone-*,cert-*,clang-analyzer-*,cppcoreguidelines-*,modernize-*,performance-*,portability-*,readability-*
check_severity = medium
```

### .clang-tidy設定ファイル

プロジェクトルートに`.clang-tidy`ファイルを作成し、以下の設定を行っています：

- **有効なチェック**: バグ検出、セキュリティ、コーディングガイドライン、パフォーマンス、可読性に関するチェック
- **無効化したチェック**: Arduino/ESP32環境に適さないチェックや、過度に厳格なチェック
- **命名規則**: クラス名はCamelCase、関数・変数名はlower_case、定数はUPPER_CASE

## 使用方法

### 静的解析の実行

```bash
# 特定の環境で静的解析を実行
pio check -e m5stack-fire
pio check -e native
pio check -e test-m5stack-fire

# 全環境で静的解析を実行
pio check

# 特定のファイルのみ解析
pio check --src-filter="+<src/main.cpp>"
```

### 解析結果の確認

```bash
# 詳細な解析結果を表示
pio check -e native --verbose

# 特定の重要度以上の警告のみ表示
pio check -e native --severity=high
```

## 主なチェック項目

### バグ検出 (bugprone-*)
- 潜在的なバグやエラーの検出
- 未初期化変数の使用
- 不正なポインタ操作

### セキュリティ (cert-*)
- CERTセキュアコーディングガイドラインに基づくチェック
- バッファオーバーフロー
- 不正なメモリ操作

### コーディングガイドライン (cppcoreguidelines-*)
- C++ Core Guidelinesに基づくチェック
- 現代的なC++の使用
- 型安全性の確保

### パフォーマンス (performance-*)
- 非効率なアルゴリズムの検出
- 不要なコピーの検出
- 最適化の機会の特定

### 可読性 (readability-*)
- コードの可読性向上
- 一貫した命名規則
- 不要な複雑さの検出

## 設定のカスタマイズ

### 特定のチェックを無効化

`.clang-tidy`ファイルの`CheckOptions`セクションで特定のチェックを無効化できます：

```yaml
CheckOptions:
  - key: readability-magic-numbers
    value: 'false'
```

### 新しいチェックを追加

`platformio.ini`の`check_flags`で新しいチェックを追加できます：

```ini
check_flags = 
    clangtidy: --checks=-*,bugprone-*,cert-*,clang-analyzer-*,cppcoreguidelines-*,modernize-*,performance-*,portability-*,readability-*,misc-*
```

## トラブルシューティング

### 誤検知の対処

1. **特定の行でチェックを無効化**:
   ```cpp
   // NOLINTNEXTLINE(readability-magic-numbers)
   int magicNumber = 42;
   ```

2. **関数全体でチェックを無効化**:
   ```cpp
   // NOLINTBEGIN(readability-magic-numbers)
   void functionWithMagicNumbers() {
       // ...
   }
   // NOLINTEND(readability-magic-numbers)
   ```

3. **ファイル全体でチェックを無効化**:
   ```cpp
   // NOLINTBEGIN
   // ファイル全体のコード
   // NOLINTEND
   ```

### よくある問題

1. **Arduino固有の警告**: ESP32/Arduino環境では一部のチェックが適切でない場合があります
2. **外部ライブラリの警告**: M5Stackライブラリなど、外部ライブラリからの警告は無視できます
3. **プラットフォーム固有のコード**: ハードウェア依存のコードでは一部のチェックが適用できない場合があります

## CI/CDでの活用

### GitHub Actionsでの設定例

```yaml
- name: Run Clang-Tidy
  run: |
    pio check -e native
    pio check -e m5stack-fire
```

### 品質ゲート

- **High**: 高重要度の警告は0件以下
- **Medium**: 中重要度の警告は10件以下
- **Low**: 低重要度の警告は50件以下

## 参考資料

- [PlatformIO Clang-Tidy公式ドキュメント](https://docs.platformio.org/en/stable/advanced/static-code-analysis/tools/clang-tidy.html)
- [Clang-Tidy公式ドキュメント](https://clang.llvm.org/extra/clang-tidy/)
- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/) 