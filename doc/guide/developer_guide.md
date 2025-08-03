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
pio run -t clean
```

## 4. 開発ワークフロー

### 4.1 新機能開発

#### 4.1.1 開発手順
1. **要件確認**: `doc/spec/requirements.md`で要件を確認
2. **設計**: `doc/design/`で設計を確認・更新
3. **実装**: 純粋ロジック→アダプター→UIの順で実装
4. **テスト**: 単体テスト→統合テストの順でテスト
5. **静的解析**: Clang-Tidyによる品質チェック
6. **ドキュメント更新**: 必要に応じてドキュメントを更新

#### 4.1.2 実装順序
1. **純粋ロジック**: `lib/libaimatix/src/`に実装
2. **インターフェース**: `lib/libaimatix/include/`に定義
3. **テスト**: 単体テストの作成・実行
4. **静的解析**: Clang-Tidyによる静的解析実行
5. **アダプター**: `src/m5stack_adapters.cpp/h`に実装
6. **UI**: `src/ui.cpp/h`に実装
7. **統合**: `src/main.cpp`で統合

### 4.2 バグ修正

#### 4.2.1 修正手順
1. **問題の特定**: エラーメッセージ、ログを確認
2. **原因分析**: どのレイヤーで問題が発生しているか特定
3. **修正**: 該当レイヤーで修正
4. **テスト**: 修正内容のテスト
5. **静的解析**: Clang-Tidyによる品質チェック
6. **回帰テスト**: 他の機能への影響確認

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
pio run -t monitor

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
- **英語**: スネークケース（lower_case）
- **例**: `handle_digit_edit_input`, `draw_main_display`

#### 6.1.2 変数名
- **英語**: スネークケース（lower_case）
- **例**: `current_mode`, `last_mode_change`
- **制約**: 3文字以上の意味のある名前を使用

#### 6.1.3 定数名
- **英語**: 大文字+アンダースコア（UPPER_CASE）
- **例**: `LONG_PRESS_TIME`, `DEBOUNCE_TIME`

#### 6.1.4 クラス名
- **英語**: キャメルケース（CamelCase）
- **例**: `ButtonManager`, `InputState`

#### 6.1.5 ファイル名
- **形式**: スネークケース
- **説明**: クラス名はスネークケースに変換してファイル名とします。
- **例**: `ButtonManager` クラス -> `button_manager.h`, `button_manager.cpp`

#### 6.1.6 定数定義
- **方法**: `constexpr`変数を使用（現代的なC++方式）
- **説明**: マジックナンバーは避け、意味のある定数名で定義します。
- **例**: 
  ```cpp
  constexpr int SCREEN_WIDTH = 320;
  constexpr int SCREEN_HEIGHT = 240;
  constexpr unsigned long DEBOUNCE_TIME = 50;
  constexpr int SECONDS_10 = 10;
  constexpr int MINUTES_60 = 60;
  constexpr int HOURS_24 = 24;
  ```

#### 6.1.7 グローバル変数
- **原則**: グローバル変数は避ける
- **説明**: 可能な限りローカル変数やクラスメンバーを使用し、グローバル変数は最小限に抑えます。
- **例外**: ハードウェア依存の変数（例：`TFT_eSprite sprite`）は必要に応じて使用

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
1. 標準ライブラリ（C++形式: `<cstdio>`, `<cstring>`）
2. 外部ライブラリ
3. プロジェクト内

#### 6.3.4 インクルード禁止事項
- **禁止**: `.cpp`ファイルの直接include
- **理由**: コンパイルエラーの原因となる
- **例**:
  ```cpp
  // ❌ 悪い例
  #include "SettingsLogic.cpp"
  
  // ✅ 良い例
  #include "SettingsLogic.h"
  ```
- **対策**: 実装は`.h`ファイルにinline関数として記述するか、適切なライブラリ構成にする

#### 6.3.5 関数サイズ制限
- **原則**: 関数は50行以内を目安とする
- **説明**: 長い関数は分割し、単一責任の原則に従います。
- **例外**: UI描画関数など、分割が困難な場合は適切にコメントで説明

### 6.4 Clang-Tidy準拠

#### 6.4.1 主要なチェック項目
- **bugprone-***: バグ検出
- **performance-***: パフォーマンス最適化
- **readability-***: 可読性向上
- **modernize-***: 現代的なC++の使用
- **cppcoreguidelines-***: C++ Core Guidelines準拠
- **misc-***: その他の品質チェック

#### 6.4.2 必須遵守項目
- **マジックナンバーの禁止**: 定数定義を使用
- **短い識別子名の禁止**: 3文字以上の意味のある名前
- **暗黙的bool変換の禁止**: 明示的な比較を使用
- **波括弧なしの文の禁止**: 単一文でも波括弧を使用
- **const修飾子の適切な使用**: 変更されない変数はconst
- **C++標準ヘッダーの使用**: `<cstdio>`、`<cstring>`等

#### 6.4.3 品質ゲート基準
- **High**: 高重要度の警告は0件以下
- **Medium**: 中重要度の警告は19件以下（ゲート基準）
- **Low**: 低重要度の警告は50件以下

### 6.5 Clang-Tidy警告防止ガイドライン

#### 6.5.1 変数初期化の徹底
**警告**: `cppcoreguidelines-init-variables`
**対策**:
```cpp
// ❌ 悪い例
int value;
bool flag;
std::vector<int> items;

