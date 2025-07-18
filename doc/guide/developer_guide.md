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
└── platformio.ini           # PlatformIO設定
```

## 3. ビルド・テスト方法

### 3.1 基本的なビルド

#### 3.1.1 M5Stack Fire向けビルド
```bash
# ビルド
pio run -e m5stack-fire

# アップロード
pio run -e m5stack-fire -t upload

# シリアルモニター
pio run -e m5stack-fire -t monitor
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
pio test -e native -f test_button_manager_pure
```

#### 3.2.2 テストカバレッジ測定
```bash
# カバレッジ測定
pio test -e native --coverage
```

### 3.3 クリーンビルド
```bash
# 全環境クリーン
pio run -t clean

# 特定環境クリーン
pio run -e m5stack-fire -t clean
```

## 4. 開発ワークフロー

### 4.1 新機能開発

#### 4.1.1 開発手順
1. **要件確認**: `doc/spec/requirements.md`で要件を確認
2. **設計**: `doc/design/`で設計を確認・更新
3. **実装**: 純粋ロジック→アダプター→UIの順で実装
4. **テスト**: 単体テスト→統合テストの順でテスト
5. **ドキュメント更新**: 必要に応じてドキュメントを更新

#### 4.1.2 実装順序
1. **純粋ロジック**: `lib/libaimatix/src/`に実装
2. **インターフェース**: `lib/libaimatix/include/`に定義
3. **アダプター**: `src/m5stack_adapters.cpp/h`に実装
4. **UI**: `src/ui.cpp/h`に実装
5. **統合**: `src/main.cpp`で統合

### 4.2 バグ修正

#### 4.2.1 修正手順
1. **問題の特定**: エラーメッセージ、ログを確認
2. **原因分析**: どのレイヤーで問題が発生しているか特定
3. **修正**: 該当レイヤーで修正
4. **テスト**: 修正内容のテスト
5. **回帰テスト**: 他の機能への影響確認

#### 4.2.2 修正優先度
1. **高優先度**: ビルドエラー、クラッシュ
2. **中優先度**: 機能不具合、UI問題
3. **低優先度**: パフォーマンス、UI改善

## 5. デバッグ方法

### 5.1 シリアルデバッグ

#### 5.1.1 デバッグ出力
```cpp
// デバッグ出力の有効化
#define CORE_DEBUG_LEVEL 3

// デバッグ出力
Serial.println("Debug message");
Serial.printf("Value: %d\n", value);
```

#### 5.1.2 シリアルモニター
```bash
# シリアルモニター起動
pio run -e m5stack-fire -t monitor

# ボーレート設定
monitor_speed = 115200
```

### 5.2 テストデバッグ

#### 5.2.1 テスト実行時のデバッグ
```cpp
// テスト内でのデバッグ出力
TEST_ASSERT_EQUAL(expected, actual);
printf("Debug: expected=%d, actual=%d\n", expected, actual);
```

#### 5.2.2 モックデバッグ
```cpp
// モックの状態確認
MockButtonManager* mock = new MockButtonManager();
mock->setButtonState(0, true);
TEST_ASSERT_TRUE(mock->isPressed(0));
```

### 5.3 メモリデバッグ

#### 5.3.1 メモリ使用量確認
```cpp
// メモリ使用量出力
Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
Serial.printf("Min free heap: %d\n", ESP.getMinFreeHeap());
```

#### 5.3.2 メモリリーク検出
```cpp
// メモリリーク検出（開発時のみ）
#ifdef DEBUG
    // メモリ使用量の記録
    size_t initialHeap = ESP.getFreeHeap();
    // 処理実行
    size_t finalHeap = ESP.getFreeHeap();
    if (initialHeap != finalHeap) {
        Serial.println("Memory leak detected!");
    }
#endif
```

## 6. コーディング規約

### 6.1 命名規則

#### 6.1.1 関数名
- **英語**: 動詞+名詞
- **例**: `handleDigitEditInput`, `drawMainDisplay`

#### 6.1.2 変数名
- **英語**: キャメルケース
- **例**: `currentMode`, `lastModeChange`

#### 6.1.3 定数名
- **英語**: 大文字+アンダースコア
- **例**: `LONG_PRESS_TIME`, `DEBOUNCE_TIME`

#### 6.1.4 クラス名
- **英語**: パスカルケース
- **例**: `ButtonManager`, `InputState`

### 6.2 コメント規約

#### 6.2.1 日本語コメント
- **用途**: 機能説明、設計意図の説明
- **例**: `// ボタン状態を更新する`

