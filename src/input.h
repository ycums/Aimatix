#ifndef INPUT_H
#define INPUT_H

// 入力モード用の構造体
struct InputState {
  int hours = 0;
  int minutes = 0;
  int currentDigit = 0;  // 0-3: HH:MM
  bool inputting = false;
};

// グローバル変数
extern InputState inputState;

// 関数プロトタイプ
void resetInput();

#endif // INPUT_H
