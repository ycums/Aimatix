#include <unity.h>
#include <cstdio>

// InputLogicクラスの純粋ロジックテスト
// M5Stack依存を排除し、標準C++のみでテスト

// 桁増加テスト
void test_digit_increment() {
  int digit = 0;
  
  // 0から9まで増加
  for (int i = 0; i < 10; i++) {
    TEST_ASSERT_EQUAL(i, digit);
    incrementDigit(digit, 9);
  }
  
  // 9から0に戻る
  TEST_ASSERT_EQUAL(0, digit);
  
  printf("✓ 桁増加テスト: 成功\n");
}

// 桁減少テスト
void test_digit_decrement() {
  int digit = 9;
  
  // 9から0まで減少
  for (int i = 9; i >= 0; i--) {
    TEST_ASSERT_EQUAL(i, digit);
    decrementDigit(digit, 9);
  }
  
  // 0から9に戻る
  TEST_ASSERT_EQUAL(9, digit);
  
  printf("✓ 桁減少テスト: 成功\n");
}

// 時十の位バリデーションテスト
void test_hour_tens_validation() {
  // 0-1の場合は常に有効
  TEST_ASSERT_TRUE(isValidHourTens(0, 0));
  TEST_ASSERT_TRUE(isValidHourTens(0, 9));
  TEST_ASSERT_TRUE(isValidHourTens(1, 0));
  TEST_ASSERT_TRUE(isValidHourTens(1, 9));
  
  // 2の場合は一の位が0-3のみ有効
  TEST_ASSERT_TRUE(isValidHourTens(2, 0));
  TEST_ASSERT_TRUE(isValidHourTens(2, 1));
  TEST_ASSERT_TRUE(isValidHourTens(2, 2));
  TEST_ASSERT_TRUE(isValidHourTens(2, 3));
  TEST_ASSERT_FALSE(isValidHourTens(2, 4));
  TEST_ASSERT_FALSE(isValidHourTens(2, 9));
  
  // 3以上は無効
  TEST_ASSERT_FALSE(isValidHourTens(3, 0));
  TEST_ASSERT_FALSE(isValidHourTens(9, 0));
  
  printf("✓ 時十の位バリデーションテスト: 成功\n");
}

// 時一の位バリデーションテスト
void test_hour_ones_validation() {
  // 十の位が0-1の場合、0-9が有効
  TEST_ASSERT_TRUE(isValidHourOnes(0, 0));
  TEST_ASSERT_TRUE(isValidHourOnes(0, 9));
  TEST_ASSERT_TRUE(isValidHourOnes(1, 0));
  TEST_ASSERT_TRUE(isValidHourOnes(1, 9));
  
  // 十の位が2の場合、0-3のみ有効
  TEST_ASSERT_TRUE(isValidHourOnes(2, 0));
  TEST_ASSERT_TRUE(isValidHourOnes(2, 1));
  TEST_ASSERT_TRUE(isValidHourOnes(2, 2));
  TEST_ASSERT_TRUE(isValidHourOnes(2, 3));
  TEST_ASSERT_FALSE(isValidHourOnes(2, 4));
  TEST_ASSERT_FALSE(isValidHourOnes(2, 9));
  
  printf("✓ 時一の位バリデーションテスト: 成功\n");
}

// 分十の位バリデーションテスト
void test_min_tens_validation() {
  // 0-5が有効
  TEST_ASSERT_TRUE(isValidMinTens(0));
  TEST_ASSERT_TRUE(isValidMinTens(1));
  TEST_ASSERT_TRUE(isValidMinTens(2));
  TEST_ASSERT_TRUE(isValidMinTens(3));
  TEST_ASSERT_TRUE(isValidMinTens(4));
  TEST_ASSERT_TRUE(isValidMinTens(5));
  
  // 6以上は無効
  TEST_ASSERT_FALSE(isValidMinTens(6));
  TEST_ASSERT_FALSE(isValidMinTens(9));
  
  printf("✓ 分十の位バリデーションテスト: 成功\n");
}

