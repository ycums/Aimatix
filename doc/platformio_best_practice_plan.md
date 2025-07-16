# Aimatix PlatformIO/Unity/LDF ベストプラクティス再構成プラン

## 1. ディレクトリ構成

```
Aimatix/
├── include/                  # 公開ヘッダ（src/の外部公開用、必要に応じて）
├── lib/                      # サードパーティ/再利用ライブラリ
│   └── aimatix_lib/         # 純粋ロジックライブラリ
│       └── src/
│           ├── button_manager.h
│           ├── button_manager.cpp
│           ├── alarm.h
│           ├── alarm.cpp
│           └── ...
├── src/                      # メイン実装（M5Stack依存部）
│   ├── main.cpp
│   ├── ui.cpp
│   ├── ui.h
│   └── ...
├── test/                     # テスト関連
│   ├── pure/                 # native環境向け「純粋ロジック」テスト
│   │   ├── test_button_manager_pure/    # テストごとに独立したディレクトリ
│   │   │   └── test_main.cpp           # ButtonManager専用テスト
│   │   ├── test_alarm_logic_pure/      # テストごとに独立したディレクトリ
│   │   │   └── test_main.cpp           # Alarm専用テスト
│   │   ├── test_debounce_manager_pure/  # 独立したディレクトリ
│   │   │   └── test_main.cpp            # DebounceManager専用テスト
│   │   └── ...
│   ├── integration/          # 実機向け統合テスト
│   │   ├── test_integration.cpp
│   │   └── ...
│   ├── mocks/                # native用モック
│   │   ├── mock_button_manager.h
│   │   ├── mock_time.h
│   │   └── ...
│   └── test_framework.h      # テスト共通ヘッダ等
├── platformio.ini
└── ...
```