// ✅ 良い例
int value = 0;
bool flag = false;
std::vector<int> items{};
```

**ルール**:
- すべての変数は宣言時に初期化する
- ポインタは`nullptr`で初期化
- 配列・ベクターは空の初期化子`{}`を使用

#### 6.5.2 静的メンバー関数の活用
**警告**: `readability-convert-member-functions-to-static`
**対策**:
```cpp
// ❌ 悪い例
class AlarmLogic {
    void addAlarm(int time) {
        // インスタンス変数を使用しない処理
    }
};

// ✅ 良い例
class AlarmLogic {
    static void addAlarm(int time) {
        // 静的関数として定義
    }
};
```

**ルール**:
- インスタンス変数を使用しないメソッドは`static`にする
- 純粋な計算関数は静的関数として定義
- 状態を変更しない関数は静的関数を検討

#### 6.5.3 パラメータ順序の明確化
**警告**: `bugprone-easily-swappable-parameters`
**対策**:
```cpp
// ❌ 悪い例（順序が入れ替わりやすい）
void drawRectangle(int x, int y, int width, int height);

// ✅ 良い例（型が異なる）
void drawRectangle(int x, int y, unsigned int width, unsigned int height);

// ✅ 良い例（構造体を使用）
struct Rectangle {
    int x, y, width, height;
};
void drawRectangle(const Rectangle& rect);
```

**ルール**:
- 同じ型の連続するパラメータは避ける
- 構造体やクラスでパラメータをグループ化
- 型を変えて区別を明確にする

#### 6.5.4 グローバル変数のconst化
**警告**: `cppcoreguidelines-avoid-non-const-global-variables`
**対策**:
```cpp
// ❌ 悪い例
int global_counter = 0;
std::vector<int> global_data;

// ✅ 良い例
const int MAX_COUNT = 100;
const std::vector<int> DEFAULT_DATA = {1, 2, 3};

// ✅ 良い例（シングルトンパターン）
class GlobalState {
private:
    static GlobalState* instance;
    GlobalState() = default;
public:
    static GlobalState& getInstance();
};
```

**ルール**:
- グローバル変数は可能な限り`const`にする
- 変更が必要な場合はシングルトンパターンを使用
- モジュール内のstatic変数を検討

#### 6.5.5 ポインタ演算の回避
**警告**: `cppcoreguidelines-pro-bounds-pointer-arithmetic`
**対策**:
```cpp
// ❌ 悪い例
char* ptr = buffer;
*(ptr + 1) = 'a';
*(ptr + 2) = 'b';

// ✅ 良い例
char* ptr = buffer;
ptr[1] = 'a';
ptr[2] = 'b';

// ✅ 良い例（イテレータ使用）
std::vector<char> buffer;
auto it = buffer.begin();
std::advance(it, 1);
*it = 'a';
```

**ルール**:
- ポインタ演算の代わりに配列添字`[]`を使用
- 可能な限りイテレータを使用
- 範囲チェックを必ず行う

#### 6.5.6 配列添字の安全な使用
**警告**: `cppcoreguidelines-pro-bounds-constant-array-index`
**対策**:
```cpp
// ❌ 悪い例
int array[10];
int index = getIndex();
array[index] = value;  // 実行時インデックス

// ✅ 良い例
int array[10];
int index = getIndex();
if (index >= 0 && index < 10) {
    array[index] = value;
}

// ✅ 良い例（std::array使用）
std::array<int, 10> array;
int index = getIndex();
if (index >= 0 && index < array.size()) {
    array[index] = value;
}
```

**ルール**:
- 実行時インデックスには必ず範囲チェックを行う
- `std::array`や`std::vector`の使用を検討
- 定数インデックスのみ配列添字を使用

#### 6.5.7 現代的なC++機能の活用
**警告**: `modernize-use-trailing-return-type`, `modernize-use-default-member-init`
**対策**:
```cpp
// ❌ 悪い例
class Settings {
    bool soundEnabled;
    int selectedItem;
public:
    Settings() : soundEnabled(false), selectedItem(0) {}
    int getSelectedItem() { return selectedItem; }
};

