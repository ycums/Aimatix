# Aimatix PlatformIO/Unity/LDF ベストプラクティス再構成プラン

## 1. ディレクトリ構成

```
Aimatix/
├── include/                  # 公開ヘッダ（src/の外部公開用、必要に応じて）
├── lib/                      # サードパーティ/再利用ライブラリ
├── src/                      # メイン実装
│   ├── main.cpp
│   ├── *.cpp
│   └── *.h
├── test/                     # テスト関連
│   ├── pure/                 # native環境向け「純粋ロジック」テスト
│   │   └── test_<feature>_pure.cpp
│   │   └── ...
│   ├── integration/          # 実機向け統合テスト
│   │   └── test_<feature>_integration.cpp
│   │   └── ...
│   ├── mocks/                # native用モック
│   │   └── mock_*.h/.cpp
│   └── test_framework.h      # テスト共通ヘッダ等
├── platformio.ini
└── ...
```

- **src/**: 実装と内部ヘッダ
- **include/**: 外部公開ヘッダ（必要に応じて）
- **lib/**: サードパーティや自作ライブラリ
- **test/pure/**: native環境での純粋ロジックテスト（Unityベース）
- **test/integration/**: 実機（M5Stack Fire等）での統合テスト
- **test/mocks/**: nativeテスト用のモック実装
- **test_framework.h**: テスト共通のヘルパーやマクロ

## 2. platformio.ini 設定例

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

## 3. テストの書き方

- 各機能ごとに `test/pure/test_<feature>_pure.cpp`（native用）、`test/integration/test_<feature>_integration.cpp`（実機用）を作成
- テスト用モックは `test/mocks/` に配置し、nativeテストで利用
- テストは `platformio test` で一括実行

## 4. LDF（Library Dependency Finder）設定

- デフォルトの `LDF Mode = chain+` で十分
- 特殊な依存がある場合のみ `lib_ldf_mode` を明示

## 5. 移行手順

1. **不要な build_src_filter 設定を削除**
2. **src/・include/・lib/・test/ ディレクトリを整理**
3. **テストは test/pure/・test/integration/ 配下に集約し、mocks も test/mocks/ へ移動**
4. **platformio.ini を上記例に準拠して修正**
5. **ビルド・テストが正常に通ることを確認**

---

## 備考

- 既存のテストやモックは `test/` 配下に移動・整理
- `test/disabled/` など一時的なディレクトリは整理後削除
- `lib/` 配下の自作ライブラリは必要に応じて `src/` へ統合
- nativeテストと実機テストの両立により、ロジックの純粋性と実装の信頼性を両立

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
│       └── test_button_manager_pure/
│           └── test_main.cpp
├── platformio.ini
```

### トラブルシューティング
- includeエラー時は
    - ヘッダの配置場所・記法・lib/xxx/src/構成を再確認
    - platformio.iniのbuild_src_filter, build_flagsを確認
    - `pio run --target clean` でキャッシュクリア
- それでも解決しない場合は、具体的なエラー内容・include記法・ディレクトリ構成を記録し、issue化

### 備考
- LDFの詳細: https://docs.platformio.org/en/latest/librarymanager/ldf.html
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
5. **不要なキャッシュ・ビルド生成物を削除する**
    - `pio run --target clean` でクリーンビルド
    - 必要に応じて `.pio/` ディレクトリを手動削除
6. **テスト・本体ビルドが通ることを確認する**
    - `pio test` および `pio run` でエラーが出ないか確認
7. **問題が解決しない場合は、エラー内容・ディレクトリ構成・include記法を記録し、issue化**

---

この手順に従うことで、lib/構成・LDF活用のトラブルを大幅に減らすことができます。 