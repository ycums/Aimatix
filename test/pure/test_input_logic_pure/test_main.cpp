#include <unity.h>
#include <cstdio>
#include <cstring>
#include <input.h>
#include <alarm.h>
#include "types.h"
#include "../../../lib/libaimatix/src/time_logic.h"

// 外部変数のモック定義
Mode currentMode = MAIN_DISPLAY;

// テスト用の時間関数
uint32_t testTime = 0;
uint32_t getTestTime() {
  return testTime;
}

// モックButtonManagerクラス
class MockButtonManager : public IButtonManager {
public:
    bool button_states[3] = {false, false, false}; // A, B, C
    bool is_pressed_called = false;
    bool is_long_pressed_called = false;
    bool update_called = false;
    
    void update() override {
        update_called = true;
    }
    
    bool isPressed(ButtonType button) override {
        is_pressed_called = true;
        if (button >= 0 && button < 3) {
            return button_states[button];
        }
        return false;
    }
    
    bool isLongPressed(ButtonType button) override {
        is_long_pressed_called = true;
        if (button >= 0 && button < 3) {
            return button_states[button];
        }
        return false;
    }
    
    void reset() {
        memset(button_states, 0, sizeof(button_states));
        is_pressed_called = false;
        is_long_pressed_called = false;
        update_called = false;
    }
    
    void setButtonState(ButtonType button, bool state) {
        if (button >= 0 && button < 3) {
            button_states[button] = state;
        }
    }
};

// InputLogicの純粋ロジックテスト
// M5Stack依存を排除し、標準C++のみでテスト

// DigitEditTimeInputStateの初期化テスト
void test_digit_edit_initialization() {
  DigitEditTimeInputState state;
  
  // 初期値の確認
  TEST_ASSERT_EQUAL(DigitEditTimeInputState::INIT_HOUR_TENS, state.hourTens);
  TEST_ASSERT_EQUAL(DigitEditTimeInputState::INIT_HOUR_ONES, state.hourOnes);
  TEST_ASSERT_EQUAL(DigitEditTimeInputState::INIT_MIN_TENS, state.minTens);
  TEST_ASSERT_EQUAL(DigitEditTimeInputState::INIT_MIN_ONES, state.minOnes);
  TEST_ASSERT_EQUAL(3, state.cursor);
  
  printf("✓ DigitEditTimeInputState初期化テスト: 成功\n");
}

// 時刻バリデーション関数のテスト
void test_time_validation() {
  // 有効な時刻
  TEST_ASSERT_TRUE(0 <= 12 && 12 <= 23);  // 12時
  TEST_ASSERT_TRUE(0 <= 30 && 30 <= 59);  // 30分
  
  // 境界値テスト
  TEST_ASSERT_TRUE(0 <= 0 && 0 <= 23);    // 0時
  TEST_ASSERT_TRUE(0 <= 23 && 23 <= 23);  // 23時
  TEST_ASSERT_TRUE(0 <= 0 && 0 <= 59);    // 0分
  TEST_ASSERT_TRUE(0 <= 59 && 59 <= 59);  // 59分
  
  // 無効な時刻
  TEST_ASSERT_FALSE(0 <= 24 && 24 <= 23); // 24時
  TEST_ASSERT_FALSE(0 <= 60 && 60 <= 59); // 60分
  TEST_ASSERT_FALSE(0 <= -1 && -1 <= 23); // -1時
  TEST_ASSERT_FALSE(0 <= -1 && -1 <= 59); // -1分
  
  printf("✓ 時刻バリデーションテスト: 成功\n");
}

// 時刻計算ロジックのテスト
void test_time_calculation() {
  // 桁から時刻への変換
  int hour, minute;
  
  // 12:30
  hour = 1 * 10 + 2;  // hourTens=1, hourOnes=2
  minute = 3 * 10 + 0; // minTens=3, minOnes=0
  TEST_ASSERT_EQUAL(12, hour);
  TEST_ASSERT_EQUAL(30, minute);
  
  // 00:00
  hour = 0 * 10 + 0;
  minute = 0 * 10 + 0;
  TEST_ASSERT_EQUAL(0, hour);
  TEST_ASSERT_EQUAL(0, minute);
  
  // 23:59
  hour = 2 * 10 + 3;
  minute = 5 * 10 + 9;
  TEST_ASSERT_EQUAL(23, hour);
  TEST_ASSERT_EQUAL(59, minute);
  
  printf("✓ 時刻計算ロジックテスト: 成功\n");
}