// ✅ 良い例
class Settings {
    bool soundEnabled = false;
    int selectedItem = 0;
public:
    auto getSelectedItem() -> int { return selectedItem; }
};
```

**ルール**:
- メンバー変数はデフォルト初期化子を使用
- 複雑な戻り値型には後置戻り値型を使用
- `auto`キーワードを適切に活用

#### 6.5.8 コード複雑度の管理
**警告**: `readability-function-cognitive-complexity`
**対策**:
```cpp
// ❌ 悪い例（複雑な関数）
void processAlarm(int input) {
    if (input > 0) {
        if (input < 100) {
            if (isValidTime(input)) {
                if (hasSpace()) {
                    // 処理
                }
            }
        }
    }
}

// ✅ 良い例（分割された関数）
void processAlarm(int input) {
    if (!isValidInput(input)) return;
    if (!hasSpace()) return;
    addAlarm(input);
}

bool isValidInput(int input) {
    return input > 0 && input < 100 && isValidTime(input);
}
```

**ルール**:
- 関数の認知複雑度は25以下を目安とする
- 早期リターンでネストを減らす
- 複雑な条件は別関数に分離

#### 6.5.9 重複分岐の統合
**警告**: `bugprone-branch-clone`
**対策**:
```cpp
// ❌ 悪い例
if (condition1) {
    maxValue = MAX_DIGIT_9;
} else if (condition2) {
    maxValue = MAX_DIGIT_9;
} else if (condition3) {
    maxValue = MAX_DIGIT_9;
}

// ✅ 良い例
if (condition1 || condition2 || condition3) {
    // maxValue = MAX_DIGIT_9; // デフォルト値と同じなので設定不要
} else if (special_condition) {
    maxValue = SPECIAL_VALUE;
}
```

**ルール**:
- 同じ処理を行う分岐は統合する
- デフォルト値と同じ設定は省略する
- コメントでデフォルト値であることを明記

#### 6.5.10 疑わしいincludeの禁止
**警告**: `bugprone-suspicious-include`
**対策**:
```cpp
// ❌ 悪い例
#include "SettingsLogic.cpp"

// ✅ 良い例
#include "SettingsLogic.h"
```

**ルール**:
- `.cpp`ファイルの直接includeは禁止
- 実装は`.h`ファイルにinline関数として記述するか、適切なライブラリ構成にする
- コンパイルエラーの原因となるため厳格に禁止

#### 6.5.11 const修飾子の適用基準
**警告**: `misc-const-correctness`
**適用基準**:
```cpp
// ✅ 適用可能な例
const PartialInputLogic::ParsedTime parsedTime = PartialInputLogic::parsePartialInput(digits, entered);

// ❌ 適用不可な例（関数の制約）
char buffer[32] = {};  // snprintfで書き込まれるためconst不可
int totalWidth = 0;    // 計算中に変更されるためconst不可
```

**ルール**:
- 初期化後変更されない変数は`const`にする
- 関数の制約（例：`snprintf`の引数）がある場合は適用しない
- 計算中に変更される変数は`const`にしない

#### 6.5.12 静的メソッド宣言の型指定方法
**警告**: `readability-convert-member-functions-to-static`
**対策**:
```cpp
// ❌ 悪い例
static auto initAlarms(std::vector<time_t>& alarms, time_t now) -> void;

// ✅ 良い例
static void initAlarms(std::vector<time_t>& alarms, time_t now);
```

**ルール**:
- `static auto ... -> type;`ではなく`static type ...();`を使用
- Clang-Tidyが`static`メソッドを正しく認識するため
- 戻り値型は明示的に指定する

#### 6.5.13 メンバー関数のstatic化基準
**警告**: `readability-convert-member-functions-to-static`
**適用基準**:
```cpp
// ✅ static化可能な例
class DateTimeInputState {
    static int getDigitValue(int position) {
        // インスタンス変数を使用しない純粋な計算
        return position >= 0 ? position : 0;
    }
};

