#include <unity.h>
#include "button_manager.h"
#include "mocks/mock_m5stack.h"

// mock_m5stack.hのButton型を使用
using Button = MockM5Stack::Button;

// テスト前のセットアップ
void setUp(void) {
  ButtonManager::initialize();
  M5.BtnA.reset();
  M5.BtnB.reset();
  M5.BtnC.reset();
}

// テスト後のクリーンアップ
void tearDown(void) {
  ButtonManager::resetButtonStates();
}

// テスト1: ButtonManagerの初期化
void test_button_manager_initialization() {
  ButtonManager::initialize();
  
  // 初期化後、ボタン状態が正しく設定されているかチェック
  ButtonState* stateA = ButtonManager::getButtonState(M5.BtnA);
  ButtonState* stateB = ButtonManager::getButtonState(M5.BtnB);
  ButtonState* stateC = ButtonManager::getButtonState(M5.BtnC);
  
  TEST_ASSERT_NOT_NULL(stateA);
  TEST_ASSERT_NOT_NULL(stateB);
  TEST_ASSERT_NOT_NULL(stateC);
  
  TEST_ASSERT_FALSE(stateA->isPressed);
  TEST_ASSERT_FALSE(stateB->isPressed);
  TEST_ASSERT_FALSE(stateC->isPressed);
}

// テスト2: 短押し判定
void test_short_press_detection() {
  // 短押しをシミュレート
  M5.BtnA.simulatePress();
  
  // ボタン状態を更新
  ButtonManager::updateButtonStates();
  
  // 短押し判定をテスト
  bool isShort = ButtonManager::isShortPress(M5.BtnA, 1000);
  
  // 短押しとして判定されるべき
  TEST_ASSERT_TRUE(isShort);
}

// テスト3: 長押し判定
void test_long_press_detection() {
  // 長押しをシミュレート
  M5.BtnA.simulatePress();
  
  // ボタン状態を更新
  ButtonManager::updateButtonStates();
  
  // 長押し判定をテスト
  bool isLong = ButtonManager::isLongPress(M5.BtnA, 1000);
  
  // 長押しとして判定されるべき
  TEST_ASSERT_TRUE(isLong);
}

// テスト4: リリース判定
void test_release_detection() {
  // ボタンを押してからリリース
  M5.BtnA.simulatePress();
  ButtonManager::updateButtonStates();
  
  M5.BtnA.simulateRelease();
  ButtonManager::updateButtonStates();
  
  // リリース判定をテスト
  bool isReleased = ButtonManager::isReleased(M5.BtnA);
  
  TEST_ASSERT_TRUE(isReleased);
}

// テスト5: デバウンス処理
void test_debounce_processing() {
  // デバウンス処理が正しく動作するかテスト
  bool canProcess = ButtonManager::canProcessButton(M5.BtnA);
  
  // 初回は処理可能であるべき
  TEST_ASSERT_TRUE(canProcess);
}

// テスト6: 状態管理
void test_state_management() {
  // ボタン状態の取得と管理をテスト
  ButtonState* state = ButtonManager::getButtonState(M5.BtnA);
  
  TEST_ASSERT_NOT_NULL(state);
  TEST_ASSERT_FALSE(state->isPressed);
  TEST_ASSERT_EQUAL(0, state->pressCount);
}

// テスト7: 複数ボタンの同時管理
void test_multiple_button_management() {
  // 複数のボタンを同時に管理できるかテスト
  M5.BtnA.simulatePress();
  M5.BtnB.simulatePress();
  
  ButtonManager::updateButtonStates();
  
  ButtonState* stateA = ButtonManager::getButtonState(M5.BtnA);
  ButtonState* stateB = ButtonManager::getButtonState(M5.BtnB);
  
  TEST_ASSERT_TRUE(stateA->isPressed);
  TEST_ASSERT_TRUE(stateB->isPressed);
  TEST_ASSERT_EQUAL(1, stateA->pressCount);
  TEST_ASSERT_EQUAL(1, stateB->pressCount);
}

// テスト8: 状態リセット
void test_state_reset() {
  // 状態リセットが正しく動作するかテスト
  M5.BtnA.simulatePress();
  ButtonManager::updateButtonStates();
  
  ButtonManager::resetButtonStates();
  
  ButtonState* state = ButtonManager::getButtonState(M5.BtnA);
  TEST_ASSERT_NULL(state);  // リセット後は状態がクリアされる
}

// Arduinoフレームワーク用のsetup関数
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  UNITY_BEGIN();
  
  // テストケースの実行
  RUN_TEST(test_button_manager_initialization);
  RUN_TEST(test_short_press_detection);
  RUN_TEST(test_long_press_detection);
  RUN_TEST(test_release_detection);
  RUN_TEST(test_debounce_processing);
  RUN_TEST(test_state_management);
  RUN_TEST(test_multiple_button_management);
  RUN_TEST(test_state_reset);
  
  UNITY_END();
  
  Serial.println("All tests completed!");
}

// Arduinoフレームワーク用のloop関数
void loop() {
  // テスト完了後は何もしない
  delay(1000);
} 