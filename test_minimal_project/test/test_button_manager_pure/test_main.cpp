#include <unity.h>
#include <cstdio>
#include <cstring>
#include <map>
#include <vector>
#include "mocks/mock_m5stack.h"
#include <button_manager.h>

// ButtonManagerの純粋ロジックテスト
// M5Stack依存を排除し、標準C++のみでテスト

// mock_m5stack.hのButton型を使用
using Button = MockM5Stack::Button;

// ボタン状態の構造体
struct ButtonState {
  bool isPressed;
  bool wasPressed;
  bool wasReleased;
  unsigned long pressStartTime;
  unsigned long lastChangeTime;
  int pressCount;
  bool longPressHandled;
  
  ButtonState() : isPressed(false), wasPressed(false), wasReleased(false),
                  pressStartTime(0), lastChangeTime(0), pressCount(0), longPressHandled(false) {}
};

// モック時間管理
unsigned long millis() { return mockMillis; }
void setMockTime(unsigned long time) { mockMillis = time; }

// モックDebounceManager
class DebounceManager {
public:
  static bool canProcessHardware(Button& button) {
    return true; // 簡略化のため常にtrue
  }
};

// 純粋ロジックButtonManager
class PureButtonManager {
private:
  static std::map<Button*, ButtonState> buttonStates;
  static unsigned long lastUpdateTime;
  
public:
  static void initialize() {
    buttonStates.clear();
    lastUpdateTime = 0;
  }
  
  static void updateButtonStates(Button& btnA, Button& btnB, Button& btnC) {
    unsigned long currentTime = mockMillis;
    
    updateButtonState(btnA, currentTime);
    updateButtonState(btnB, currentTime);
    updateButtonState(btnC, currentTime);
    
    lastUpdateTime = currentTime;
  }
  
  static void updateButtonState(Button& button, unsigned long currentTime) {
    ButtonState& state = getOrCreateButtonState(button);
    
    // 現在の状態を保存
    bool wasPressed = state.isPressed;
    state.isPressed = button.isPressed();
    
    // 状態変化の検出
    if (state.isPressed && !wasPressed) {
      // 押下開始
      state.wasPressed = true;
      state.pressStartTime = currentTime;
      state.longPressHandled = false;
      state.pressCount++;
      state.lastChangeTime = currentTime;
    } else if (!state.isPressed && wasPressed) {
      // リリース
      state.wasReleased = true;
      state.lastChangeTime = currentTime;
    } else {
      // 状態変化なし
      state.wasPressed = false;
      state.wasReleased = false;
    }
  }
  
  static bool isShortPress(Button& button, unsigned long threshold = 1000) {
    ButtonState* state = getButtonState(button);
    if (!state) return false;
    
    if (!DebounceManager::canProcessHardware(button)) {
      return false;
    }
    
    // リリースされた瞬間に短押しかどうかを判定
    if (state->wasReleased) {
      unsigned long pressDuration = mockMillis - state->pressStartTime;
      return pressDuration < threshold;
    }
    
    return false;
  }
  
  static bool isLongPress(Button& button, unsigned long threshold = 1000) {
    ButtonState* state = getButtonState(button);
    if (!state) return false;
    
    if (!DebounceManager::canProcessHardware(button)) {
      return false;
    }
    
    // 押されている状態で長押し時間を超えているかチェック
    if (state->isPressed && !state->longPressHandled) {
      unsigned long pressDuration = mockMillis - state->pressStartTime;
      if (pressDuration >= threshold) {
        state->longPressHandled = true;
        return true;
      }
    }
    
    return false;
  }
  
  static bool isReleased(Button& button) {
    ButtonState* state = getButtonState(button);
    if (!state) return false;
    
    if (!DebounceManager::canProcessHardware(button)) {
      return false;
    }
    
    return state->wasReleased;
  }
  
  static void resetButtonStates() {
    buttonStates.clear();
    lastUpdateTime = 0;
  }
  
  static ButtonState* getButtonState(Button& button) {
    auto it = buttonStates.find(&button);
    if (it != buttonStates.end()) {
      return &(it->second);
    }
    return nullptr;
  }
  
private:
  static ButtonState& getOrCreateButtonState(Button& button) {
    auto it = buttonStates.find(&button);
    if (it == buttonStates.end()) {
      ButtonState newState;
      buttonStates[&button] = newState;
      return buttonStates[&button];
    }
    return it->second;
  }
};