// 分一の位バリデーションテスト
void test_min_ones_validation() {
  // 0-9が有効
  for (int i = 0; i <= 9; i++) {
    TEST_ASSERT_TRUE(isValidMinOnes(i));
  }
  
  printf("✓ 分一の位バリデーションテスト: 成功\n");
}

// 入力値から時刻への変換テスト
void test_input_to_time_conversion() {
  int hour, minute;
  
  // 正常な変換
  inputToTime(1, 2, 3, 4, hour, minute);
  TEST_ASSERT_EQUAL(12, hour);
  TEST_ASSERT_EQUAL(34, minute);
  
  // 境界値テスト
  inputToTime(0, 0, 0, 0, hour, minute);
  TEST_ASSERT_EQUAL(0, hour);
  TEST_ASSERT_EQUAL(0, minute);
  
  inputToTime(2, 3, 5, 9, hour, minute);
  TEST_ASSERT_EQUAL(23, hour);
  TEST_ASSERT_EQUAL(59, minute);
  
  printf("✓ 入力値から時刻への変換テスト: 成功\n");
}

// 時刻から入力値への変換テスト
void test_time_to_input_conversion() {
  int hour_tens, hour_ones, min_tens, min_ones;
  
  // 正常な変換
  timeToInput(12, 34, hour_tens, hour_ones, min_tens, min_ones);
  TEST_ASSERT_EQUAL(1, hour_tens);
  TEST_ASSERT_EQUAL(2, hour_ones);
  TEST_ASSERT_EQUAL(3, min_tens);
  TEST_ASSERT_EQUAL(4, min_ones);
  
  // 境界値テスト
  timeToInput(0, 0, hour_tens, hour_ones, min_tens, min_ones);
  TEST_ASSERT_EQUAL(0, hour_tens);
  TEST_ASSERT_EQUAL(0, hour_ones);
  TEST_ASSERT_EQUAL(0, min_tens);
  TEST_ASSERT_EQUAL(0, min_ones);
  
  timeToInput(23, 59, hour_tens, hour_ones, min_tens, min_ones);
  TEST_ASSERT_EQUAL(2, hour_tens);
  TEST_ASSERT_EQUAL(3, hour_ones);
  TEST_ASSERT_EQUAL(5, min_tens);
  TEST_ASSERT_EQUAL(9, min_ones);
  
  printf("✓ 時刻から入力値への変換テスト: 成功\n");
}

// 複合バリデーションテスト
void test_composite_validation() {
  // 有効な時刻の組み合わせ
  TEST_ASSERT_TRUE(isValidHourTens(1, 2) && isValidHourOnes(1, 2));
  TEST_ASSERT_TRUE(isValidHourTens(2, 3) && isValidHourOnes(2, 3));
  
  // 無効な時刻の組み合わせ
  TEST_ASSERT_FALSE(isValidHourTens(2, 4) && isValidHourOnes(2, 4));
  TEST_ASSERT_FALSE(isValidHourTens(3, 0) && isValidHourOnes(3, 0));
  
  printf("✓ 複合バリデーションテスト: 成功\n");
}

// ヘルパー関数（InputLogicクラスの関数を模擬）
void incrementDigit(int& digit, int max_value, int increment) {
  digit = (digit + increment) % (max_value + 1);
}

void decrementDigit(int& digit, int max_value, int decrement) {
  digit = (digit - decrement + max_value + 1) % (max_value + 1);
}

bool isValidHourTens(int tens, int ones) {
  if (tens == 2) return ones <= 3;
  return tens <= 1;
}

bool isValidHourOnes(int tens, int ones) {
  if (tens == 2) return ones <= 3;
  return ones <= 9;
}

bool isValidMinTens(int tens) {
  return tens <= 5;
}

bool isValidMinOnes(int ones) {
  return ones <= 9;
}

void inputToTime(int hour_tens, int hour_ones, int min_tens, int min_ones, int& hour, int& minute) {
  hour = hour_tens * 10 + hour_ones;
  minute = min_tens * 10 + min_ones;
}

void timeToInput(int hour, int minute, int& hour_tens, int& hour_ones, int& min_tens, int& min_ones) {
  hour_tens = hour / 10;
  hour_ones = hour % 10;
  min_tens = minute / 10;
  min_ones = minute % 10;
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