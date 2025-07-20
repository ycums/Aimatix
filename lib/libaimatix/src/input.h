#ifndef INPUT_H
#define INPUT_H

#include <time.h>
#include <cstdint>
#include "../include/IButtonManager.h"

// 入力モード用の構造体
struct InputState {
  int hours = 0;
  int minutes = 0;
  int currentDigit = 0;  // 0-3: HH:MM
  bool inputting = false;
};

// 右詰め入力用の状態
struct RightJustifiedInputState {
  char digits[5] = ""; // 最大4桁＋終端
  int numDigits = 0;    // 現在の入力桁数
  bool error = false;   // 入力エラー状態
  time_t predictedTime = 0; // 予測される時刻
};

// 桁ごと編集方式の時刻入力状態
struct DigitEditTimeInputState {
  int hourTens = 0;
  int hourOnes = 0;
  int minTens = 0;
  int minOnes = 0;
  int cursor = 3; // 0:時十, 1:時一, 2:分十, 3:分一
  // 初期値（00:00）
  static constexpr int INIT_HOUR_TENS = 0;
  static constexpr int INIT_HOUR_ONES = 0;
  static constexpr int INIT_MIN_TENS = 0;
  static constexpr int INIT_MIN_ONES = 0;
};

// 画面遷移や確定イベントを示す結果型
enum class InputEventResult {
    None,
    Confirmed,
    Cancelled
};

// グローバル変数
extern InputState inputState;
extern RightJustifiedInputState rjInputState;
extern DigitEditTimeInputState digitEditInput;

// 時間取得関数の型定義
typedef uint32_t (*TimeFunction)();

// 関数プロトタイプ
bool confirmInputAndAddAlarm();
void resetInput();
InputEventResult handleDigitEditInput(IButtonManager* buttonManager, TimeFunction timeFunc, DigitEditTimeInputState& state);

#endif // INPUT_H
