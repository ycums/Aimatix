#include "state_transition.h"

// メインの状態遷移処理
TransitionResult StateTransitionManager::handleStateTransition(
  Mode currentMode, 
  const ButtonEvent& event, 
  const SystemState& systemState
) {
  // 遷移の妥当性をチェック
  if (!TransitionValidator::isValidTransition(currentMode, event, systemState)) {
    const char* errorMessage = TransitionValidator::getErrorMessage(currentMode, event, systemState);
    return createInvalidTransition(errorMessage);
  }
  
  // 現在のモードに応じて適切なハンドラーを呼び出し
  switch (currentMode) {
    case MAIN_DISPLAY:
      return handleMainDisplayTransition(event, systemState);
      
    case ABS_TIME_INPUT:
      return handleAbsTimeInputTransition(event, systemState);
      
    case REL_PLUS_TIME_INPUT:
      return handleRelPlusTimeInputTransition(event, systemState);
      
    case ALARM_MANAGEMENT:
      return handleAlarmManagementTransition(event, systemState);
      
    case SETTINGS_MENU:
      return handleSettingsMenuTransition(event, systemState);
      
    case INFO_DISPLAY:
      return handleInfoDisplayTransition(event, systemState);
      
    case WARNING_COLOR_TEST:
      return handleWarningColorTestTransition(event, systemState);
      
    case ALARM_ACTIVE:
      return handleAlarmActiveTransition(event, systemState);
      
    default:
      return createInvalidTransition("未対応のモード");
  }
}

// メイン画面の遷移ハンドラー（完全版）
TransitionResult StateTransitionManager::handleMainDisplayTransition(const ButtonEvent& event, const SystemState& state) {
  switch (event.button) {
    case BUTTON_A:
      if (event.action == SHORT_PRESS) {
        // A短押し: 絶対時刻入力画面へ
        return createValidTransition(ABS_TIME_INPUT, ACTION_RESET_INPUT);
      }
      break;
      
    case BUTTON_B:
      if (event.action == SHORT_PRESS) {
        // B短押し: 相対時刻加算入力画面へ
        return createValidTransition(REL_PLUS_TIME_INPUT, ACTION_RESET_INPUT);
      } else if (event.action == LONG_PRESS) {
        // B長押し: 何もしない（同じ画面に留まる）
        return createNoChangeTransition();
      }
      break;
      
    case BUTTON_C:
      if (event.action == SHORT_PRESS) {
        // C短押し: アラーム管理画面へ
        return createValidTransition(ALARM_MANAGEMENT);
      } else if (event.action == LONG_PRESS) {
        // C長押し: 設定メニューへ
        return createValidTransition(SETTINGS_MENU);
      }
      break;
  }
  
  return createNoChangeTransition();
}

// 絶対時刻入力の遷移ハンドラー（完全版）
TransitionResult StateTransitionManager::handleAbsTimeInputTransition(const ButtonEvent& event, const SystemState& state) {
  switch (event.button) {
    case BUTTON_A:
      if (event.action == SHORT_PRESS) {
        // A短押し: 数字変更（同じ画面に留まる）
        return createNoChangeTransition();
      } else if (event.action == LONG_PRESS) {
        // A長押し: 数字を+5（同じ画面に留まる）
        return createNoChangeTransition();
      }
      break;
      
    case BUTTON_B:
      if (event.action == SHORT_PRESS) {
        // B短押し: 桁送り（同じ画面に留まる）
        return createNoChangeTransition();
      } else if (event.action == LONG_PRESS) {
        // B長押し: 入力値をリセット（同じ画面に留まる）
        return createNoChangeTransition();
      }
      break;
      
    case BUTTON_C:
      if (event.action == SHORT_PRESS) {
        // C短押し: 入力確定・アラーム追加
        return createValidTransition(MAIN_DISPLAY, ACTION_ADD_ALARM);
      } else if (event.action == LONG_PRESS) {
        // C長押し: キャンセル
        return createValidTransition(MAIN_DISPLAY);
      }
      break;
  }
  
  return createNoChangeTransition();
}

// 相対時刻加算入力の遷移ハンドラー（完全版）
TransitionResult StateTransitionManager::handleRelPlusTimeInputTransition(const ButtonEvent& event, const SystemState& state) {
  switch (event.button) {
    case BUTTON_A:
      if (event.action == SHORT_PRESS) {
        // A短押し: 数字変更（同じ画面に留まる）
        return createNoChangeTransition();
      } else if (event.action == LONG_PRESS) {
        // A長押し: 数字を+5（同じ画面に留まる）
        return createNoChangeTransition();
      }
      break;
      
    case BUTTON_B:
      if (event.action == SHORT_PRESS) {
        // B短押し: 桁送り（同じ画面に留まる）
        return createNoChangeTransition();
      } else if (event.action == LONG_PRESS) {
        // B長押し: 入力値をリセット（同じ画面に留まる）
        return createNoChangeTransition();
      }
      break;
      
    case BUTTON_C:
      if (event.action == SHORT_PRESS) {
        // C短押し: 入力確定・アラーム追加
        return createValidTransition(MAIN_DISPLAY, ACTION_ADD_ALARM);
      } else if (event.action == LONG_PRESS) {
        // C長押し: キャンセル
        return createValidTransition(MAIN_DISPLAY);
      }
      break;
  }
  
  return createNoChangeTransition();
}

