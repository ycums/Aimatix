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
  TEST_ASSERT_TRUE(InputLogic::isValidHourTens(2, 1));
  TEST_ASSERT_TRUE(InputLogic::isValidHourTens(2, 2));
  TEST_ASSERT_TRUE(InputLogic::isValidHourTens(2, 3));
  
  // 無効な組み合わせ
  TEST_ASSERT_FALSE(InputLogic::isValidHourTens(2, 4));
  TEST_ASSERT_FALSE(InputLogic::isValidHourTens(2, 9));
  TEST_ASSERT_FALSE(InputLogic::isValidHourTens(3, 0));
  TEST_ASSERT_FALSE(InputLogic::isValidHourTens(9, 0));
  
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
  TEST_ASSERT_TRUE(InputLogic::isValidHourOnes(2, 1));
  TEST_ASSERT_TRUE(InputLogic::isValidHourOnes(2, 2));
  TEST_ASSERT_TRUE(InputLogic::isValidHourOnes(2, 3));
  
  // 無効な組み合わせ
  TEST_ASSERT_FALSE(InputLogic::isValidHourOnes(2, 4));
  TEST_ASSERT_FALSE(InputLogic::isValidHourOnes(2, 9));
  
  printf("✓ 時間の一の位バリデーションテスト: 成功\n");
}

// 分の十の位バリデーションテスト
void test_min_tens_validation() {
  // 有効な値
  TEST_ASSERT_TRUE(InputLogic::isValidMinTens(0));
  TEST_ASSERT_TRUE(InputLogic::isValidMinTens(1));
  TEST_ASSERT_TRUE(InputLogic::isValidMinTens(2));
  TEST_ASSERT_TRUE(InputLogic::isValidMinTens(3));
  TEST_ASSERT_TRUE(InputLogic::isValidMinTens(4));
  TEST_ASSERT_TRUE(InputLogic::isValidMinTens(5));
  
  // 無効な値
  TEST_ASSERT_FALSE(InputLogic::isValidMinTens(6));
  TEST_ASSERT_FALSE(InputLogic::isValidMinTens(9));
  
  printf("✓ 分の十の位バリデーションテスト: 成功\n");
}

// 分の一の位バリデーションテスト
void test_min_ones_validation() {
  // 0から9まで全て有効
  for (int i = 0; i <= 9; i++) {
    TEST_ASSERT_TRUE(InputLogic::isValidMinOnes(i));
  }
  
  printf("✓ 分の一の位バリデーションテスト: 成功\n");
}

// 入力値から時刻への変換テスト
void test_input_to_time_conversion() {
  int hour, minute;
  
  // 12:34のテスト
  InputLogic::inputToTime(1, 2, 3, 4, hour, minute);
  TEST_ASSERT_EQUAL(12, hour);
  TEST_ASSERT_EQUAL(34, minute);
  
  // 00:00のテスト
  InputLogic::inputToTime(0, 0, 0, 0, hour, minute);
  TEST_ASSERT_EQUAL(0, hour);
  TEST_ASSERT_EQUAL(0, minute);
  
  // 23:59のテスト
  InputLogic::inputToTime(2, 3, 5, 9, hour, minute);
  TEST_ASSERT_EQUAL(23, hour);
  TEST_ASSERT_EQUAL(59, minute);
  
  printf("✓ 入力値から時刻への変換テスト: 成功\n");
}

// 時刻から入力値への変換テスト
void test_time_to_input_conversion() {
  int hour_tens, hour_ones, min_tens, min_ones;
  
  // 12:34のテスト
  InputLogic::timeToInput(12, 34, hour_tens, hour_ones, min_tens, min_ones);
  TEST_ASSERT_EQUAL(1, hour_tens);
  TEST_ASSERT_EQUAL(2, hour_ones);
  TEST_ASSERT_EQUAL(3, min_tens);
  TEST_ASSERT_EQUAL(4, min_ones);
  
  // 00:00のテスト
  InputLogic::timeToInput(0, 0, hour_tens, hour_ones, min_tens, min_ones);
  TEST_ASSERT_EQUAL(0, hour_tens);
  TEST_ASSERT_EQUAL(0, hour_ones);
  TEST_ASSERT_EQUAL(0, min_tens);
  TEST_ASSERT_EQUAL(0, min_ones);
  
  // 23:59のテスト
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

void setUp(void) {
  // テスト前のセットアップ
}

void tearDown(void) {
  // テスト後のクリーンアップ
}

int main() {
  printf("=== InputLogicクラステスト開始 ===\n");
  
  UNITY_BEGIN();
  
  RUN_TEST(test_digit_increment);
  RUN_TEST(test_digit_decrement);
  RUN_TEST(test_hour_tens_validation);
  RUN_TEST(test_hour_ones_validation);
  RUN_TEST(test_min_tens_validation);
  RUN_TEST(test_min_ones_validation);
  RUN_TEST(test_input_to_time_conversion);
  RUN_TEST(test_time_to_input_conversion);
  RUN_TEST(test_composite_validation);
  
  UNITY_END();
  
  printf("=== InputLogicクラステスト完了 ===\n");
  printf("全テストが完了しました！\n");
  
  return 0;
} 