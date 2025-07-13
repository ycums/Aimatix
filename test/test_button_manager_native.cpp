#include <unity.h>
#include "button_manager.h"
#include <M5Stack.h>

// テスト用のモックボタンクラス
class MockButton {
public:
  bool isPressed() const { return pressed; }
  bool wasPressed() const { return wasPressedFlag; }
  
  void simulatePress(unsigned long duration) {
    pressed = true;
    wasPressedFlag = true;
    pressTime = millis();
    this->duration = duration;
  }
  
  void simulateRelease() {
    pressed = false;
    wasPressedFlag = false;
  }
  
  void reset() {
    pressed = false;
    wasPressedFlag = false;
    pressTime = 0;
    duration = 0;
  }
  
private:
  bool pressed = false;
  bool wasPressedFlag = false;
  unsigned long pressTime = 0;
  unsigned long duration = 0;
};

// テスト用のグローバル変数
MockButton mockButtonA, mockButtonB, mockButtonC;

// テスト前のセットアップ
void setUp(void) {
  ButtonManager::initialize();
  mockButtonA.reset();
  mockButtonB.reset();
  mockButtonC.reset();
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
  // 短押しをシミュレート（500ms）
  mockButtonA.simulatePress(500);
  
  // ボタン状態を更新
  ButtonManager::updateButtonStates();
  
  // 短押し判定をテスト
  bool isShort = ButtonManager::isShortPress(M5.BtnA, 1000);
  
  // 500msは1000ms未満なので短押しとして判定されるべき
  TEST_ASSERT_TRUE(isShort);
}

// テスト3: 長押し判定
void test_long_press_detection() {
  // 長押しをシミュレート（1500ms）
  mockButtonA.simulatePress(1500);
  
  // ボタン状態を更新
  ButtonManager::updateButtonStates();
  
  // 長押し判定をテスト
  bool isLong = ButtonManager::isLongPress(M5.BtnA, 1000);
  
  // 1500msは1000ms以上なので長押しとして判定されるべき
  TEST_ASSERT_TRUE(isLong);
}

// テスト4: リリース判定
void test_release_detection() {
  // ボタンを押してからリリース
  mockButtonA.simulatePress(500);
  ButtonManager::updateButtonStates();
  
  mockButtonA.simulateRelease();
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
  mockButtonA.simulatePress(500);
  mockButtonB.simulatePress(1000);
  
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
  mockButtonA.simulatePress(500);
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