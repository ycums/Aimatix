# Aimatix PlatformIO/Unity/LDF ベストプラクティス最新版（2024/07）

## 1. ディレクトリ構成（現状準拠）

```
Aimatix/
├── lib/
│   └── libaimatix/
│       └── src/
│           ├── button_manager.h/.cpp
│           ├── alarm.h/.cpp
│           ├── debounce_manager.h/.cpp
│           ├── input.h/.cpp
│           ├── settings.h/.cpp
│           ├── time_logic.h/.cpp
│           └── types.h
├── src/
│   ├── main.cpp
│   ├── ui.cpp/.h
│   ├── time_sync.cpp/.h
│   ├── wifi_manager.cpp/.h
│   └── state_transition/
│       ├── state_transition.cpp/.h
│       ├── button_event.cpp/.h
│       ├── system_state.cpp/.h
│       ├── transition_result.cpp/.h
│       └── transition_validator.cpp/.h
├── test/
│   ├── pure/
│   │   ├── test_button_manager_pure/
│   │   │   ├── test_main.cpp
│   │   │   ├── mock_button_manager.h/.cpp
│   │   │   └── mock_time.h
│   │   ├── test_alarm_logic_pure/
│   │   │   └── test_main.cpp
│   │   ├── test_debounce_manager_pure/
│   │   │   └── test_main.cpp
│   │   ├── test_input_logic_pure/
│   │   │   └── test_main.cpp
│   │   ├── test_settings_logic_pure/
│   │   │   └── test_main.cpp
│   │   └── test_time_logic_simple/
│   │       └── test_main.cpp
│   ├── mock/
│   │   ├── mock_button_manager.h/.cpp
│   │   ├── mock_m5stack.h
│   │   └── mock_time.h
│   ├── integration/          # 実機向け統合テスト
│   │   ├── test_integration.cpp
│   │   └── ...
│   └── unity_config.h
├── platformio.ini
└── ...
```

- **lib/libaimatix/src/**: 純粋ロジック（ハード非依存）
- **src/**: M5Stack依存の本体実装
- **test/pure/**: 各テストごとに独立ディレクトリ（test_main.cpp必須、必要に応じて固有mockも同梱）
- **test/mock/**: 共通モック（複数テストで使うもの）
- **test/integration/**: 実機（M5Stack Fire等）での統合テスト
- **test/unity_config.h**: Unity設定
- **test/integration/**: 現状未使用

## 2. テストディレクトリ構造・運用ルール

- **test/pure/**配下は「各テストごとに独立ディレクトリ＋test_main.cpp」
  - 例: test_button_manager_pure/test_main.cpp
- **mockの配置**
  - 複数テストで使う共通モックは test/mock/ に集約
  - テスト固有のmockは test/pure/各ディレクトリ内に配置可
- **test/integration/**は現状未使用（必要時のみ作成）
- **test/unity_config.h**は全テスト共通で利用

## 3. platformio.ini 設定（現状）

```ini
[platformio]
default_envs = m5stack-fire

[env:m5stack-fire]
platform = espressif32
board = m5stack-fire
framework = arduino
lib_deps =
    m5stack/M5Stack @ ^0.4.3
    WiFiProv
    arduino-libraries/NTPClient @ ^3.2.1
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
    -DUNITY_INCLUDE_DOUBLE
    -DUNITY_DOUBLE_PRECISION=1e-12
    -DTEST_MODE
    -DARDUINO=100
    -std=c++11
lib_deps =
    throwtheswitch/Unity @ ^2.5.2
build_unflags = -std=gnu++11 

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

[env:unit-test-esp32]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps =
    throwtheswitch/Unity @ ^2.5.2
    m5stack/M5Stack @ ^0.4.3
build_flags = 
    -DTEST_MODE
    -DCORE_DEBUG_LEVEL=3
    -DUNITY_INCLUDE_CONFIG_H
test_framework = unity
```

- **build_src_filter**等は未使用。LDF（Library Dependency Finder）でlib/配下の自作ロジックを自動認識。
- **test/pure/**配下の各ディレクトリにtest_main.cppが1つずつ。
- **mockのインクルード**は "mock/xxx.h" または "mock_button_manager.h" などで相対パス指定。

## 4. テスト・モック運用のベストプラクティス

- **各テストはtest/pure/配下の独立ディレクトリ＋test_main.cppで管理**
- **共通モックはtest/mock/、固有モックは必要に応じて各テストディレクトリ内に配置**
- **lib/配下の純粋ロジックはlib/libaimatix/src/に統一し、LDFで自動認識**
- **platformio.iniでbuild_src_filterや-Ilib/xxx/src等の追加指定は原則不要**
- **テスト追加・リファクタ時も常に全テストがグリーンになることを重視**

## 5. トラブルシューティング・運用Tips

- **includeエラー時**
  - ヘッダの配置場所・記法・lib/xxx/src/構成を再確認
  - platformio.iniのbuild_src_filter, build_flagsを確認
  - `pio run --target clean` でキャッシュクリア
- **テストエラー時**
  - テストディレクトリ構造が正しいか（test_main.cppが1つ/ディレクトリ）
  - モックファイルの配置とインクルードパスを確認
- **LDFの詳細**: https://docs.platformio.org/en/latest/librarymanager/ldf.html
- **PlatformIOテストの詳細**: https://docs.platformio.org/en/latest/advanced/unit-testing/index.html

---

このドキュメントはAimatix本体の現状構成を正とし、今後の運用・レビュー時の基準とする。

## 6. コーディング規約

コーディング規約の詳細は `../guide/developer_guide.md` を参照してください。
特に、ファイル名の命名規則（クラス名をスネークケースに変換）を遵守してください。 

## 7. 静的解析・品質管理

### 7.1 Clang-Tidy静的解析
- **実行環境**: native環境での静的解析を推奨
- **実行タイミング**: 各開発ステップで実行
- **設定ファイル**: `.clang-tidy`でプロジェクト固有の設定を管理
- **警告管理**: 高重要度警告は0件、中重要度警告は適切に管理

### 7.2 品質管理フロー
```bash
# 開発中の品質チェック
pio test -e native  # テスト実行
pio check -e native  # 静的解析実行
python scripts/test_coverage.py --quick  # カバレッジ測定
```

## 8. 今後のポイント

- enumや構造体の定義はlib配下で一元管理し、src/や他のlibからは必ずそのヘッダをインクルードすること
- プロジェクト全体で重複定義を絶対に避けること（特にButtonAction等のenumはlibで一元化）
- PlatformIOのlib/include解決の癖に注意し、相対パスや<>の使い分けを徹底すること
- 静的解析とカバレッジ測定を開発フローに統合し、継続的な品質向上を図ること 