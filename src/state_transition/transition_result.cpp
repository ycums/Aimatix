#include "transition_result.h"

// 有効な遷移結果を作成
TransitionResult createValidTransition(Mode nextMode, TransitionAction action, int parameter) {
  return TransitionResult(nextMode, true, nullptr, action, parameter);
}

// 無効な遷移結果を作成
TransitionResult createInvalidTransition(const char* errorMessage) {
  return TransitionResult(MAIN_DISPLAY, false, errorMessage, ACTION_NONE, 0);
}

// 変更なしの遷移結果を作成
TransitionResult createNoChangeTransition() {
  return TransitionResult(MAIN_DISPLAY, true, nullptr, ACTION_NONE, 0);
} 