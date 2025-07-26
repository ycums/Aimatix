# Clang-Tidy設定と@/guide齟齬分析・判断書

## 概要

Clang-Tidyの設定（`.clang-tidy`）と`@/guide`の内容を照らし合わせて、齟齬を特定し、どちらに寄せるかの判断を求めます。

## 齟齬リスト

### 1. 命名規則の齟齬

#### 1.1 クラス名の命名規則
- **`.clang-tidy`設定**: `CamelCase`（例：`ButtonManager`）
- **`@/guide/developer_guide.md`**: `パスカルケース`（例：`ButtonManager`）
- **`@/guide/sample.ui.h/.cpp`**: `CamelCase`（例：`TFT_eSprite`）

**判断**: どちらに統一しますか？
- [x] `.clang-tidy`に合わせて`CamelCase`に統一
- [ ] `@/guide`に合わせて`パスカルケース`に統一

#### 1.2 関数名の命名規則
- **`.clang-tidy`設定**: `lower_case`（例：`draw_title_bar`）
- **`@/guide/developer_guide.md`**: `動詞+名詞`（例：`handleDigitEditInput`, `drawMainDisplay`）
- **`@/guide/sample.ui.h/.cpp`**: `lower_case`（例：`drawTitleBar`, `initUI`）

**判断**: どちらに統一しますか？
- [ ] `.clang-tidy`に合わせて`lower_case`に統一
- [x] `@/guide`に合わせて`動詞+名詞`（キャメルケース）に統一

#### 1.3 変数名の命名規則
- **`.clang-tidy`設定**: `lower_case`（例：`battery_level`）
- **`@/guide/developer_guide.md`**: `キャメルケース`（例：`currentMode`, `lastModeChange`）
- **`@/guide/sample.ui.h/.cpp`**: `lower_case`（例：`batteryLevel`, `isCharging`）

**判断**: どちらに統一しますか？
- [x] `.clang-tidy`に合わせて`lower_case`に統一
- [ ] `@/guide`に合わせて`キャメルケース`に統一

#### 1.4 定数名の命名規則
- **`.clang-tidy`設定**: `UPPER_CASE`（例：`AMBER_COLOR`）
- **`@/guide/developer_guide.md`**: `大文字+アンダースコア`（例：`LONG_PRESS_TIME`）
- **`@/guide/sample.ui.h/.cpp`**: `UPPER_CASE`（例：`AMBER_COLOR`, `SCREEN_WIDTH`）

**判断**: どちらに統一しますか？
- [x] `.clang-tidy`に合わせて`UPPER_CASE`に統一
- [ ] `@/guide`に合わせて`大文字+アンダースコア`に統一

### 2. マジックナンバーの扱い

#### 2.1 マジックナンバー警告の設定
- **`.clang-tidy`設定**: `readability-magic-numbers`を無効化
- **`@/guide/developer_guide.md`**: マジックナンバーについて言及なし
- **`@/guide/sample.ui.h/.cpp`**: マジックナンバーを多用（例：`320`, `240`, `20`）

**判断**: マジックナンバーの扱いをどうしますか？
- [ ] `.clang-tidy`に合わせて警告を無効化したまま
- [x] `@/guide`に合わせて定数定義を推奨し、警告を有効化

#### 2.2 定数定義の方法
- **`.clang-tidy`設定**: マジックナンバー警告無効
- **`@/guide/sample.ui.h`**: `#define`で定数定義（例：`#define SCREEN_WIDTH 320`）
- **`@/guide/developer_guide.md`**: 定数定義について言及なし

**判断**: 定数定義の方法をどうしますか？
- [ ] `#define`マクロを使用（現在の`@/guide`方式）
- [x] `const`変数を使用（現代的なC++方式）

### 3. グローバル変数の扱い

#### 3.1 グローバル変数警告の設定
- **`.clang-tidy`設定**: `cppcoreguidelines-avoid-non-const-global-variables`を無効化
- **`@/guide/developer_guide.md`**: グローバル変数について言及なし
- **`@/guide/sample.ui.h/.cpp`**: グローバル変数を使用（例：`extern TFT_eSprite sprite`）

**判断**: グローバル変数の扱いをどうしますか？
- [ ] `.clang-tidy`に合わせて警告を無効化したまま
- [ ] `@/guide`に合わせてグローバル変数は非推奨とし、警告を有効化

### 4. コメント規約

#### 4.1 コメント言語
- **`.clang-tidy`設定**: コメント関連の警告を無効化
- **`@/guide/developer_guide.md`**: 日本語コメントを推奨
- **`@/guide/sample.ui.h/.cpp`**: 日本語コメントを使用

**判断**: コメント言語をどうしますか？
- [ ] `.clang-tidy`に合わせて英語コメントを推奨
- [x] `@/guide`に合わせて日本語コメントを推奨

### 5. インクルード順序

#### 5.1 インクルード順序の設定
- **`.clang-tidy`設定**: インクルード順序関連の警告を無効化
- **`@/guide/developer_guide.md`**: 標準ライブラリ→外部ライブラリ→プロジェクト内の順序を推奨
- **`@/guide/sample.ui.h/.cpp`**: 順序が混在

**判断**: インクルード順序をどうしますか？
- [ ] `.clang-tidy`に合わせて警告を無効化したまま
- [x] `@/guide`に合わせて順序を統一し、警告を有効化

### 6. 関数サイズ・複雑度

#### 6.1 関数サイズ警告の設定
- **`.clang-tidy`設定**: `readability-function-size`を無効化
- **`@/guide/developer_guide.md`**: 関数サイズについて言及なし
- **`@/guide/sample.ui.cpp`**: 長い関数が存在

**判断**: 関数サイズの制限をどうしますか？
- [ ] `.clang-tidy`に合わせて警告を無効化したまま
- [x] `@/guide`に合わせて関数サイズ制限を推奨し、警告を有効化

### 7. エラーハンドリング

#### 7.1 エラーハンドリング警告の設定
- **`.clang-tidy`設定**: 多くのエラーハンドリング関連警告を無効化
- **`@/guide/developer_guide.md`**: エラーハンドリングについて言及なし
- **`@/guide/sample.ui.cpp`**: 基本的なエラーハンドリング

**判断**: エラーハンドリングの厳格さをどうしますか？
- [ ] `.clang-tidy`に合わせて緩い設定のまま
- [x] `@/guide`に合わせて厳格なエラーハンドリングを推奨し、警告を有効化


---

**作成日**: 2025年1月  
**作成者**: AI Assistant  
**目的**: Clang-Tidy設定と@/guideの整合性確保 