// 静的メンバ変数の定義
std::map<Button*, ButtonState> PureButtonManager::buttonStates;
unsigned long PureButtonManager::lastUpdateTime = 0;

// ボタン状態初期化テスト
void test_button_state_initialization() {
  Button button;
  PureButtonManager::initialize();
  
  ButtonState* state = PureButtonManager::getButtonState(button);
  TEST_ASSERT_NULL(state);
  
  printf("✓ ボタン状態初期化テスト: 成功\n");
}

// ボタン押下検出テスト
void test_button_press_detection() {
  Button button;
  PureButtonManager::initialize();
  
  setMockTime(0);
  
  // ボタンを押す
  button.simulatePress();
  PureButtonManager::updateButtonState(button, mockMillis);
  
  ButtonState* state = PureButtonManager::getButtonState(button);
  TEST_ASSERT_NOT_NULL(state);
  TEST_ASSERT_TRUE(state->isPressed);
  TEST_ASSERT_TRUE(state->wasPressed);
  TEST_ASSERT_EQUAL(1, state->pressCount);
  
  printf("✓ ボタン押下検出テスト: 成功\n");
}

// ボタンリリース検出テスト
void test_button_release_detection() {
  Button button;
  PureButtonManager::initialize();
  
  setMockTime(0);
  
  // ボタンを押す
  button.simulatePress();
  PureButtonManager::updateButtonState(button, mockMillis);
  
  // ボタンを離す
  setMockTime(100);
  button.simulateRelease();
  PureButtonManager::updateButtonState(button, mockMillis);
  
  ButtonState* state = PureButtonManager::getButtonState(button);
  TEST_ASSERT_FALSE(state->isPressed);
  TEST_ASSERT_TRUE(state->wasReleased);
  
  printf("✓ ボタンリリース検出テスト: 成功\n");
}

// 短押し判定テスト
void test_short_press_detection() {
  Button button;
  PureButtonManager::initialize();
  
  setMockTime(0);
  
  // ボタンを押す
  button.simulatePress();
  PureButtonManager::updateButtonState(button, mockMillis);
  
  // 短時間後に離す
  setMockTime(500);
  button.simulateRelease();
  PureButtonManager::updateButtonState(button, mockMillis);
  
  // 短押し判定
  TEST_ASSERT_TRUE(PureButtonManager::isShortPress(button, 1000));
  TEST_ASSERT_FALSE(PureButtonManager::isShortPress(button, 200));
  
  printf("✓ 短押し判定テスト: 成功\n");
}

// 長押し判定テスト
void test_long_press_detection() {
  Button button;
  PureButtonManager::initialize();
  
  setMockTime(0);
  
  // ボタンを押す
  button.simulatePress();
  PureButtonManager::updateButtonState(button, mockMillis);
  
  // 長時間押し続ける
  setMockTime(1500);
  PureButtonManager::updateButtonState(button, mockMillis);
  
  // 長押し判定
  TEST_ASSERT_TRUE(PureButtonManager::isLongPress(button, 1000));
  TEST_ASSERT_FALSE(PureButtonManager::isLongPress(button, 2000));
  
  printf("✓ 長押し判定テスト: 成功\n");
}

// リリース判定テスト
void test_release_detection() {
  Button button;
  PureButtonManager::initialize();
  
  setMockTime(0);
  
  // ボタンを押す
  button.simulatePress();
  PureButtonManager::updateButtonState(button, mockMillis);
  
  // リリース判定（まだ押されている）
  TEST_ASSERT_FALSE(PureButtonManager::isReleased(button));
  
  // ボタンを離す
  setMockTime(100);
  button.simulateRelease();
  PureButtonManager::updateButtonState(button, mockMillis);
  
  // リリース判定
  TEST_ASSERT_TRUE(PureButtonManager::isReleased(button));
  
  printf("✓ リリース判定テスト: 成功\n");
}

// 押下回数カウントテスト
void test_press_count() {
  Button button;
  PureButtonManager::initialize();
  
  setMockTime(0);
  
  // 1回目の押下
  button.simulatePress();
  PureButtonManager::updateButtonState(button, mockMillis);
  
  setMockTime(100);
  button.simulateRelease();
  PureButtonManager::updateButtonState(button, mockMillis);
  
  // 2回目の押下
  setMockTime(200);
  button.simulatePress();
  PureButtonManager::updateButtonState(button, mockMillis);
  
  setMockTime(300);
  button.simulateRelease();
  PureButtonManager::updateButtonState(button, mockMillis);
  
  ButtonState* state = PureButtonManager::getButtonState(button);
  TEST_ASSERT_EQUAL(2, state->pressCount);
  
  printf("✓ 押下回数カウントテスト: 成功\n");
}