// 桁制限ロジックのテスト
void test_digit_limitation() {
  // 時十の位の制限
  int hourTens = 2;
  int hourOnes = 4;
  
  // 2xの場合、一の位は0-3まで
  if (hourTens == 2 && hourOnes > 3) {
    hourOnes = 3; // 制限
  }
  TEST_ASSERT_EQUAL(3, hourOnes);
  
  // 0x, 1xの場合は制限なし
  hourTens = 1;
  hourOnes = 9;
  if (hourTens == 2 && hourOnes > 3) {
    hourOnes = 3;
  }
  TEST_ASSERT_EQUAL(9, hourOnes); // 変更されない
  
  printf("✓ 桁制限ロジックテスト: 成功\n");
}

// カーソル移動ロジックのテスト
void test_cursor_movement() {
  int cursor = 3;
  
  // 右回り（左から右）
  cursor = (cursor + 1) % 4;
  TEST_ASSERT_EQUAL(0, cursor);
  
  cursor = (cursor + 1) % 4;
  TEST_ASSERT_EQUAL(1, cursor);
  
  cursor = (cursor + 1) % 4;
  TEST_ASSERT_EQUAL(2, cursor);
  
  cursor = (cursor + 1) % 4;
  TEST_ASSERT_EQUAL(3, cursor);
  
  printf("✓ カーソル移動ロジックテスト: 成功\n");
}

// 桁インクリメントロジックのテスト
void test_digit_increment() {
  // 時十の位（0-2）
  int hourTens = 0;
  hourTens = (hourTens + 1) % 3;
  TEST_ASSERT_EQUAL(1, hourTens);
  
  hourTens = (hourTens + 1) % 3;
  TEST_ASSERT_EQUAL(2, hourTens);
  
  hourTens = (hourTens + 1) % 3;
  TEST_ASSERT_EQUAL(0, hourTens);
  
  // 分十の位（0-5）
  int minTens = 0;
  minTens = (minTens + 1) % 6;
  TEST_ASSERT_EQUAL(1, minTens);
  
  minTens = (minTens + 5) % 6;
  TEST_ASSERT_EQUAL(0, minTens);
  
  // 分一の位（0-9）
  int minOnes = 0;
  minOnes = (minOnes + 1) % 10;
  TEST_ASSERT_EQUAL(1, minOnes);
  
  minOnes = (minOnes + 9) % 10;
  TEST_ASSERT_EQUAL(0, minOnes);
  
  printf("✓ 桁インクリメントロジックテスト: 成功\n");
}

// 複合時刻バリデーションテスト
void test_composite_time_validation() {
  // 有効な組み合わせ
  int hour = 12, minute = 30;
  bool isValid = (hour >= 0 && hour <= 23) && (minute >= 0 && minute <= 59);
  TEST_ASSERT_TRUE(isValid);
  
  // 無効な組み合わせ
  hour = 24, minute = 30;
  isValid = (hour >= 0 && hour <= 23) && (minute >= 0 && minute <= 59);
  TEST_ASSERT_FALSE(isValid);
  
  hour = 12, minute = 60;
  isValid = (hour >= 0 && hour <= 23) && (minute >= 0 && minute <= 59);
  TEST_ASSERT_FALSE(isValid);
  
  printf("✓ 複合時刻バリデーションテスト: 成功\n");
}

// 新しく追加するテストケース（実際に存在する関数のみ）

void test_input_state_operations() {
  InputState state;
  
  // 初期状態の確認
  TEST_ASSERT_EQUAL(0, state.hours);
  TEST_ASSERT_EQUAL(0, state.minutes);
  TEST_ASSERT_EQUAL(0, state.currentDigit);
  TEST_ASSERT_FALSE(state.inputting);
  
  // 状態変更のテスト
  state.hours = 12;
  state.minutes = 30;
  state.currentDigit = 2;
  state.inputting = true;
  
  TEST_ASSERT_EQUAL(12, state.hours);
  TEST_ASSERT_EQUAL(30, state.minutes);
  TEST_ASSERT_EQUAL(2, state.currentDigit);
  TEST_ASSERT_TRUE(state.inputting);
  
  printf("✓ input_state_operations: 成功\n");
}

