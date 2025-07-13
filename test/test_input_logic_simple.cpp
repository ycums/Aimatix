#include <unity.h>
#include <cstdio>
#include "time_logic.h"

// InputLogicクラスの純粋ロジックテスト
// M5Stack依存を排除し、標準C++のみでテスト

// 桁インクリメントテスト
void test_digit_increment() {
  int digit = 0;
  
  // 0から9までインクリメント
  for (int i = 0; i < 10; i++) {
    TEST_ASSERT_EQUAL(i, digit);
    InputLogic::incrementDigit(digit, 9);
  }
  
  // 9から0に戻る
  TEST_ASSERT_EQUAL(0, digit);
  
  printf("✓ 桁インクリメントテスト: 成功\n");
}

// 桁デクリメントテスト
void test_digit_decrement() {
  int digit = 9;
  
  // 9から0までデクリメント
  for (int i = 9; i >= 0; i--) {
    TEST_ASSERT_EQUAL(i, digit);
    InputLogic::decrementDigit(digit, 9);
  }
  
  // 0から9に戻る
  TEST_ASSERT_EQUAL(9, digit);
  
  printf("✓ 桁デクリメントテスト: 成功\n");
}

// 時間の十の位バリデーションテスト
void test_hour_tens_validation() {
  // 有効な組み合わせ
  TEST_ASSERT_TRUE(InputLogic::isValidHourTens(0, 0));
  TEST_ASSERT_TRUE(InputLogic::isValidHourTens(0, 9));
  TEST_ASSERT_TRUE(InputLogic::isValidHourTens(1, 0));
  TEST_ASSERT_TRUE(InputLogic::isValidHourTens(1, 9));
  TEST_ASSERT_TRUE(InputLogic::isValidHourTens(2, 0));
  TEST_ASSERT_TRUE(InputLogic::isValidHourTens(2, 3));
  
  // 無効な組み合わせ
  TEST_ASSERT_FALSE(InputLogic::isValidHourTens(2, 4));
  TEST_ASSERT_FALSE(InputLogic::isValidHourTens(2, 9));
  TEST_ASSERT_FALSE(InputLogic::isValidHourTens(3, 0));
  
  printf("✓ 時間の十の位バリデーションテスト: 成功\n");
}

// 時間の一の位バリデーションテスト
void test_hour_ones_validation() {
  // 有効な組み合わせ
  TEST_ASSERT_TRUE(InputLogic::isValidHourOnes(0, 0));
  TEST_ASSERT_TRUE(InputLogic::isValidHourOnes(0, 9));
  TEST_ASSERT_TRUE(InputLogic::isValidHourOnes(1, 0));
  TEST_ASSERT_TRUE(InputLogic::isValidHourOnes(1, 9));
  TEST_ASSERT_TRUE(InputLogic::isValidHourOnes(2, 0));
  TEST_ASSERT_TRUE(InputLogic::isValidHourOnes(2, 3));
  
  // 無効な組み合わせ
  TEST_ASSERT_FALSE(InputLogic::isValidHourOnes(2, 4));
  TEST_ASSERT_FALSE(InputLogic::isValidHourOnes(2, 9));
  
  printf("✓ 時間の一の位バリデーションテスト: 成功\n");
}

// 分の十の位バリデーションテスト
void test_minute_tens_validation() {
  // 有効な値
  for (int i = 0; i <= 5; i++) {
    TEST_ASSERT_TRUE(InputLogic::isValidMinTens(i));
  }
  
  // 無効な値
  for (int i = 6; i <= 9; i++) {
    TEST_ASSERT_FALSE(InputLogic::isValidMinTens(i));
  }
  
  printf("✓ 分の十の位バリデーションテスト: 成功\n");
}

// 分の一の位バリデーションテスト
void test_minute_ones_validation() {
  // 有効な値
  for (int i = 0; i <= 9; i++) {
    TEST_ASSERT_TRUE(InputLogic::isValidMinOnes(i));
  }
  
  printf("✓ 分の一の位バリデーションテスト: 成功\n");
}

// 入力値から時刻への変換テスト
void test_input_to_time() {
  int hour, minute;
  
  // 12:30
  InputLogic::inputToTime(1, 2, 3, 0, hour, minute);
  TEST_ASSERT_EQUAL(12, hour);
  TEST_ASSERT_EQUAL(30, minute);
  
  // 00:00
  InputLogic::inputToTime(0, 0, 0, 0, hour, minute);
  TEST_ASSERT_EQUAL(0, hour);
  TEST_ASSERT_EQUAL(0, minute);
  
  // 23:59
  InputLogic::inputToTime(2, 3, 5, 9, hour, minute);
  TEST_ASSERT_EQUAL(23, hour);
  TEST_ASSERT_EQUAL(59, minute);
  
  printf("✓ 入力値から時刻への変換テスト: 成功\n");
}

// 時刻から入力値への変換テスト
void test_time_to_input() {
  int hour_tens, hour_ones, min_tens, min_ones;
  
  // 12:30
  InputLogic::timeToInput(12, 30, hour_tens, hour_ones, min_tens, min_ones);
  TEST_ASSERT_EQUAL(1, hour_tens);
  TEST_ASSERT_EQUAL(2, hour_ones);
  TEST_ASSERT_EQUAL(3, min_tens);
  TEST_ASSERT_EQUAL(0, min_ones);
  
  // 00:00
  InputLogic::timeToInput(0, 0, hour_tens, hour_ones, min_tens, min_ones);
  TEST_ASSERT_EQUAL(0, hour_tens);
  TEST_ASSERT_EQUAL(0, hour_ones);
  TEST_ASSERT_EQUAL(0, min_tens);
  TEST_ASSERT_EQUAL(0, min_ones);
  
  // 23:59
  InputLogic::timeToInput(23, 59, hour_tens, hour_ones, min_tens, min_ones);
  TEST_ASSERT_EQUAL(2, hour_tens);
  TEST_ASSERT_EQUAL(3, hour_ones);
  TEST_ASSERT_EQUAL(5, min_tens);
  TEST_ASSERT_EQUAL(9, min_ones);
  
  printf("✓ 時刻から入力値への変換テスト: 成功\n");
}

// 複合バリデーションテスト
void test_composite_validation() {
  // 有効な組み合わせ
  TEST_ASSERT_TRUE(InputLogic::isValidHourTens(1, 2) && InputLogic::isValidHourOnes(1, 2));
  TEST_ASSERT_TRUE(InputLogic::isValidHourTens(2, 3) && InputLogic::isValidHourOnes(2, 3));
  
  // 無効な組み合わせ
  TEST_ASSERT_FALSE(InputLogic::isValidHourTens(2, 4) && InputLogic::isValidHourOnes(2, 4));
  TEST_ASSERT_FALSE(InputLogic::isValidHourTens(3, 0) && InputLogic::isValidHourOnes(3, 0));
  
  printf("✓ 複合バリデーションテスト: 成功\n");
}

void setUp(void) {}
void tearDown(void) {}

// メイン関数
int main() {
  UNITY_BEGIN();
  
  printf("=== InputLogic Unit Test ===\n");
  
  RUN_TEST(test_digit_increment);
  RUN_TEST(test_digit_decrement);
  RUN_TEST(test_hour_tens_validation);
  RUN_TEST(test_hour_ones_validation);
  RUN_TEST(test_minute_tens_validation);
  RUN_TEST(test_minute_ones_validation);
  RUN_TEST(test_input_to_time);
  RUN_TEST(test_time_to_input);
  
  printf("=== 全テスト完了 ===\n");
  
  return UNITY_END();
} 