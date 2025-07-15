#ifndef DEBOUNCE_MANAGER_H
#define DEBOUNCE_MANAGER_H

#include <map>
#include <string>

// テスト環境と実環境の切り替え
#ifdef TESTING
  #include "../test/mocks/mock_m5stack.h"
  using Button = MockM5Stack::Button;
#else
  #include <M5Stack.h>
#endif

// DebounceManagerクラス（静的クラス）
class DebounceManager {
public:
  // 階層別デバウンス判定
  static bool canProcessHardware(Button& button);
  static bool canProcessOperation(const std::string& operationType);
  static bool canProcessModeChange();
  
private:
  // デフォルトのデバウンス時間
  static const unsigned long DEFAULT_HARDWARE_DEBOUNCE = 50;
  static const unsigned long DEFAULT_OPERATION_DEBOUNCE = 200;
  static const unsigned long DEFAULT_MODE_CHANGE_DEBOUNCE = 300;
  
  // 各操作タイプの最後の処理時刻
  static std::map<std::string, unsigned long> lastOperationTimes;
  static unsigned long lastModeChangeTime;
};

#endif // DEBOUNCE_MANAGER_H 