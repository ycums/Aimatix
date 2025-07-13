#ifndef TRANSITION_VALIDATOR_H
#define TRANSITION_VALIDATOR_H

#include "button_event.h"
#include "system_state.h"
#include "transition_result.h"

// 遷移妥当性チェッククラス
class TransitionValidator {
public:
  // 遷移の妥当性をチェック
  static bool isValidTransition(Mode currentMode, const ButtonEvent& event, const SystemState& state);
  
  // 警告表示中の遷移をチェック
  static bool isWarningBlockingTransition(const SystemState& state);
  
  // アラーム鳴動中の遷移をチェック
  static bool isAlarmBlockingTransition(const SystemState& state);
  
  // 境界値チェック
  static bool isIndexInRange(int index, int maxValue);
  static bool isAlarmCountValid(size_t count);
  
  // エラーメッセージの生成
  static const char* getErrorMessage(Mode currentMode, const ButtonEvent& event, const SystemState& state);
};

#endif // TRANSITION_VALIDATOR_H 