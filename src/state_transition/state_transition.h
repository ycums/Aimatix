#ifndef STATE_TRANSITION_H
#define STATE_TRANSITION_H

#include "button_event.h"
#include "system_state.h"
#include "transition_result.h"
#include "transition_validator.h"

// 状態遷移マネージャークラス
class StateTransitionManager {
public:
  // メインの状態遷移処理
  static TransitionResult handleStateTransition(
    Mode currentMode, 
    const ButtonEvent& event, 
    const SystemState& systemState
  );
  
private:
  // 各モード用の遷移ハンドラー
  static TransitionResult handleMainDisplayTransition(const ButtonEvent& event, const SystemState& state);
  static TransitionResult handleAbsTimeInputTransition(const ButtonEvent& event, const SystemState& state);
  static TransitionResult handleRelPlusTimeInputTransition(const ButtonEvent& event, const SystemState& state);
  static TransitionResult handleAlarmManagementTransition(const ButtonEvent& event, const SystemState& state);
  static TransitionResult handleSettingsMenuTransition(const ButtonEvent& event, const SystemState& state);
  static TransitionResult handleInfoDisplayTransition(const ButtonEvent& event, const SystemState& state);
  static TransitionResult handleWarningColorTestTransition(const ButtonEvent& event, const SystemState& state);
  static TransitionResult handleAlarmActiveTransition(const ButtonEvent& event, const SystemState& state);
  
  // 共通処理
  static TransitionResult handleCommonCButtonTransition(const ButtonEvent& event, const SystemState& state);
};

#endif // STATE_TRANSITION_H 