#### 6.2.2 英語コメント
- **用途**: 技術的な詳細、API仕様
- **例**: `// Update button states and handle debouncing`

#### 6.2.3 関数ヘッダー
```cpp
/**
 * 時刻を入力形式で表示する
 * @param time 表示する時刻（秒）
 * @param x 表示位置X座標
 * @param y 表示位置Y座標
 * @return 表示成功時true
 */
bool displayTimeInput(time_t time, int x, int y);
```

### 6.3 ファイル構成

#### 6.3.1 ヘッダーファイル
- **型定義**: 構造体、列挙型
- **関数宣言**: 公開関数の宣言
- **定数定義**: 定数の定義

#### 6.3.2 ソースファイル
- **実装**: 関数の実装
- **ロジック**: ビジネスロジック
- **処理**: 具体的な処理

#### 6.3.3 インクルード順序
1. 標準ライブラリ
2. 外部ライブラリ
3. プロジェクト内

## 7. トラブルシューティング

### 7.1 よくある問題

#### 7.1.1 ビルドエラー
```
undefined reference to `function_name'
```
**解決方法**: 
- 関数の宣言・定義を確認
- インクルードファイルを確認
- リンクエラーの場合は依存関係を確認

#### 7.1.2 インクルードエラー
```
fatal error: 'header.h' file not found
```
**解決方法**:
- ファイルパスを確認
- インクルード順序を確認
- ファイルの存在を確認

#### 7.1.3 テストエラー
```
Test failed: expected X, but was Y
```
**解決方法**:
- テストケースの期待値を確認
- 実装のロジックを確認
- 境界値の処理を確認

### 7.2 デバッグTips

#### 7.2.1 段階的デバッグ
1. **最小限のテストケース**: 問題を再現する最小のケースを作成
2. **ログ出力**: 重要なポイントでログを出力
3. **状態確認**: 変数の状態を定期的に確認
4. **条件分岐**: 条件分岐の結果を確認

#### 7.2.2 パフォーマンスデバッグ
```cpp
// 処理時間測定
unsigned long startTime = millis();
// 処理実行
unsigned long endTime = millis();
Serial.printf("Processing time: %lu ms\n", endTime - startTime);
```

## 8. 品質保証

### 8.1 テスト戦略

#### 8.1.1 単体テスト
- **対象**: 純粋ロジック
- **環境**: native環境
- **カバレッジ**: 80%以上

#### 8.1.2 統合テスト
- **対象**: モジュール間の連携
- **環境**: native環境（モック使用）
- **カバレッジ**: 95%以上

#### 8.1.3 実機テスト
- **対象**: ハードウェア動作
- **環境**: M5Stack Fire
- **頻度**: 統合テスト通過後の最終確認

### 8.2 コードレビュー

#### 8.2.1 レビュー項目
- **機能**: 要件を満たしているか
- **品質**: コードの品質は適切か
- **テスト**: テストは十分か
- **ドキュメント**: ドキュメントは更新されているか

#### 8.2.2 レビュープロセス
1. **自己レビュー**: 実装者が自己レビュー
2. **ピアレビュー**: 他の開発者がレビュー
3. **修正**: 指摘された問題を修正
4. **再レビュー**: 必要に応じて再レビュー

## 9. リソース

### 9.1 ドキュメント
- `doc/spec/`: 仕様書
- `doc/design/`: 設計書
- `doc/guide/`: 使用方法ガイド
- `doc/operation/`: 運用・開発ルール

### 9.2 テストファイル
- `test/pure/`: 純粋ロジックテスト
- `test/mock/`: モックファイル
- `test/integration/`: 統合テスト

### 9.3 設定ファイル
- `platformio.ini`: PlatformIO設定
- `.gitignore`: Git除外設定
- `README.md`: プロジェクト概要

---

**作成日**: 2025年1月  
**バージョン**: 1.0.0  
**更新日**: 2025年1月 