// ❌ static化不可な例
class DateTimeInputState {
    int getDigitValue(int position) const {
        return dateTimeDigits[position];  // メンバー変数を使用
    }
private:
    std::vector<int> dateTimeDigits;
};
```

**ルール**:
- インスタンス変数を使用しないメソッドは`static`にする
- メンバー変数にアクセスするメソッドは`static`にしない
- 引数にポインタや参照がある場合は`static`化を慎重に検討

#### 6.5.14 開発時のチェックリスト
新しいコードを書く際は以下を確認：

1. **変数初期化**: すべての変数が初期化されているか
2. **静的関数**: インスタンス変数を使用しない関数は`static`か
3. **パラメータ設計**: 同じ型の連続パラメータは避けているか
4. **グローバル変数**: 必要最小限で`const`を検討しているか
5. **ポインタ操作**: 安全な配列アクセスを使用しているか
6. **範囲チェック**: 実行時インデックスに範囲チェックがあるか
7. **現代的なC++**: 適切な初期化子と戻り値型を使用しているか
8. **関数サイズ**: 認知複雑度が適切か
9. **重複分岐**: 同じ処理を複数箇所で重複していないか
10. **include**: `.cpp`ファイルを直接includeしていないか
11. **const修飾子**: 適用可能な箇所で`const`を使用しているか
12. **静的メソッド宣言**: `static type ...();`形式を使用しているか

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

#### 7.1.4 Clang-Tidy警告
```
warning: 'x' is too short, expected at least 3 characters
```
**解決方法**:
- 短い変数名を意味のある名前に変更
- 例: `x` → `pos_x`, `y` → `pos_y`

```
warning: 10 is a magic number; consider replacing it with a named constant
```
**解決方法**:
- マジックナンバーを定数定義に変更
- 例: `10` → `constexpr int SECONDS_10 = 10;`

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
- **カバレッジ**: 85%以上

#### 8.1.2 統合テスト
- **対象**: モジュール間の連携
- **環境**: native環境（モック使用）
- **カバレッジ**: 95%以上

#### 8.1.3 実機テスト
- **対象**: ハードウェア動作
- **環境**: M5Stack Fire
- **頻度**: 統合テスト通過後の最終確認

### 8.2 静的解析戦略

#### 8.2.1 Clang-Tidy静的解析
- **対象**: 全ソースコード
- **環境**: native環境
- **頻度**: 各開発ステップで実行
- **品質ゲート**: 中重要度警告19件以下

#### 8.2.2 静的解析の活用
- **バグ検出**: 潜在的なバグの早期発見
- **コード品質**: コーディング規約の遵守
- **保守性**: コードの可読性・保守性向上

#### 8.2.3 品質ゲート基準
```bash
# 品質ゲートチェック
pio check -e native

# 結果確認
Component            HIGH    MEDIUM    LOW
------------------  ------  --------  -----
lib\libaimatix\src    0        17       0
src                   0        2        0

Total                 0        19       0  # ← 19件以下で合格
```

### 8.3 コードレビュー

#### 8.3.1 レビュー項目
- **機能**: 要件を満たしているか
- **品質**: コードの品質は適切か
- **テスト**: テストは十分か
- **静的解析**: Clang-Tidy警告は適切に対処されているか
- **品質ゲート**: 85件以下の基準を満たしているか
- **ドキュメント**: ドキュメントは更新されているか

#### 8.3.2 レビュープロセス
1. **自己レビュー**: 実装者が自己レビュー
2. **静的解析**: Clang-Tidyによる自動チェック
3. **品質ゲート確認**: 85件以下の基準確認
4. **ピアレビュー**: 他の開発者がレビュー
5. **修正**: 指摘された問題を修正
6. **再レビュー**: 必要に応じて再レビュー

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
- `.clang-tidy`: Clang-Tidy設定
- `.gitignore`: Git除外設定
- `README.md`: プロジェクト概要

## 10. Cursor Rules管理

### 10.1 Cursor Rules概要

#### 10.1.1 目的
- **AI支援**: Cursor AIがプロジェクトを理解するためのルール
- **効率化**: 開発時の自動補完・提案の精度向上
- **一貫性**: プロジェクト全体でのコーディング規約の統一

#### 10.1.2 構成
```
.cursor/rules/
├── project-overview.mdc      # Always Apply - プロジェクト基本情報
├── architecture.mdc          # Auto Attached - アーキテクチャ設計
├── testing.mdc              # Auto Attached - テスト戦略
└── platformio.mdc           # Auto Attached - PlatformIO運用
```

### 10.2 ルールタイプ

#### 10.2.1 Always Apply
- **適用範囲**: 全ファイル
- **用途**: プロジェクトの基本原則
- **例**: `project-overview.mdc`

#### 10.2.2 Auto Attached
- **適用範囲**: 特定のファイルパターン
- **用途**: 領域別の詳細ルール
- **例**: `architecture.mdc` (lib/**/*, src/**/*)

#### 10.2.3 Agent Requested
- **適用範囲**: AIが必要に応じて参照
- **用途**: 詳細なガイドライン
- **例**: デバッグ用ルール

### 10.3 更新手順

#### 10.3.1 自動更新（推奨）
```bash
# 1. @doc/ を更新
# 2. 関連ドキュメントを読み込み
@doc/README.md
@doc/guide/developer_guide.md
@doc/design/architecture.md
@doc/operation/testing_strategy.md

