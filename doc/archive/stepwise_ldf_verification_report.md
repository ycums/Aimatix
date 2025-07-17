# LDF動作検証レポート: inputペア移植時

## 構成
- test_minimal_project/lib/your_lib/src/
  - input.cpp, input.h
- test_minimal_project/test/test_input_logic_pure/
  - test_main.cpp

## エラー内容
- test_main.cpp: `#include <alarm.h>` が見つからない
- input.cpp: `#include "alarm.h"` が見つからない

## include記法
- test_main.cpp: `<alarm.h>`
- input.cpp: `"alarm.h"`

## 依存関係
- input.cpp → alarm.h
- test_main.cpp → alarm.h

## 考察・原因
- alarm.hがtest_minimal_project/lib/your_lib/src/に存在しない（依存ペア未投入） 


# LDF動作検証レポート: settingsペア移植時

## 構成
- test_minimal_project/lib/your_lib/src/
  - settings.cpp, settings.h
- test_minimal_project/test/test_settings_logic_pure/
  - test_main.cpp

## エラー内容
- test_main.cpp: `#include <EEPROM.h>` が見つからない
- settings.h: `#include <EEPROM.h>` が見つからない

## include記法
- test_main.cpp: `<EEPROM.h>`
- settings.h: `<EEPROM.h>`

## 依存関係
- settings.h → EEPROM.h
- test_main.cpp → EEPROM.h

## 考察・原因
- EEPROM.hがtest_minimal_project/lib/your_lib/src/に存在しない 


# LDF動作検証レポート: time_logicペア移植時

## 構成
- test_minimal_project/lib/your_lib/src/
  - time_logic.cpp, time_logic.h
- test_minimal_project/test/test_time_logic_simple/
  - test_main.cpp

## エラー内容
- test_main.cpp: `#include "test_framework.h"` が見つからない

## include記法
- test_main.cpp: `"test_framework.h"`

## 依存関係
- test_main.cpp → test_framework.h

## 考察・原因
- test_framework.hがtest_minimal_project/test/に存在しない 


# LDF動作検証レポート: テストディレクトリ構造問題

## 構成
- test_minimal_project/test/
  - test_main.cpp
  - test_button_manager_pure_test_main.cpp
  - mocks/

## エラー内容
- リンカーエラー: `multiple definition of main`
- 複数のtest_main.cppが同じディレクトリに存在

## 原因
- PlatformIOのテスト構成ベストプラクティスに違反
- 複数のtest_main.cppを同じディレクトリに配置している

## 解決策
### PlatformIOのテスト構成ベストプラクティス

#### 正しいディレクトリ構造
```
test_minimal_project/test/
├── test_button_manager_pure/     # 独立したディレクトリ
│   └── test_main.cpp            # ButtonManager専用テスト
├── test_alarm_logic_pure/       # 独立したディレクトリ
│   └── test_main.cpp            # Alarm専用テスト
└── mocks/
    ├── mock_button_manager.h
    └── ...
```

#### 重要なポイント
- **各テストは独立したディレクトリに配置**: 複数のtest_main.cppを同じディレクトリに配置しない
- **ディレクトリ名は`test_`プレフィックスで開始**: PlatformIOが自動検出するため
- **各test_main.cppは単一の責務**: 特定のモジュール/機能のテストのみ

#### 修正手順
1. 不要なtest_main.cppを削除
2. 各テストを独立したディレクトリに配置
3. ディレクトリ名を`test_`プレフィックスで開始
4. `pio test`でビルド・テスト確認

## 学んだこと
- PlatformIOでは、テストごとに独立したディレクトリとtest_main.cppを作成するのが標準的なアプローチ
- 複数のtest_main.cppを同じディレクトリに配置するとリンカーエラーが発生
- テストディレクトリの命名規則は重要（`test_`プレフィックス必須）
- 各test_main.cppは単一の責務を持つべき 