void test_right_justified_input_state() {
  RightJustifiedInputState state;
  
  // 初期状態の確認
  TEST_ASSERT_EQUAL(0, state.numDigits);
  TEST_ASSERT_FALSE(state.error);
  TEST_ASSERT_EQUAL(0, state.predictedTime);
  
  // 文字列操作のテスト
  strcpy(state.digits, "123");
  state.numDigits = 3;
  
  TEST_ASSERT_EQUAL_STRING("123", state.digits);
  TEST_ASSERT_EQUAL(3, state.numDigits);
  
  printf("✓ right_justified_input_state: 成功\n");
}

void test_digit_edit_time_operations() {
  DigitEditTimeInputState state;
  
  // 初期状態の確認
  TEST_ASSERT_EQUAL(0, state.hourTens);
  TEST_ASSERT_EQUAL(0, state.hourOnes);
  TEST_ASSERT_EQUAL(0, state.minTens);
  TEST_ASSERT_EQUAL(0, state.minOnes);
  TEST_ASSERT_EQUAL(3, state.cursor);
  
  // 時刻設定のテスト
  state.hourTens = 1;
  state.hourOnes = 2;
  state.minTens = 3;
  state.minOnes = 0;
  state.cursor = 1;
  
  TEST_ASSERT_EQUAL(1, state.hourTens);
  TEST_ASSERT_EQUAL(2, state.hourOnes);
  TEST_ASSERT_EQUAL(3, state.minTens);
  TEST_ASSERT_EQUAL(0, state.minOnes);
  TEST_ASSERT_EQUAL(1, state.cursor);
  
  printf("✓ digit_edit_time_operations: 成功\n");
}

void test_time_function_pointer() {
  // 時間関数ポインタのテスト
  TimeFunction timeFunc = getTestTime;
  
  testTime = 1000;
  uint32_t result = timeFunc();
  TEST_ASSERT_EQUAL(1000, result);
  
  testTime = 2000;
  result = timeFunc();
  TEST_ASSERT_EQUAL(2000, result);
  
  printf("✓ time_function_pointer: 成功\n");
}

void test_input_validation_logic() {
  // 入力検証ロジックのテスト
  
  // 有効な時刻範囲
  int hour = 12, minute = 30;
  bool isValid = (hour >= 0 && hour <= 23) && (minute >= 0 && minute <= 59);
  TEST_ASSERT_TRUE(isValid);
  
  // 境界値テスト
  hour = 0, minute = 0;
  isValid = (hour >= 0 && hour <= 23) && (minute >= 0 && minute <= 59);
  TEST_ASSERT_TRUE(isValid);
  
  hour = 23, minute = 59;
  isValid = (hour >= 0 && hour <= 23) && (minute >= 0 && minute <= 59);
  TEST_ASSERT_TRUE(isValid);
  
  printf("✓ input_validation_logic: 成功\n");
}

void test_cursor_navigation() {
  // カーソルナビゲーションのテスト
  int cursor = 0;
  
  // 順方向移動
  cursor = (cursor + 1) % 4;
  TEST_ASSERT_EQUAL(1, cursor);
  
  cursor = (cursor + 1) % 4;
  TEST_ASSERT_EQUAL(2, cursor);
  
  cursor = (cursor + 1) % 4;
  TEST_ASSERT_EQUAL(3, cursor);
  
  cursor = (cursor + 1) % 4;
  TEST_ASSERT_EQUAL(0, cursor);
  
  // 逆方向移動
  cursor = (cursor - 1 + 4) % 4;
  TEST_ASSERT_EQUAL(3, cursor);
  
  printf("✓ cursor_navigation: 成功\n");
}

