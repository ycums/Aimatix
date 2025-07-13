#include "debounce_manager.h"

// 静的メンバ変数の定義
std::map<String, unsigned long> DebounceManager::lastOperationTimes;
unsigned long DebounceManager::lastModeChangeTime = 0;

// ハードウェアレベルのデバウンス判定
bool DebounceManager::canProcessHardware(Button& button) {
  // 現在の時刻
  unsigned long currentTime = millis();
  
  // ボタンの最後の状態変化時刻を取得（簡易実装）
  // 実際の実装では、ButtonManagerと連携して正確な時刻を取得する
  static unsigned long lastButtonChangeTime = 0;
  
  // 前回の処理から一定時間経過しているかチェック
  if (currentTime - lastButtonChangeTime >= DEFAULT_HARDWARE_DEBOUNCE) {
    lastButtonChangeTime = currentTime;
    return true;
  }
  
  return false;
}

// 操作レベルのデバウンス判定
bool DebounceManager::canProcessOperation(const String& operationType) {
  unsigned long currentTime = millis();
  
  // 指定された操作タイプの最後の処理時刻を取得
  auto it = lastOperationTimes.find(operationType);
  if (it == lastOperationTimes.end()) {
    // 初回の場合は処理可能
    lastOperationTimes[operationType] = currentTime;
    return true;
  }
  
  // 前回の処理から一定時間経過しているかチェック
  if (currentTime - it->second >= DEFAULT_OPERATION_DEBOUNCE) {
    it->second = currentTime;
    return true;
  }
  
  return false;
}

// 画面遷移レベルのデバウンス判定
bool DebounceManager::canProcessModeChange() {
  unsigned long currentTime = millis();
  
  // 前回のモード変更から一定時間経過しているかチェック
  if (currentTime - lastModeChangeTime >= DEFAULT_MODE_CHANGE_DEBOUNCE) {
    lastModeChangeTime = currentTime;
    return true;
  }
  
  return false;
} 