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