- **src/**: 実装と内部ヘッダ（M5Stack依存部）
- **include/**: 外部公開ヘッダ（必要に応じて）
- **lib/aimatix_lib/src/**: 純粋ロジック（ハードウェア非依存）
- **test/pure/**: native環境での純粋ロジックテスト（Unityベース）
  - **重要**: 各テストは独立したディレクトリに配置
  - 例: `test_button_manager_pure/test_main.cpp`
- **test/integration/**: 実機（M5Stack Fire等）での統合テスト
- **test/mocks/**: nativeテスト用のモック実装
- **test_framework.h**: テスト共通のヘルパーやマクロ

## 2. PlatformIOテスト構成のベストプラクティス

### テストディレクトリ構造
PlatformIOでは、**テストごとに独立したディレクトリとtest_main.cppを作成する**のが標準的なアプローチです：

```
test/
├── pure/
│   ├── test_button_manager_pure/     # 独立したディレクトリ
│   │   └── test_main.cpp            # ButtonManager専用テスト
│   ├── test_alarm_logic_pure/       # 独立したディレクトリ
│   │   └── test_main.cpp            # Alarm専用テスト
│   ├── test_debounce_manager_pure/  # 独立したディレクトリ
│   │   └── test_main.cpp            # DebounceManager専用テスト
│   └── ...
└── integration/
    ├── test_integration.cpp         # 統合テスト
    └── ...
```

### 各test_main.cppの役割
- **単一の責務**: 特定のモジュール/機能のテストのみ
- **独立した実行**: 他のテストに依存しない
- **専用のセットアップ/ティアダウン**: そのテストに特化した初期化

### PlatformIOのビルド動作
```ini
[env:native]
# test/pure/ 配下の各ディレクトリを個別にビルド
# 各ディレクトリ内のtest_main.cppがエントリーポイント
# 複数のtest_main.cppが同じディレクトリにあるとリンカーエラー
```

## 3. platformio.ini 設定例

```ini
[platformio]
default_envs = m5stack-fire

[env:m5stack-fire]
platform = espressif32
board = m5stack-fire
framework = arduino
lib_deps =
    m5stack/M5Stack @ ^0.4.3
    arduino-libraries/NTPClient @ ^3.2.1
    WiFiProv
monitor_speed = 115200
build_flags =
    -DCORE_DEBUG_LEVEL=0
    -DM5STACK_FIRE
    -DILI9341_ENABLE_DOUBLE_BUFFER
upload_port = COM3
monitor_port = COM3
build_type = release
upload_speed = 921600

[env:native]
platform = native
build_flags =
    -Iinclude
    -Isrc
    -Itest/mocks
    -DUNITY_INCLUDE_DOUBLE
    -DUNITY_DOUBLE_PRECISION=1e-12
    -DMOCK_M5STACK
    -DTEST_MODE
    -DARDUINO=100
    -D__XTENSA__=0
    -D_WIN32
    -std=c++11
lib_deps =
    throwtheswitch/Unity @ ^2.5.2
build_unflags = -std=gnu++11
test_build_src = yes
; nativeテストは test/pure/ 配下を主に利用
; 各ディレクトリ内のtest_main.cppが個別にビルドされる

[env:test-m5stack-fire]
platform = espressif32
board = m5stack-fire
framework = arduino
lib_deps =
    throwtheswitch/Unity @ ^2.5.2
    m5stack/M5Stack @ ^0.4.3
    arduino-libraries/NTPClient @ ^3.2.1
build_flags =
    -DTEST_MODE
    -DCORE_DEBUG_LEVEL=3
    -DM5STACK_FIRE
    -DUNITY_INCLUDE_CONFIG_H
test_framework = unity
test_build_src = yes
; 実機テストは test/integration/ 配下を主に利用
```

- **build_src_filter**は原則使わず、標準構成でLDFに任せる
- **test/** 配下のテストは `platformio test` で自動検出
- **test/pure/**, **test/integration/** でテスト種別を明確に分離
- **test/mocks/** も `-Itest/mocks` でインクルード
- **重要**: 複数のtest_main.cppを同じディレクトリに配置しない

## 4. テストの書き方

### テストディレクトリの命名規則
```
test/pure/test_<module>_<type>/
└── test_main.cpp
```

例：
- `test_button_manager_pure/` - ButtonManagerの純粋ロジックテスト
- `test_alarm_logic_pure/` - Alarmの純粋ロジックテスト
- `test_integration_basic/` - 基本的な統合テスト

### 各test_main.cppの構造
```cpp
#include <unity.h>
#include <button_manager.h>
#include "mocks/mock_button_manager.h"

void setUp(void) {
    // テスト固有の初期化
}

void tearDown(void) {
    // テスト固有のクリーンアップ
}

void test_button_press_detection() {
    // テストケース
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_button_press_detection);
    // 他のテストケース
    return UNITY_END();
}
```

- 各機能ごとに `test/pure/test_<feature>_pure/test_main.cpp`（native用）
- テスト用モックは `test/mocks/` に配置し、nativeテストで利用
- テストは `platformio test` で一括実行

## 5. LDF（Library Dependency Finder）設定

- デフォルトの `LDF Mode = chain+` で十分
- 特殊な依存がある場合のみ `lib_ldf_mode` を明示

## 6. 移行手順

1. **不要な build_src_filter 設定を削除**
2. **src/・include/・lib/・test/ ディレクトリを整理**
3. **テストは test/pure/・test/integration/ 配下に集約し、mocks も test/mocks/ へ移動**
4. **各テストを独立したディレクトリに配置**
5. **platformio.ini を上記例に準拠して修正**
6. **ビルド・テストが正常に通ることを確認**

## 7. トラブルシューティング

### よくある問題と解決策

#### 複数のtest_main.cppが同じディレクトリにある
**問題**: リンカーエラー「multiple definition of main」
**解決策**: 各テストを独立したディレクトリに配置
```
❌ 悪い例:
test/
├── test_main.cpp
├── test_button_manager_pure_test_main.cpp
└── mocks/

✅ 良い例:
test/
├── pure/
│   ├── test_button_manager_pure/
│   │   └── test_main.cpp
│   └── test_alarm_logic_pure/
│       └── test_main.cpp
└── mocks/
```

#### テストディレクトリの命名
**問題**: テストが検出されない
**解決策**: `test_` プレフィックスで始まるディレクトリ名を使用
```
❌ 悪い例: button_manager_test/
✅ 良い例: test_button_manager_pure/
```

#### モックファイルの配置
**問題**: モックが見つからない
**解決策**: `test/mocks/` に配置し、`-Itest/mocks` でインクルード
```cpp
#include "mocks/mock_button_manager.h"  // 相対パスでインクルード
```

---

## 備考

- 既存のテストやモックは `test/` 配下に移動・整理
- `test/disabled/` など一時的なディレクトリは整理後削除
- `lib/` 配下の自作ライブラリは必要に応じて `src/` へ統合
- nativeテストと実機テストの両立により、ロジックの純粋性と実装の信頼性を両立
- **重要**: PlatformIOのテスト構成では、各テストを独立したディレクトリに配置することがベストプラクティス

--- 

## 【2024年7月 最新】lib/構成・LDF活用 是正プラン

### 背景
- 最小PlatformIO+Unityプロジェクトでlib/xxx/src/xxx.h構成がLDFで自動認識されることを確認。
- しかしAimatix本体では同様の構成でincludeエラー等が発生するケースがあった。

### 主な是正ポイント
1. **lib/配下は必ず `lib/パッケージ名/src/ヘッダ・ソース` 構成に統一する**
    - 例: `lib/aimatix_lib/src/button_manager.h`
    - srcサブディレクトリ必須（PlatformIO仕様）
2. **include記法は `<button_manager.h>` でも "button_manager.h" でもLDFが有効ならどちらでもOK**
    - ただしプロジェクト内で統一推奨
    - 外部/標準は山括弧、自作はダブルクォート等の流儀も可
3. **platformio.iniでbuild_src_filterやbuild_flagsでlib/配下を除外しない**
    - 例: `build_src_filter = +<src/>` だけだとlib/が無視される
    - 基本的にbuild_src_filterは使わない、またはlib/も明示的に含める
4. **循環依存や依存解決失敗に注意**
    - 複数lib間の相互#includeは極力避ける
    - 依存が複雑な場合は設計見直し
5. **テスト/本体srcからlib/配下を参照する際は、LDFが有効なら追加の-I指定は不要**
    - ただし特殊な構成やLDFが効かない場合のみbuild_flagsで-Ilib/xxx/srcを追加
6. **キャッシュ不整合時は `pio run --target clean` を実施**
7. **テストディレクトリ構造はPlatformIOのベストプラクティスに従う**
    - 各テストを独立したディレクトリに配置
    - 複数のtest_main.cppを同じディレクトリに配置しない

### 推奨ディレクトリ構成（例）
```
Aimatix/
├── lib/
│   └── aimatix_lib/
│       └── src/
│           ├── button_manager.h
│           ├── button_manager.cpp
│           └── ...
├── src/
│   └── main.cpp
├── test/
│   └── pure/
│       ├── test_button_manager_pure/
│       │   └── test_main.cpp
│       ├── test_alarm_logic_pure/
│       │   └── test_main.cpp
│       └── ...
├── platformio.ini
```

### トラブルシューティング
- includeエラー時は
    - ヘッダの配置場所・記法・lib/xxx/src/構成を再確認
    - platformio.iniのbuild_src_filter, build_flagsを確認
    - `pio run --target clean` でキャッシュクリア
- テストエラー時は
    - テストディレクトリ構造が正しいか確認
    - 複数のtest_main.cppが同じディレクトリにないか確認
    - モックファイルの配置とインクルードパスを確認
- それでも解決しない場合は、具体的なエラー内容・include記法・ディレクトリ構成を記録し、issue化

### 備考
- LDFの詳細: https://docs.platformio.org/en/latest/librarymanager/ldf.html
- PlatformIOテストの詳細: https://docs.platformio.org/en/latest/advanced/unit-testing/index.html
- 本ドキュメントは今後の構成変更・トラブル時の参照用とする 

### 【付録】Aimatixプロジェクト構成 是正のための手順

1. **lib/配下の構成を見直す**
    - すべての自作ライブラリは `lib/パッケージ名/src/` 配下にヘッダ・ソースを移動する
    - 例: `lib/aimatix_lib/src/button_manager.h` など
    - 旧: `lib/aimatix_lib/button_manager.h` → 新: `lib/aimatix_lib/src/button_manager.h`
2. **src/やtest/からの#include記法を統一する**
    - 基本は `#include <button_manager.h>` もしくは `#include "button_manager.h"` で統一
    - 相対パスやlib/aimatix_lib/src/を含む記法は避ける
3. **platformio.iniのbuild_src_filter, build_flagsを確認・修正**
    - `build_src_filter` を使っている場合はlib/も明示的に含める、または削除
    - `build_flags` で不要な-I指定があれば削除（LDFが有効なら不要）
4. **循環依存がないか確認し、必要なら設計を見直す**
    - 複数lib間の相互#includeを避ける
    - 依存が複雑な場合は分割・抽象化を検討
5. **テストディレクトリ構造をPlatformIOのベストプラクティスに従って整理**
    - 各テストを独立したディレクトリに配置
    - 複数のtest_main.cppを同じディレクトリに配置しない
6. **不要なキャッシュ・ビルド生成物を削除する**
    - `pio run --target clean` でクリーンビルド 

## 8. レポート・フィードバック（2024/06 最新状況）

### 現状の構成
- lib/aimatix_lib/src/：純粋ロジック（M5Stack非依存、DI設計）
- test/pure/test_xxx_pure/：各ロジックごとに独立ディレクトリ＋test_main.cpp
- test/mocks/：nativeテスト用モック
- 不要なダミーテストや重複ディレクトリは削除し、責務ごとに整理

### 工夫点・LDF対策
- LDF（Library Dependency Finder）はデフォルト設定（chain+）で十分。lib/aimatix_lib/src/配下のロジックは自動でビルド・インクルードされる
- build_src_filterやlib_extra_dirs等の余計な設定は極力使わず、標準構成を維持
- テストごとに独立ディレクトリ＋test_main.cppとし、複数main定義エラーを防止
- テスト用モックはtest/mocks/に集約し、-Itest/mocksでインクルード
- テストディレクトリ名は必ずtest_で始める

### include記法のベストプラクティス
- プロジェクト内ロジックのincludeは "ファイル名.h" 記法（例：#include "button_manager.h"）
- サードパーティや標準ライブラリは <...> 記法
- テスト用モックは "mocks/mock_xxx.h" のように相対パスで記述
- includeパスの衝突や循環依存を避けるため、責務ごとにディレクトリを分離

### まとめ
- PlatformIO/Unity/LDFの標準構成・命名規則・include記法を守ることで、移植・拡張・CI運用が容易になる
- 問題発生時は、まずディレクトリ構成・include記法・余計なbuild設定を見直すこと
- 本ドキュメントは今後も実践知見を随時追記・更新する 