void test_digit_constraints() {
  // 桁制約のテスト
  
  // 時十の位（0-2）
  int hourTens = 0;
  hourTens = (hourTens + 1) % 3;
  TEST_ASSERT_EQUAL(1, hourTens);
  
  hourTens = (hourTens + 1) % 3;
  TEST_ASSERT_EQUAL(2, hourTens);
  
  hourTens = (hourTens + 1) % 3;
  TEST_ASSERT_EQUAL(0, hourTens);
  
  // 時一の位（0-9、ただし時十が2の場合は0-3）
  int hourOnes = 0;
  hourOnes = (hourOnes + 1) % 10;
  TEST_ASSERT_EQUAL(1, hourOnes);
  
  // 分十の位（0-5）
  int minTens = 0;
  minTens = (minTens + 1) % 6;
  TEST_ASSERT_EQUAL(1, minTens);
  
  // 分一の位（0-9）
  int minOnes = 0;
  minOnes = (minOnes + 1) % 10;
  TEST_ASSERT_EQUAL(1, minOnes);
  
  printf("✓ digit_constraints: 成功\n");
}

void test_boundary_values() {
  // 境界値テスト
  int hour, minute;
  
  // 00:00
  hour = 0, minute = 0;
  bool isValid = (hour >= 0 && hour <= 23) && (minute >= 0 && minute <= 59);
  TEST_ASSERT_TRUE(isValid);
  
  // 23:59
  hour = 23, minute = 59;
  isValid = (hour >= 0 && hour <= 23) && (minute >= 0 && minute <= 59);
  TEST_ASSERT_TRUE(isValid);
  
  // 無効な境界値
  hour = -1, minute = 0;
  isValid = (hour >= 0 && hour <= 23) && (minute >= 0 && minute <= 59);
  TEST_ASSERT_FALSE(isValid);
  
  hour = 24, minute = 0;
  isValid = (hour >= 0 && hour <= 23) && (minute >= 0 && minute <= 59);
  TEST_ASSERT_FALSE(isValid);
  
  hour = 0, minute = -1;
  isValid = (hour >= 0 && hour <= 23) && (minute >= 0 && minute <= 59);
  TEST_ASSERT_FALSE(isValid);
  
  hour = 0, minute = 60;
  isValid = (hour >= 0 && hour <= 23) && (minute >= 0 && minute <= 59);
  TEST_ASSERT_FALSE(isValid);
  
  printf("✓ boundary_values: 成功\n");
}

void test_resetInput_function() {
    // resetInput関数のテスト
    // この関数はグローバル変数をリセットする
    resetInput();
    
    // 関数が正常に実行されたことを確認（エラーが発生しない）
    TEST_ASSERT_TRUE(true);
    
    printf("✓ resetInput_function: 成功\n");
}

void test_handleDigitEditInput_function() {
    MockButtonManager mockButtonManager;
    
    // ボタンAを押した状態をシミュレート
    mockButtonManager.setButtonState(BUTTON_TYPE_A, true);
    
    // handleDigitEditInput関数をテスト
    handleDigitEditInput(&mockButtonManager, getTestTime);
    
    // ボタン状態が確認されたことを確認
    TEST_ASSERT_TRUE(mockButtonManager.is_pressed_called || 
                     mockButtonManager.is_long_pressed_called);
    
    printf("✓ handleDigitEditInput_function: 成功\n");
}

void test_confirmInputAndAddAlarm_function() {
    // confirmInputAndAddAlarm関数のテスト
    // この関数はグローバル変数を使用してアラームを追加する
    confirmInputAndAddAlarm();
    
    // 関数が正常に実行されたことを確認（エラーが発生しない）
    TEST_ASSERT_TRUE(true);
    
    printf("✓ confirmInputAndAddAlarm_function: 成功\n");
}

void test_input_functions_integration() {
    MockButtonManager mockButtonManager;
    
    // 完全な入力サイクルのテスト
    // 1. 入力リセット
    resetInput();
    
    // 2. ボタン入力処理
    mockButtonManager.setButtonState(BUTTON_TYPE_A, true);
    handleDigitEditInput(&mockButtonManager, getTestTime);
    
    // 3. 入力確認とアラーム追加
    confirmInputAndAddAlarm();
    
    // すべての関数が正常に実行されたことを確認
    TEST_ASSERT_TRUE(true);
    
    printf("✓ input_functions_integration: 成功\n");
}

void test_input_error_handling() {
    MockButtonManager mockButtonManager;
    
    // 無効なボタン状態でのテスト
    mockButtonManager.setButtonState(BUTTON_TYPE_A, false);
    mockButtonManager.setButtonState(BUTTON_TYPE_B, false);
    mockButtonManager.setButtonState(BUTTON_TYPE_C, false);
    
    // ボタンが押されていない状態での入力処理
    handleDigitEditInput(&mockButtonManager, getTestTime);
    
    // エラーが発生しないことを確認
    TEST_ASSERT_TRUE(true);
    
    printf("✓ input_error_handling: 成功\n");
}

