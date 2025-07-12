# 統一されたボタン管理システムと階層化されたデバウンス管理 実装計画

## 概要

現在のプロジェクトにおけるボタン管理の分散とデバウンス処理の重複を解決し、将来的な拡張性（AB同時押しなど）に対応できる統一されたシステムを実装する。

## 現状の問題点

### 1. ボタン管理の分散
- `main.cpp`と`input.cpp`でボタン処理が重複
- 各画面で個別にボタン判定ロジックを実装
- 長押し/短押し判定の基準が不統一

### 2. デバウンス処理の重複
- ハードウェアレベル、操作レベル、画面遷移レベルで重複
- デバウンス時間の管理が分散
- 保守性と拡張性の低下

### 3. 将来拡張への対応困難
- AB同時押し、ABC同時押しの実装が困難
- 新しいボタン操作パターンの追加が複雑

## 設計方針

### 1. 階層化されたデバウンス管理
```
┌─────────────────────────────────────┐
│ 画面遷移レベル (ModeManager)         │
│ - モード切り替えの安定性            │
│ - デバウンス時間: 300ms             │
├─────────────────────────────────────┤
│ 操作レベル (各画面)                  │
│ - 意図しない操作の防止              │
│ - デバウンス時間: 200ms             │
├─────────────────────────────────────┤
│ ハードウェアレベル (ButtonManager)   │
│ - 物理的なチャタリング対策          │
│ - デバウンス時間: 50ms              │
└─────────────────────────────────────┘
```

### 2. 統一されたボタン管理システム
```
┌─────────────────────────────────────┐
│ ButtonManager (一元管理)             │
│ ├─ 基本的な判定                      │
│ │  ├─ isShortPress()                │
│ │  ├─ isLongPress()                 │
│ │  └─ isReleased()                  │
│ ├─ 同時押し判定                      │
│ │  ├─ isSimultaneousPress()         │
│ │  ├─ isAllButtonsPressed()         │
│ │  └─ isCombinationPress()          │
│ └─ 拡張性のための基盤                │
│   ├─ ボタン状態の履歴管理            │
│   ├─ イベントシステム                │
│   └─ プラグイン可能な判定機能        │
└─────────────────────────────────────┘
```

## 実装計画

### Phase 1: 基盤システムの構築

#### 1.1 ButtonManagerクラスの作成
**ファイル**: `src/button_manager.h`, `src/button_manager.cpp`

```cpp
// 基本的な構造
class ButtonManager {
public:
  // 基本的な判定
  static bool isShortPress(Button& button, unsigned long threshold = 1000);
  static bool isLongPress(Button& button, unsigned long threshold = 1000);
  static bool isReleased(Button& button);
  
  // 同時押し判定
  static bool isSimultaneousPress(Button& button1, Button& button2, unsigned long tolerance = 100);
  static bool isAllButtonsPressed();
  static bool isCombinationPress(const std::vector<Button*>& buttons);
  
  // 状態管理
  static void updateButtonStates();
  static void resetButtonStates();
  
  // 拡張性のための基盤
  static ButtonState getButtonState(Button& button);
  static void addButtonEventListener(Button& button, ButtonEventType type, std::function<void()> callback);
  
private:
  static std::map<Button*, ButtonState> buttonStates;
  static std::vector<ButtonEvent> eventListeners;
  static void applyHardwareDebounce(ButtonState& state);
};

struct ButtonState {
  bool isPressed;
  bool wasPressed;
  bool wasReleased;
  unsigned long pressStartTime;
  unsigned long lastChangeTime;
  int pressCount;
  std::vector<unsigned long> pressHistory; // 将来の拡張用
};

struct ButtonEvent {
  Button* button;
  ButtonEventType type;
  std::function<void()> callback;
};

enum ButtonEventType {
  SHORT_PRESS,
  LONG_PRESS,
  RELEASE,
  SIMULTANEOUS_PRESS
};
```

#### 1.2 DebounceManagerクラスの作成
**ファイル**: `src/debounce_manager.h`, `src/debounce_manager.cpp`

```cpp
class DebounceManager {
public:
  // 階層別デバウンス判定
  static bool canProcessHardware(Button& button);
  static bool canProcessOperation(const String& operationType);
  static bool canProcessModeChange();
  
  // デバウンス時間の設定
  static void setHardwareDebounceTime(unsigned long time);
  static void setOperationDebounceTime(unsigned long time);
  static void setModeChangeDebounceTime(unsigned long time);
  
private:
  static const unsigned long DEFAULT_HARDWARE_DEBOUNCE = 50;
  static const unsigned long DEFAULT_OPERATION_DEBOUNCE = 200;
  static const unsigned long DEFAULT_MODE_CHANGE_DEBOUNCE = 300;
  
  static std::map<String, unsigned long> lastOperationTimes;
  static unsigned long lastModeChangeTime;
};
```

### Phase 2: 既存コードの段階的移行

#### 2.1 main.cppのリファクタリング
**目標**: ボタン処理の一元化