# 3. Cursor Rulesを再生成
/Generate Cursor Rules
```

#### 10.3.2 手動更新
```bash
# 特定ルールの直接編集
code .cursor/rules/project-overview.mdc
code .cursor/rules/testing.mdc

# 新しいルールの追加
code .cursor/rules/new-rule.mdc
```

### 10.4 保守方針

#### 10.4.1 @doc/ との関係
- **@doc/ を主要な情報源として維持**
- **.cursor/rules は @doc/ から生成**
- **両方を並行して使用**

#### 10.4.2 更新タイミング
- **@doc/ 更新時**: 自動再生成
- **微調整時**: 直接編集
- **新機能追加時**: 新しいルールファイル作成

#### 10.4.3 品質管理
- **ルールの整合性**: @doc/ との整合性確認
- **適用範囲**: 適切なglobs設定
- **内容の正確性**: プロジェクトの現状を反映

### 10.5 運用ガイド

#### 10.5.1 新規参加者向け
```bash
# 1. プロジェクト概要を確認
cat .cursor/rules/project-overview.mdc

# 2. アーキテクチャを理解
cat .cursor/rules/architecture.mdc

# 3. テスト方法を確認
cat .cursor/rules/testing.mdc
```

#### 10.5.2 開発者向け
```bash
# 開発時のルール確認
# Cursorが自動的に適用するルールを確認

# 特定領域の開発時
# 該当するルールファイルを参照
```

#### 10.5.3 管理者向け
```bash
# ルールの更新管理
# @doc/ 更新時の自動再生成

# 品質確認
# ルールの整合性と適用範囲の確認
```

### 10.6 トラブルシューティング

#### 10.6.1 ルールが適用されない場合
```bash
# 1. ファイルパターンの確認
# globs設定が正しいか確認

# 2. ルールファイルの構文確認
# frontmatterの形式が正しいか確認

# 3. Cursorの再起動
# ルール変更後はCursorの再起動が必要
```

#### 10.6.2 ルールの競合
```bash
# 1. 適用順序の確認
# Always Apply > Auto Attached > Agent Requested

# 2. 重複するglobsの確認
# 複数のルールが同じファイルに適用される場合

# 3. ルールの統合
# 必要に応じてルールを統合
```

### 10.7 ベストプラクティス

#### 10.7.1 ルール設計
- **明確な目的**: 各ルールの目的を明確にする
- **適切な範囲**: 過度に広範囲なルールを避ける
- **保守性**: 更新しやすい構造にする

#### 10.7.2 内容管理
- **@doc/ との整合性**: 常に@doc/と整合性を保つ
- **現状反映**: プロジェクトの現状を正確に反映
- **段階的更新**: 大きな変更は段階的に行う

#### 10.7.3 品質保証
- **定期的な確認**: ルールの有効性を定期的に確認
- **フィードバック**: 開発者からのフィードバックを収集
- **継続的改善**: ルールの継続的な改善

## 11. リソース

### 11.1 ドキュメント
- `doc/spec/`: 仕様書
- `doc/design/`: 設計書
- `doc/guide/`: 使用方法ガイド
- `doc/operation/`: 運用・開発ルール

### 11.2 Cursor Rules
- `.cursor/rules/project-overview.mdc`: プロジェクト概要
- `.cursor/rules/architecture.mdc`: アーキテクチャ設計
- `.cursor/rules/testing.mdc`: テスト戦略
- `.cursor/rules/platformio.mdc`: PlatformIO運用

### 11.3 テストファイル
- `test/pure/`: 純粋ロジックテスト
- `test/mock/`: モックファイル
- `test/integration/`: 統合テスト

### 11.4 設定ファイル
- `platformio.ini`: PlatformIO設定
- `.clang-tidy`: Clang-Tidy設定
- `.gitignore`: Git除外設定
- `README.md`: プロジェクト概要

---

**作成日**: 2025年1月  
**バージョン**: 2.1.0  
**更新日**: 2025年1月  
**品質ゲート基準**: 中重要度警告19件以下  
**Cursor Rules**: 自動生成・手動調整対応 