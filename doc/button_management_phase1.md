# Phase 1: ButtonManagerクラスの実装

## 概要

統一されたボタン管理システムの基盤となるButtonManagerクラスを実装する。既存のDebounceManagerを活用し、既存コードには一切影響を与えない形で実装する。

## 目的

- 統一されたボタン管理システムの基盤構築
- 既存のDebounceManagerとの連携
- 基本的なボタン判定機能の実装
- 将来拡張への基盤準備

## 成果物

- ButtonManagerクラス（`src/button_manager.h`, `src/button_manager.cpp`）
- 基本的なUnit Test
- 既存コードとの並行動作確認

## 実装内容

### Step 1.1: ButtonManagerクラスの基本実装

#### ファイル構成
```
src/
├── button_manager.h      # ヘッダーファイル
└── button_manager.cpp    # 実装ファイル
```

#### ヘッダーファイル（`src/button_manager.h`）
```cpp
#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include <M5Stack.h>
#include <map>
#include "debounce_manager.h"

// ボタン状態の構造体
struct ButtonState {
  bool isPressed;           // 現在押されているか
  bool wasPressed;          // 前回の更新で押されたか
  bool wasReleased;         // 前回の更新でリリースされたか
  unsigned long pressStartTime;  // 押下開始時刻
  unsigned long lastChangeTime;  // 最後の状態変化時刻
  int pressCount;           // 押下回数
  bool longPressHandled;    // 長押し処理済みフラグ
};

// ButtonManagerクラス（静的クラス）
class ButtonManager {
public:
  // 基本的な判定（既存のM5.BtnX.wasPressed()をラップ）
  static bool isShortPress(Button& button, unsigned long threshold = 1000);
  static bool isLongPress(Button& button, unsigned long threshold = 1000);
  static bool isReleased(Button& button);
  
  // 状態管理
  static void updateButtonStates();
  static void resetButtonStates();
  static ButtonState* getButtonState(Button& button);
  
  // デバウンス処理（DebounceManagerと連携）
  static bool canProcessButton(Button& button);
  
  // 初期化
  static void initialize();

private:
  // 静的メンバ変数
  static std::map<Button*, ButtonState> buttonStates;
  static unsigned long lastUpdateTime;
  
  // 内部処理関数
  static void updateButtonState(Button& button, unsigned long currentTime);
  static void applyHardwareDebounce(ButtonState& state);
  static ButtonState& getOrCreateButtonState(Button& button);
};

#endif // BUTTON_MANAGER_H
```

#### 実装ファイル（`src/button_manager.cpp`）
```cpp
#include "button_manager.h"

// 静的メンバ変数の定義
std::map<Button*, ButtonState> ButtonManager::buttonStates;
unsigned long ButtonManager::lastUpdateTime = 0;

// 初期化
void ButtonManager::initialize() {
  buttonStates.clear();
  lastUpdateTime = 0;
  
  // 初期状態を設定
  getOrCreateButtonState(M5.BtnA);
  getOrCreateButtonState(M5.BtnB);
  getOrCreateButtonState(M5.BtnC);
}

// ボタン状態の更新
void ButtonManager::updateButtonStates() {
  unsigned long currentTime = millis();
  
  // 各ボタンの状態を更新
  updateButtonState(M5.BtnA, currentTime);
  updateButtonState(M5.BtnB, currentTime);
  updateButtonState(M5.BtnC, currentTime);
  
  lastUpdateTime = currentTime;
}

// 個別ボタンの状態更新
void ButtonManager::updateButtonState(Button& button, unsigned long currentTime) {
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
  } else if (!state.isPressed && wasPressed) {
    // リリース
    state.wasReleased = true;
  } else {
    // 状態変化なし
    state.wasPressed = false;
    state.wasReleased = false;
  }
  
  state.lastChangeTime = currentTime;
  
  // ハードウェアデバウンス適用
  applyHardwareDebounce(state);
}

// 短押し判定
bool ButtonManager::isShortPress(Button& button, unsigned long threshold) {
  ButtonState* state = getButtonState(button);
  if (!state) return false;
  
  // リリースされた瞬間に短押しかどうかを判定
  if (state->wasReleased) {
    unsigned long pressDuration = millis() - state->pressStartTime;
    return pressDuration < threshold;
  }
  
  return false;
}

// 長押し判定
bool ButtonManager::isLongPress(Button& button, unsigned long threshold) {
  ButtonState* state = getButtonState(button);
  if (!state) return false;
  
  // 押されている状態で長押し時間を超えているかチェック
  if (state->isPressed && !state->longPressHandled) {
    unsigned long pressDuration = millis() - state->pressStartTime;
    if (pressDuration >= threshold) {
      state->longPressHandled = true;  // 重複処理を防ぐ
      return true;
    }
  }
  
  return false;
}

// リリース判定
bool ButtonManager::isReleased(Button& button) {
  ButtonState* state = getButtonState(button);
  if (!state) return false;
  
  return state->wasReleased;
}

// デバウンス処理（DebounceManagerと連携）
bool ButtonManager::canProcessButton(Button& button) {
  return DebounceManager::canProcessHardware(button);
}

// 状態リセット
void ButtonManager::resetButtonStates() {
  buttonStates.clear();
  lastUpdateTime = 0;
}

// ボタン状態の取得
ButtonState* ButtonManager::getButtonState(Button& button) {
  auto it = buttonStates.find(&button);
  if (it != buttonStates.end()) {
    return &(it->second);
  }
  return nullptr;
}

// ボタン状態の取得または作成
ButtonState& ButtonManager::getOrCreateButtonState(Button& button) {
  auto it = buttonStates.find(&button);
  if (it == buttonStates.end()) {
    // 新しい状態を作成
    ButtonState newState = {
      false,  // isPressed
      false,  // wasPressed
      false,  // wasReleased
      0,      // pressStartTime
      0,      // lastChangeTime
      0,      // pressCount
      false   // longPressHandled
    };
    buttonStates[&button] = newState;
    return buttonStates[&button];
  }
  return it->second;
}

// ハードウェアデバウンス適用
void ButtonManager::applyHardwareDebounce(ButtonState& state) {
  // 既存のDebounceManagerのハードウェアレベルデバウンスを適用
  // 実際の実装では、DebounceManagerとの連携を強化
}
```

