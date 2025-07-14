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