```cpp
// 変更前
void handleButtons() {
  // 各モードで個別にボタン処理
  if (M5.BtnA.wasPressed()) { /* 処理 */ }
  if (M5.BtnB.wasPressed()) { /* 処理 */ }
  if (M5.BtnC.wasPressed()) { /* 処理 */ }
}

// 変更後
void handleButtons() {
  ButtonManager::updateButtonStates();
  
  // デバウンスチェック
  if (!DebounceManager::canProcessModeChange()) return;
  
  switch (currentMode) {
    case MAIN_DISPLAY:
      handleMainDisplayButtons();
      break;
    case ABS_TIME_INPUT:
    case REL_PLUS_TIME_INPUT:
    case REL_MINUS_TIME_INPUT:
      handleInputModeButtons();
      break;
    // 他のモード...
  }
}
```

#### 2.2 input.cppのリファクタリング
**目標**: ButtonManagerの活用

```cpp
// 変更前
void handleDigitEditInput() {
  static uint32_t aPressStart = 0;
  static bool aLongPressFired = false;
  // 個別のボタン処理...
}

// 変更後
void handleDigitEditInput() {
  // 操作レベルのデバウンスチェック
  if (!DebounceManager::canProcessOperation("input_mode")) return;
  
  // ButtonManagerを使用した統一された処理
  if (ButtonManager::isShortPress(M5.BtnA)) {
    handleDigitIncrement();
  }
  if (ButtonManager::isLongPress(M5.BtnA, 500)) {
    handleDigitIncrementBy5();
  }
  // 他のボタン処理...
}
```

### Phase 3: 同時押し機能の実装

#### 3.1 基本的な同時押し機能
```cpp
// ButtonManagerに実装
class ButtonManager {
public:
  // 同時押し判定
  static bool isSimultaneousPress(Button& button1, Button& button2, unsigned long tolerance = 100);
  static bool isAllButtonsPressed();
  static bool isCombinationPress(const std::vector<Button*>& buttons);
  
  // 使用例
  if (ButtonManager::isSimultaneousPress(M5.BtnA, M5.BtnB)) {
    // AB同時押し処理
  }
  if (ButtonManager::isAllButtonsPressed()) {
    // ABC同時押し処理
  }
};
```

### Phase 4: 最適化とテスト

#### 4.1 パフォーマンス最適化
- メモリ使用量の最適化
- 処理速度の改善
- バッテリー消費の最適化

#### 4.2 テストと検証
- 各ボタン操作の動作確認
- デバウンス処理の効果検証
- 同時押し機能のテスト
- 実機での動作確認

## 実装スケジュール

### Week 1: Phase 1-2
- ButtonManagerクラスの基本実装
- DebounceManagerクラスの基本実装
- main.cppとinput.cppのリファクタリング
- 基本的なテスト

### Week 2: Phase 3-4
- 同時押し機能の実装
- 最適化とテスト
- ドキュメント更新
- 最終動作確認

## 将来の拡張性

### シーケンス操作への対応
```cpp
// 将来的な実装例（現在は実装しない）
class ButtonManager {
public:
  // シーケンス判定（将来実装）
  static bool isSequencePressed(const std::vector<Button*>& sequence, unsigned long timeout = 2000);
  static bool isPatternMatched(const String& pattern);
};
```

### 連続押し機能への対応
```cpp
// 将来的な実装例（現在は実装しない）
class ButtonManager {
public:
  // 連続押し判定（将来実装）
  static bool isDoublePress(Button& button, unsigned long interval = 300);
  static bool isTriplePress(Button& button, unsigned long interval = 300);
};
```

### プラグイン可能な判定機能
```cpp
// 将来的な拡張例（現在は実装しない）
class ButtonManager {
public:
  // カスタム判定機能の追加
  static void addCustomDetector(const String& name, std::function<bool()> detector);
  static bool isCustomDetected(const String& name);
};
```

## 期待される効果

### 1. 保守性の向上
- ボタン処理ロジックの一元化
- デバウンス処理の統一
- コードの重複削除

### 2. 拡張性の向上
- 新しいボタン操作パターンの簡単追加
- 同時押し機能への対応
- 将来的な機能拡張への対応

### 3. デバッグ性の向上
- ボタン操作の履歴管理
- デバウンス処理の可視化
- 問題の特定と解決の容易化

### 4. ユーザビリティの向上
- より直感的なボタン操作
- 意図しない操作の防止
- 一貫した操作感

## リスクと対策

### 1. 既存機能への影響
**リスク**: リファクタリングによる既存機能の破綻
**対策**: 段階的な移行と十分なテスト

### 2. パフォーマンスへの影響
**リスク**: 新しいシステムによる処理負荷の増加
**対策**: 最適化とプロファイリング

### 3. メモリ使用量の増加
**リスク**: 新しいクラスによるメモリ消費の増加
**対策**: メモリ効率の良い実装

## 次のステップ

1. **Phase 1の実装開始**
   - ButtonManagerクラスの基本構造の実装
   - DebounceManagerクラスの基本構造の実装
   - 基本的なテストの作成

2. **既存コードの分析**
   - 現在のボタン処理の詳細分析
   - 移行対象の特定
   - 移行順序の決定

3. **テスト計画の策定**
   - 各フェーズでのテスト項目の定義
   - 実機テストの計画
   - 回帰テストの計画 