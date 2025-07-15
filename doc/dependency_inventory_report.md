# 純粋ロジック外部依存棚卸しリスト

## 1. alarm.cpp
- #include <M5Stack.h>
- #include "settings.h"
- #include "../test/mocks/mock_m5stack.h"

## 2. alarm.h
- #include <vector>
- #include <time.h>
- #include <algorithm>

## 3. button_manager.cpp
- #include <M5Stack.h>
- #include "../test/mocks/mock_m5stack.h"

## 4. button_manager.h
- #include <map>
- #include <M5Stack.h>
- #include "debounce_manager.h"
- #include "../test/mocks/mock_m5stack.h"

## 5. debounce_manager.cpp
- #include "debounce_manager.h"

## 6. debounce_manager.h
- #include <map>
- #include <string>
- #include <M5Stack.h>
- #include "../test/mocks/mock_m5stack.h"

## 7. input.cpp
- #include "alarm.h"
- #include <cstdint>
- #include "button_manager.h"
- // #include <M5Stack.h>  // mock_m5stack.hで代替

## 8. input.h
- #include <time.h>

## 9. settings.h
- #include <EEPROM.h>
- #include <M5Stack.h>
- #include <cstdint>

## 10. time_logic.h
- #include <vector>
- #include <ctime>

---

### 【まとめ】
- M5Stack.h, EEPROM.h, mock_m5stack.h への依存が alarm, button_manager, debounce_manager, settings で多数存在
- input.cppはM5Stack.h依存をコメントアウトしmockで代替しているが、他は未分離
- 完全な純粋ロジック分離には、これら外部依存の排除・mock/DI化が必須 