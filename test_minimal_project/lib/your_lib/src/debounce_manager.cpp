#include "debounce_manager.h"

// 静的メンバ変数の定義
std::map<std::string, unsigned long> DebounceManager::lastOperationTimes;
unsigned long DebounceManager::lastModeChangeTime = 0;
std::map<int, unsigned long> DebounceManager::lastButtonChangeTimes;

// ハードウェアレベルのデバウンス判定
bool DebounceManager::canProcessHardware(int buttonId, unsigned long (*getTime)()) {
  unsigned long currentTime = getTime();
  auto it = lastButtonChangeTimes.find(buttonId);
  if (it == lastButtonChangeTimes.end() || currentTime - it->second >= DEFAULT_HARDWARE_DEBOUNCE) {
    lastButtonChangeTimes[buttonId] = currentTime;
    return true;
  }
  return false;
}

// 操作レベルのデバウンス判定
bool DebounceManager::canProcessOperation(const std::string& operationType, unsigned long (*getTime)()) {
  unsigned long currentTime = getTime();
  auto it = lastOperationTimes.find(operationType);
  if (it == lastOperationTimes.end()) {
    lastOperationTimes[operationType] = currentTime;
    return true;
  }
  if (currentTime - it->second >= DEFAULT_OPERATION_DEBOUNCE) {
    it->second = currentTime;
    return true;
  }
  return false;
}

// 画面遷移レベルのデバウンス判定
bool DebounceManager::canProcessModeChange(unsigned long (*getTime)()) {
  unsigned long currentTime = getTime();
  if (currentTime - lastModeChangeTime >= DEFAULT_MODE_CHANGE_DEBOUNCE) {
    lastModeChangeTime = currentTime;
    return true;
  }
  return false;
}