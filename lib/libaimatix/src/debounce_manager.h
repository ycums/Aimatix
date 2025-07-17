#ifndef DEBOUNCE_MANAGER_H
#define DEBOUNCE_MANAGER_H

#include <map>
#include <string>

// DebounceManagerクラス（静的クラス）
class DebounceManager {
public:
  // 階層別デバウンス判定
  static bool canProcessHardware(int buttonId);
  static bool canProcessOperation(const std::string& operationType);
  static bool canProcessModeChange();
  
private:
  static const unsigned long DEFAULT_HARDWARE_DEBOUNCE = 50;
  static const unsigned long DEFAULT_OPERATION_DEBOUNCE = 200;
  static const unsigned long DEFAULT_MODE_CHANGE_DEBOUNCE = 300;
  
  static std::map<std::string, unsigned long> lastOperationTimes;
  static unsigned long lastModeChangeTime;
  static std::map<int, unsigned long> lastButtonChangeTimes;
};

#endif // DEBOUNCE_MANAGER_H 