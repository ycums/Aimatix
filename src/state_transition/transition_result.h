#ifndef TRANSITION_RESULT_H
#define TRANSITION_RESULT_H

#include "types.h"

// 遷移アクションの定義
enum TransitionAction {
  ACTION_NONE,           // 何もしない
  ACTION_RESET_INPUT,    // 入力状態リセット
  ACTION_ADD_ALARM,      // アラーム追加
  ACTION_DELETE_ALARM,   // アラーム削除
  ACTION_UPDATE_SETTINGS, // 設定更新
  ACTION_PLAY_ALARM,     // アラーム鳴動
  ACTION_STOP_ALARM,     // アラーム停止
  ACTION_SHOW_WARNING,   // 警告表示
  ACTION_CLEAR_WARNING   // 警告クリア
};

// 遷移結果構造体
struct TransitionResult {
  Mode nextMode;                    // 次のモード
  bool isValid;                     // 遷移が有効か
  const char* errorMessage;         // エラーメッセージ
  TransitionAction action;          // 実行するアクション
  int actionParameter;              // アクションパラメータ
  
  TransitionResult() 
    : nextMode(MAIN_DISPLAY), isValid(false), errorMessage(nullptr),
      action(ACTION_NONE), actionParameter(0) {}
      
  TransitionResult(Mode mode, bool valid, const char* error = nullptr,
                   TransitionAction act = ACTION_NONE, int param = 0)
    : nextMode(mode), isValid(valid), errorMessage(error),
      action(act), actionParameter(param) {}
};

// 遷移結果作成関数の宣言
TransitionResult createValidTransition(Mode nextMode, TransitionAction action = ACTION_NONE, int parameter = 0);
TransitionResult createInvalidTransition(const char* errorMessage);
TransitionResult createNoChangeTransition();

#endif // TRANSITION_RESULT_H 