// アラーム管理の遷移ハンドラー（完全版）
TransitionResult StateTransitionManager::handleAlarmManagementTransition(const ButtonEvent& event, const SystemState& state) {
  // アラームが存在しない場合はメイン画面に戻る
  if (state.alarmCount == 0) {
    return createValidTransition(MAIN_DISPLAY);
  }
  
  switch (event.button) {
    case BUTTON_A:
      if (event.action == SHORT_PRESS) {
        // A短押し: 前の項目へ（同じ画面に留まる）
        return createNoChangeTransition();
      }
      break;
      
    case BUTTON_B:
      if (event.action == SHORT_PRESS) {
        // B短押し: 次の項目へ（同じ画面に留まる）
        return createNoChangeTransition();
      }
      break;
      
    case BUTTON_C:
      if (event.action == SHORT_PRESS) {
        // C短押し: 削除確認（同じ画面に留まる）
        return createNoChangeTransition();
      } else if (event.action == LONG_PRESS) {
        // C長押し: メイン画面に戻る
        return createValidTransition(MAIN_DISPLAY);
      }
      break;
  }
  
  return createNoChangeTransition();
}

// 設定メニューの遷移ハンドラー（完全版）
TransitionResult StateTransitionManager::handleSettingsMenuTransition(const ButtonEvent& event, const SystemState& state) {
  switch (event.button) {
    case BUTTON_A:
      if (event.action == SHORT_PRESS) {
        // A短押し: 前の項目へ（同じ画面に留まる）
        return createNoChangeTransition();
      }
      break;
      
    case BUTTON_B:
      if (event.action == SHORT_PRESS) {
        // B短押し: 次の項目へ（同じ画面に留まる）
        return createNoChangeTransition();
      }
      break;
      
    case BUTTON_C:
      if (event.action == SHORT_PRESS) {
        // C短押し: 設定変更/画面遷移
        switch (state.settingsMenu.selectedItem) {
          case 0: // Sound
            return createNoChangeTransition(); // 設定変更（同じ画面に留まる）
          case 1: // LCD Brightness
            return createNoChangeTransition(); // 設定変更（同じ画面に留まる）
          case 2: // Warning Color Test
            return createValidTransition(WARNING_COLOR_TEST);
          case 3: // All Clear
            return createNoChangeTransition(); // 確認ダイアログ（同じ画面に留まる）
          case 4: // Info
            return createValidTransition(INFO_DISPLAY);
          default:
            return createNoChangeTransition();
        }
      }
      break;
  }
  
  return createNoChangeTransition();
}

// 情報表示の遷移ハンドラー（完全版）
TransitionResult StateTransitionManager::handleInfoDisplayTransition(const ButtonEvent& event, const SystemState& state) {
  // 任意ボタンでメイン画面へ戻る
  if (event.action == SHORT_PRESS || event.action == LONG_PRESS) {
    return createValidTransition(MAIN_DISPLAY);
  }
  
  return createNoChangeTransition();
}

// 警告色テストの遷移ハンドラー（完全版）
TransitionResult StateTransitionManager::handleWarningColorTestTransition(const ButtonEvent& event, const SystemState& state) {
  if (event.button == BUTTON_C && event.action == SHORT_PRESS) {
    return createValidTransition(SETTINGS_MENU);
  }
  
  return createNoChangeTransition();
}

// アラーム鳴動中の遷移ハンドラー（完全版）
TransitionResult StateTransitionManager::handleAlarmActiveTransition(const ButtonEvent& event, const SystemState& state) {
  // 任意ボタンでアラーム停止
  if (event.action == SHORT_PRESS || event.action == LONG_PRESS) {
    return createValidTransition(MAIN_DISPLAY, ACTION_STOP_ALARM);
  }
  
  return createNoChangeTransition();
}

// 共通Cボタン処理（基本版）
TransitionResult StateTransitionManager::handleCommonCButtonTransition(const ButtonEvent& event, const SystemState& state) {
  if (event.action == LONG_PRESS) {
    return createValidTransition(MAIN_DISPLAY);
  }
  
  return createNoChangeTransition();
} 