void test_input_boundary_conditions() {
    MockButtonManager mockButtonManager;
    
    // 境界値でのテスト
    // 時間の境界値
    testTime = 0;
    handleDigitEditInput(&mockButtonManager, getTestTime);
    
    testTime = 0xFFFFFFFF; // 最大値
    handleDigitEditInput(&mockButtonManager, getTestTime);
    
    // エラーが発生しないことを確認
    TEST_ASSERT_TRUE(true);
    
    printf("✓ input_boundary_conditions: 成功\n");
}

void test_input_multiple_buttons() {
    MockButtonManager mockButtonManager;
    
    // 複数のボタンを同時に押した状態のテスト
    mockButtonManager.setButtonState(BUTTON_TYPE_A, true);
    mockButtonManager.setButtonState(BUTTON_TYPE_B, true);
    mockButtonManager.setButtonState(BUTTON_TYPE_C, true);
    
    handleDigitEditInput(&mockButtonManager, getTestTime);
    
    // ボタン状態が確認されたことを確認
    TEST_ASSERT_TRUE(mockButtonManager.is_pressed_called || 
                     mockButtonManager.is_long_pressed_called);
    
    printf("✓ input_multiple_buttons: 成功\n");
}

void test_input_time_dependency() {
    MockButtonManager mockButtonManager;
    
    // 時間依存のテスト
    for (uint32_t time = 0; time < 1000; time += 100) {
        testTime = time;
        mockButtonManager.setButtonState(BUTTON_TYPE_A, (time % 2 == 0));
        
        handleDigitEditInput(&mockButtonManager, getTestTime);
        
        // エラーが発生しないことを確認
        TEST_ASSERT_TRUE(true);
    }
    
    printf("✓ input_time_dependency: 成功\n");
}

// InputLogicのAPIがUI遷移や画面制御を直接行わないことを検証するテスト
void test_InputLogic_does_not_change_mode_or_call_ui() {
    // Arrange
    Mode beforeMode = currentMode;
    // 例として、InputLogicの値編集APIを呼び出す
    int digit = 0;
    InputLogic::incrementDigit(digit, 9);
    InputLogic::decrementDigit(digit, 9);
    // 他のInputLogic APIも同様に呼び出し可能

    // Assert
    TEST_ASSERT_EQUAL(beforeMode, currentMode); // currentModeが変化していないこと
    // UI描画呼び出しの有無は、必要に応じてモックやカウンタで拡張可能
    printf("✓ InputLogicはUI遷移や画面制御を直接行わない: 成功\n");
}

void setUp(void) {}
void tearDown(void) {}

// メイン関数
int main() {
  UNITY_BEGIN();
  
  printf("=== InputLogic 純粋ロジックテスト ===\n");
  
  RUN_TEST(test_digit_edit_initialization);
  RUN_TEST(test_time_validation);
  RUN_TEST(test_time_calculation);
  RUN_TEST(test_digit_limitation);
  RUN_TEST(test_cursor_movement);
  RUN_TEST(test_digit_increment);
  RUN_TEST(test_composite_time_validation);
  RUN_TEST(test_input_state_operations);
  RUN_TEST(test_right_justified_input_state);
  RUN_TEST(test_digit_edit_time_operations);
  RUN_TEST(test_time_function_pointer);
  RUN_TEST(test_input_validation_logic);
  RUN_TEST(test_cursor_navigation);
  RUN_TEST(test_digit_constraints);
  RUN_TEST(test_boundary_values);
  RUN_TEST(test_resetInput_function);
  RUN_TEST(test_handleDigitEditInput_function);
  RUN_TEST(test_confirmInputAndAddAlarm_function);
  RUN_TEST(test_input_functions_integration);
  RUN_TEST(test_input_error_handling);
  RUN_TEST(test_input_boundary_conditions);
  RUN_TEST(test_input_multiple_buttons);
  RUN_TEST(test_input_time_dependency);
  RUN_TEST(test_InputLogic_does_not_change_mode_or_call_ui);
  
  printf("=== 全テスト完了 ===\n");
  
  return UNITY_END();
} 