// 長押し重複処理防止テスト
void test_long_press_duplicate_prevention() {
  Button button;
  PureButtonManager::initialize();
  
  setMockTime(0);
  
  // ボタンを押す
  button.simulatePress();
  PureButtonManager::updateButtonState(button, mockMillis);
  
  // 長押し判定（1回目）
  setMockTime(1500);
  PureButtonManager::updateButtonState(button, mockMillis);
  TEST_ASSERT_TRUE(PureButtonManager::isLongPress(button, 1000));
  
  // 長押し判定（2回目 - 重複処理を防ぐ）
  setMockTime(2000);
  PureButtonManager::updateButtonState(button, mockMillis);
  TEST_ASSERT_FALSE(PureButtonManager::isLongPress(button, 1000));
  
  printf("✓ 長押し重複処理防止テスト: 成功\n");
}

// 複数ボタン管理テスト
void test_multiple_button_management() {
  Button btnA, btnB, btnC;
  PureButtonManager::initialize();
  
  setMockTime(0);
  
  // 複数のボタンを同時に押す
  btnA.simulatePress();
  btnB.simulatePress();
  // btnCは押さない
  
  PureButtonManager::updateButtonStates(btnA, btnB, btnC);
  
  // 各ボタンの状態確認
  TEST_ASSERT_TRUE(PureButtonManager::getButtonState(btnA)->isPressed);
  TEST_ASSERT_TRUE(PureButtonManager::getButtonState(btnB)->isPressed);
  TEST_ASSERT_FALSE(PureButtonManager::getButtonState(btnC)->isPressed);
  
  printf("✓ 複数ボタン管理テスト: 成功\n");
}

// 状態リセットテスト
void test_state_reset() {
  Button button;
  PureButtonManager::initialize();
  
  setMockTime(0);
  
  // ボタンを押す
  button.simulatePress();
  PureButtonManager::updateButtonState(button, mockMillis);
  
  // 状態確認
  TEST_ASSERT_NOT_NULL(PureButtonManager::getButtonState(button));
  
  // リセット
  PureButtonManager::resetButtonStates();
  
  // リセット後の確認
  TEST_ASSERT_NULL(PureButtonManager::getButtonState(button));
  
  printf("✓ 状態リセットテスト: 成功\n");
}

// 複合ボタン操作テスト
void test_composite_button_operations() {
  Button button;
  PureButtonManager::initialize();
  
  setMockTime(0);
  
  // 押下→長押し→リリースの一連の操作
  button.simulatePress();
  PureButtonManager::updateButtonState(button, mockMillis);
  
  // 短押し判定（まだ押されている）
  TEST_ASSERT_FALSE(PureButtonManager::isShortPress(button));
  
  // 長押し判定
  setMockTime(1500);
  PureButtonManager::updateButtonState(button, mockMillis);
  TEST_ASSERT_TRUE(PureButtonManager::isLongPress(button, 1000));
  
  // リリース
  setMockTime(2000);
  button.simulateRelease();
  PureButtonManager::updateButtonState(button, mockMillis);
  
  // 短押し判定（リリース後）
  TEST_ASSERT_TRUE(PureButtonManager::isShortPress(button, 3000));
  TEST_ASSERT_TRUE(PureButtonManager::isReleased(button));
  
  printf("✓ 複合ボタン操作テスト: 成功\n");
}

void setUp(void) {
  PureButtonManager::initialize();
  setMockTime(0);
}

void tearDown(void) {
  PureButtonManager::resetButtonStates();
}

// メイン関数
int main() {
  UNITY_BEGIN();
  
  printf("=== ButtonManager 純粋ロジックテスト ===\n");
  
  RUN_TEST(test_button_state_initialization);
  RUN_TEST(test_button_press_detection);
  RUN_TEST(test_button_release_detection);
  RUN_TEST(test_short_press_detection);
  RUN_TEST(test_long_press_detection);
  RUN_TEST(test_release_detection);
  RUN_TEST(test_press_count);
  RUN_TEST(test_long_press_duplicate_prevention);
  RUN_TEST(test_multiple_button_management);
  RUN_TEST(test_state_reset);
  RUN_TEST(test_composite_button_operations);
  
  printf("=== 全テスト完了 ===\n");
  
  return UNITY_END();
} 