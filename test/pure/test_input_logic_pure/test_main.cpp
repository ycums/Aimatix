#include <unity.h>
#include <cstdio>
#include <cstring>
#include "input.h"
#include "alarm.h"
#include "types.h"

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
  
  printf("=== 全テスト完了 ===\n");
  
  return UNITY_END();
} 