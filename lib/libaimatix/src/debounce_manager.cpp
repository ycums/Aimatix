#include "debounce_manager.h"

// 静的メンバ変数の定義
std::map<std::string, unsigned long> DebounceManager::lastOperationTimes;
unsigned long DebounceManager::lastModeChangeTime = 0;
std::map<ButtonType, unsigned long> DebounceManager::lastButtonChangeTimes;

// ハードウェアレベルのデバウンス判定
bool DebounceManager::canProcessHardware(ButtonType buttonId, unsigned long (*getTime)()) {
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
  printf("DEBUG: currentTime=%lu, lastModeChangeTime=%lu\n", currentTime, lastModeChangeTime);
  // 初回呼び出し時（lastModeChangeTimeが0）は常にtrue
  if (lastModeChangeTime == 0) {
    lastModeChangeTime = currentTime;
    printf("DEBUG: 初回呼び出し、lastModeChangeTimeを%luに設定\n", lastModeChangeTime);
    return true;
  }
  if (currentTime - lastModeChangeTime >= DEFAULT_MODE_CHANGE_DEBOUNCE) {
    lastModeChangeTime = currentTime;
    printf("DEBUG: デバウンス時間経過、lastModeChangeTimeを%luに更新\n", lastModeChangeTime);
    return true;
  }
  printf("DEBUG: デバウンス時間未経過、falseを返す\n");
  return false;
}

// テスト用：状態リセット
void DebounceManager::reset() {
  printf("DEBUG: reset() 呼び出し前 - lastModeChangeTime=%lu\n", lastModeChangeTime);
  lastOperationTimes.clear();
  lastModeChangeTime = 0;
  lastButtonChangeTimes.clear();
  printf("DEBUG: reset() 呼び出し後 - lastModeChangeTime=%lu\n", lastModeChangeTime);
}