### Step 1.2: 基本的なUnit Test

#### テストファイル（`test/test_button_manager.cpp`）
```cpp
#include <unity.h>
#include "button_manager.h"
#include "mocks/mock_button.h"

// テスト用のモックボタンクラス
class MockButton {
public:
  bool isPressed() const { return pressed; }
  bool wasPressed() const { return wasPressedFlag; }
  
  void simulatePress(unsigned long duration) {
    pressed = true;
    wasPressedFlag = true;
    pressTime = millis();
    duration = duration;
  }
  
  void simulateRelease() {
    pressed = false;
    wasPressedFlag = false;
  }
  
  void simulateChattering(unsigned long interval) {
    // チャタリングシミュレーション
    static unsigned long lastToggle = 0;
    if (millis() - lastToggle >= interval) {
      pressed = !pressed;
      lastToggle = millis();
    }
  }

private:
  bool pressed = false;
  bool wasPressedFlag = false;
  unsigned long pressTime = 0;
  unsigned long duration = 0;
};

// テスト関数
void test_button_manager_initialization() {
  ButtonManager::initialize();
  TEST_ASSERT_NOT_NULL(ButtonManager::getButtonState(M5.BtnA));
  TEST_ASSERT_NOT_NULL(ButtonManager::getButtonState(M5.BtnB));
  TEST_ASSERT_NOT_NULL(ButtonManager::getButtonState(M5.BtnC));
}

void test_short_press_detection() {
  ButtonManager::initialize();
  
  // 短押しシミュレーション
  MockButton button;
  button.simulatePress(100);  // 100ms押下
  
  ButtonManager::updateButtonStates();
  bool result = ButtonManager::isShortPress(button, 1000);
  
  TEST_ASSERT_TRUE(result);
}

void test_long_press_detection() {
  ButtonManager::initialize();
  
  // 長押しシミュレーション
  MockButton button;
  button.simulatePress(1500);  // 1.5秒押下
  
  ButtonManager::updateButtonStates();
  bool result = ButtonManager::isLongPress(button, 1000);
  
  TEST_ASSERT_TRUE(result);
}

void test_release_detection() {
  ButtonManager::initialize();
  
  MockButton button;
  button.simulatePress(100);
  button.simulateRelease();
  
  ButtonManager::updateButtonStates();
  bool result = ButtonManager::isReleased(button);
  
  TEST_ASSERT_TRUE(result);
}

void test_debounce_integration() {
  ButtonManager::initialize();
  
  // デバウンス処理の統合テスト
  bool result = ButtonManager::canProcessButton(M5.BtnA);
  TEST_ASSERT_TRUE(result);  // 初回は処理可能
}

// テスト実行
void RUN_UNITY_TESTS() {
  UNITY_BEGIN();
  
  RUN_TEST(test_button_manager_initialization);
  RUN_TEST(test_short_press_detection);
  RUN_TEST(test_long_press_detection);
  RUN_TEST(test_release_detection);
  RUN_TEST(test_debounce_integration);
  
  UNITY_END();
}

int main() {
  RUN_UNITY_TESTS();
  return 0;
}
```

## 動作確認項目

### コンパイル確認
- [ ] `src/button_manager.h`のコンパイル
- [ ] `src/button_manager.cpp`のコンパイル
- [ ] 既存コードとの統合コンパイル
- [ ] Unit Testのコンパイル

### 基本機能確認
- [ ] ButtonManager::initialize()の動作
- [ ] ButtonManager::updateButtonStates()の動作
- [ ] 短押し判定の動作
- [ ] 長押し判定の動作
- [ ] リリース判定の動作

### 既存コードとの整合性確認
- [ ] 既存のボタン処理に影響がないこと
- [ ] 既存のDebounceManagerとの連携
- [ ] メモリ使用量の確認

### Unit Test確認
- [ ] 全テストケースの実行
- [ ] テスト結果の確認
- [ ] カバレッジの確認

## 次のステップ

Phase 1が完了したら、以下のステップに進みます：

1. **Phase 2**: 既存コードにButtonManagerを並行導入
2. **統合テスト**: 既存機能との並行動作確認
3. **パフォーマンステスト**: メモリ使用量・処理速度の確認

## 注意事項

- 既存コードには一切変更を加えない
- コンパイルエラーが発生した場合は即座に修正
- 各ステップで動作確認を必ず行う
- 問題が発生した場合はロールバック可能な状態を維持

## 成功基準

- [ ] コンパイルエラー0件
- [ ] 既存機能の動作確認100%
- [ ] Unit Testの全テストケース通過
- [ ] メモリ使用量の増加が20%以下
- [ ] 既存のDebounceManagerとの正常な連携 