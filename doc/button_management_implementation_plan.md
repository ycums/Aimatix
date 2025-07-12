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

## 段階的実装戦略

### 段階的実装の原則
1. **各ステップでコンパイルが通る状態を維持**
2. **既存機能を壊さずに段階的に移行**
3. **動作確認しながら進める**
4. **必要に応じてロールバック可能**

### 実装順序
```
Step 1: 最小限のButtonManager（既存コードは変更なし）
Step 2: 既存コードにButtonManagerを並行導入
Step 3: 段階的に既存コードをButtonManagerに移行
Step 4: 完全移行と最適化
```

## 実装計画

### Phase 1: 基盤システムの構築（コンパイル通る状態を維持）

#### Step 1.1: 最小限のButtonManagerクラス
**ファイル**: `src/button_manager.h`, `src/button_manager.cpp`

```cpp
// Step 1.1: 最小限の実装（既存コードは一切変更しない）
class ButtonManager {
public:
  // 基本的な判定（既存のM5.BtnX.wasPressed()をラップ）
  static bool isShortPress(Button& button, unsigned long threshold = 1000);
  static bool isLongPress(Button& button, unsigned long threshold = 1000);
  static bool isReleased(Button& button);
  
  // 状態管理
  static void updateButtonStates();
  static void resetButtonStates();
  
private:
  static std::map<Button*, ButtonState> buttonStates;
  static void applyHardwareDebounce(ButtonState& state);
};

struct ButtonState {
  bool isPressed;
  bool wasPressed;
  bool wasReleased;
  unsigned long pressStartTime;
  unsigned long lastChangeTime;
  int pressCount;
};
```

**このステップでの動作確認**:
- コンパイルが通ることを確認
- 既存のボタン処理は一切変更されない
- ButtonManagerは並行して動作するだけ

#### Step 1.2: 最小限のDebounceManagerクラス
**ファイル**: `src/debounce_manager.h`, `src/debounce_manager.cpp`

```cpp
// Step 1.2: 最小限の実装（既存コードは一切変更しない）
class DebounceManager {
public:
  // 階層別デバウンス判定
  static bool canProcessHardware(Button& button);
  static bool canProcessOperation(const String& operationType);
  static bool canProcessModeChange();
  
private:
  static const unsigned long DEFAULT_HARDWARE_DEBOUNCE = 50;
  static const unsigned long DEFAULT_OPERATION_DEBOUNCE = 200;
  static const unsigned long DEFAULT_MODE_CHANGE_DEBOUNCE = 300;
  
  static std::map<String, unsigned long> lastOperationTimes;
  static unsigned long lastModeChangeTime;
};
```

**このステップでの動作確認**:
- コンパイルが通ることを確認
- 既存のデバウンス処理は一切変更されない
- DebounceManagerは並行して動作するだけ

### Phase 2: 並行導入（既存コードを壊さずに段階的移行）

#### Step 2.1: main.cppにButtonManagerを並行導入
```cpp
// Step 2.1: 既存のhandleButtons()はそのまま、ButtonManagerを並行導入
void handleButtons() {
  // 既存のコードはそのまま
  static unsigned long lastPress = 0;
  static unsigned long lastModeChange = 0;
  const unsigned long LONG_PRESS_TIME = 1000;
  const unsigned long DEBOUNCE_TIME = 200;
  static bool cLongPressHandled = false;
  
  // 既存のデバウンス処理
  if (millis() - lastModeChange < DEBOUNCE_TIME) {
    return;
  }
  
  // 既存のCボタン処理（そのまま）
  if (currentMode != ABS_TIME_INPUT && currentMode != REL_PLUS_TIME_INPUT && currentMode != REL_MINUS_TIME_INPUT) {
    if (M5.BtnC.wasPressed()) {
      lastPress = millis();
      cLongPressHandled = false;
      Serial.println("Main: C button pressed - common handler");
    }
    if (M5.BtnC.pressedFor(LONG_PRESS_TIME)) {
      if (!cLongPressHandled && currentMode != MAIN_DISPLAY) {
        Serial.println("Main: C button long press - returning to main");
        currentMode = MAIN_DISPLAY;
        cLongPressHandled = true;
        lastModeChange = millis();
        return;
      }
    }
  }
  
  // 既存のswitch文（そのまま）
  switch (currentMode) {
    case MAIN_DISPLAY:
      // 既存の処理
      break;
    case ABS_TIME_INPUT:
    case REL_PLUS_TIME_INPUT:
    case REL_MINUS_TIME_INPUT:
      handleDigitEditInput();
      drawInputMode();
      if (currentMode == MAIN_DISPLAY) {
        lastModeChange = millis();
      }
      break;
    // 他のケース（そのまま）
  }
  
  // 新しく追加: ButtonManagerの状態更新（既存処理には影響なし）
  ButtonManager::updateButtonStates();
}
```

**このステップでの動作確認**:
- 既存のボタン処理が正常に動作することを確認
- ButtonManagerが並行して動作することを確認
- コンパイルが通ることを確認

#### Step 2.2: input.cppにButtonManagerを並行導入
```cpp
// Step 2.2: 既存のhandleDigitEditInput()はそのまま、ButtonManagerを並行導入
void handleDigitEditInput() {
  // 既存のコードはそのまま
  static uint32_t aPressStart = 0;
  static bool aLongPressFired = false;
  static uint32_t bPressStart = 0;
  static bool bLongPressFired = false;
  static uint32_t cPressStart = 0;
  static bool cLongPressFired = false;

  // 既存のAボタン処理（そのまま）
  if (M5.BtnA.wasPressed()) {
    aPressStart = millis();
    aLongPressFired = false;
  }
  // ... 既存のAボタン処理（そのまま）

  // 既存のBボタン処理（そのまま）
  if (M5.BtnB.wasPressed()) {
    bPressStart = millis();
    bLongPressFired = false;
  }
  // ... 既存のBボタン処理（そのまま）

  // 既存のCボタン処理（そのまま）
  if (M5.BtnC.wasPressed()) {
    cPressStart = millis();
    cLongPressFired = false;
  }
  // ... 既存のCボタン処理（そのまま）
  
  // 新しく追加: ButtonManagerの状態更新（既存処理には影響なし）
  ButtonManager::updateButtonStates();
}
```

**このステップでの動作確認**:
- 既存の入力処理が正常に動作することを確認
- ButtonManagerが並行して動作することを確認
- コンパイルが通ることを確認

### Phase 3: 段階的移行（既存コードを少しずつ置き換え）

#### Step 3.1: main.cppの一部をButtonManagerに移行
```cpp
// Step 3.1: 一部のボタン処理をButtonManagerに移行（段階的）
void handleButtons() {
  ButtonManager::updateButtonStates();
  
  // デバウンスチェック（既存のロジックをDebounceManagerに移行）
  if (!DebounceManager::canProcessModeChange()) return;
  
  // 既存のCボタン処理はそのまま（後で移行）
  static unsigned long lastPress = 0;
  static bool cLongPressHandled = false;
  
  if (currentMode != ABS_TIME_INPUT && currentMode != REL_PLUS_TIME_INPUT && currentMode != REL_MINUS_TIME_INPUT) {
    if (M5.BtnC.wasPressed()) {
      lastPress = millis();
      cLongPressHandled = false;
    }
    if (M5.BtnC.pressedFor(1000)) {
      if (!cLongPressHandled && currentMode != MAIN_DISPLAY) {
        currentMode = MAIN_DISPLAY;
        cLongPressHandled = true;
        return;
      }
    }
  }
  
  switch (currentMode) {
    case MAIN_DISPLAY:
      // 既存の処理（そのまま）
      if (M5.BtnA.wasPressed()) {
        currentMode = ABS_TIME_INPUT;
        resetInput();
      }
      // ... 他の既存処理
      break;
    case ABS_TIME_INPUT:
    case REL_PLUS_TIME_INPUT:
    case REL_MINUS_TIME_INPUT:
      handleDigitEditInput();
      drawInputMode();
      break;
    // 他のケース（そのまま）
  }
}
```

**このステップでの動作確認**:
- 既存のボタン処理が正常に動作することを確認
- DebounceManagerが正常に動作することを確認
- コンパイルが通ることを確認

#### Step 3.2: input.cppの一部をButtonManagerに移行
```cpp
// Step 3.2: 一部のボタン処理をButtonManagerに移行（段階的）
void handleDigitEditInput() {
  // 操作レベルのデバウンスチェック
  if (!DebounceManager::canProcessOperation("input_mode")) return;
  
  // 既存のAボタン処理はそのまま（後で移行）
  static uint32_t aPressStart = 0;
  static bool aLongPressFired = false;
  
  if (M5.BtnA.wasPressed()) {
    aPressStart = millis();
    aLongPressFired = false;
  }
  // ... 既存のAボタン処理（そのまま）
  
  // 既存のBボタン処理はそのまま（後で移行）
  static uint32_t bPressStart = 0;
  static bool bLongPressFired = false;
  
  if (M5.BtnB.wasPressed()) {
    bPressStart = millis();
    bLongPressFired = false;
  }
  // ... 既存のBボタン処理（そのまま）
  
  // Cボタン処理をButtonManagerに移行（テスト）
  if (ButtonManager::isShortPress(M5.BtnC)) {
    // 短押し: セット（確定）
    int hour = digitEditInput.hourTens * 10 + digitEditInput.hourOnes;
    int min = digitEditInput.minTens * 10 + digitEditInput.minOnes;
    
    // 時刻バリデーション
    if (hour > 23 || min > 59) {
      return;
    }
    
    // アラーム時刻の計算
    time_t now = time(NULL);
    time_t alarmTime = 0;
    
    if (currentMode == ABS_TIME_INPUT) {
      struct tm tminfo;
      localtime_r(&now, &tminfo);
      tminfo.tm_hour = hour;
      tminfo.tm_min = min;
      tminfo.tm_sec = 0;
      alarmTime = mktime(&tminfo);
      
      if (alarmTime <= now) {
        alarmTime += 24 * 3600;
      }
    } else if (currentMode == REL_PLUS_TIME_INPUT) {
      alarmTime = now + (hour * 3600) + (min * 60);
    }
    
    // 重複チェック
    if (std::find(alarmTimes.begin(), alarmTimes.end(), alarmTime) != alarmTimes.end()) {
      return;
    }
    
    // 最大数チェック
    if (alarmTimes.size() >= 5) {
      return;
    }
    
    // アラーム追加
    alarmTimes.push_back(alarmTime);
    std::sort(alarmTimes.begin(), alarmTimes.end());
    
    // 入力状態リセット
    resetInput();
    
    // メイン画面に戻る
    currentMode = MAIN_DISPLAY;
  }
  
  if (ButtonManager::isLongPress(M5.BtnC, 1000)) {
    // 長押し: メイン画面に戻る
    currentMode = MAIN_DISPLAY;
  }
}
```

**このステップでの動作確認**:
- Cボタンの短押し/長押し判定が正常に動作することを確認
- 既存のA/Bボタン処理が正常に動作することを確認
- コンパイルが通ることを確認

### Phase 4: 完全移行と最適化

#### Step 4.1: 残りのボタン処理を完全移行
```cpp
// Step 4.1: 全てのボタン処理をButtonManagerに移行
void handleButtons() {
  ButtonManager::updateButtonStates();
  
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
    case ALARM_MANAGEMENT:
      handleAlarmManagementButtons();
      break;
    case SETTINGS_MENU:
      handleSettingsMenuButtons();
      break;
    // 他のモード...
  }
}

void handleMainDisplayButtons() {
  if (ButtonManager::isShortPress(M5.BtnA)) {
    currentMode = ABS_TIME_INPUT;
    resetInput();
  }
  
  if (ButtonManager::isShortPress(M5.BtnB)) {
    currentMode = REL_PLUS_TIME_INPUT;
    resetInput();
  }
  
  if (ButtonManager::isLongPress(M5.BtnB, 1000)) {
    currentMode = REL_MINUS_TIME_INPUT;
    resetInput();
  }
  
  if (ButtonManager::isShortPress(M5.BtnC)) {
    currentMode = ALARM_MANAGEMENT;
  }
  
  if (ButtonManager::isLongPress(M5.BtnC, 1000)) {
    currentMode = SETTINGS_MENU;
  }
}

void handleInputModeButtons() {
  if (!DebounceManager::canProcessOperation("input_mode")) return;
  
  if (ButtonManager::isShortPress(M5.BtnA)) {
    handleDigitIncrement();
  }
  
  if (ButtonManager::isLongPress(M5.BtnA, 500)) {
    handleDigitIncrementBy5();
  }
  
  if (ButtonManager::isShortPress(M5.BtnB)) {
    handleDigitMove();
  }
  
  if (ButtonManager::isLongPress(M5.BtnB, 1000)) {
    handleDigitReset();
  }
  
  if (ButtonManager::isShortPress(M5.BtnC)) {
    handleDigitConfirm();
  }
  
  if (ButtonManager::isLongPress(M5.BtnC, 1000)) {
    currentMode = MAIN_DISPLAY;
  }
}
```

#### Step 4.2: 同時押し機能の実装
```cpp
// Step 4.2: 同時押し機能を追加
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

### Phase 5: 最適化とテスト

#### 5.1 パフォーマンス最適化
- メモリ使用量の最適化
- 処理速度の改善
- バッテリー消費の最適化

#### 5.2 テストと検証
- 各ボタン操作の動作確認
- デバウンス処理の効果検証
- 同時押し機能のテスト
- 実機での動作確認

## 実装スケジュール

### Day 1: Phase 1
- Step 1.1: 最小限のButtonManagerクラス
- Step 1.2: 最小限のDebounceManagerクラス
- 基本的なテスト

### Day 2: Phase 2
- Step 2.1: main.cppにButtonManagerを並行導入
- Step 2.2: input.cppにButtonManagerを並行導入
- 既存機能の動作確認

### Day 3: Phase 3
- Step 3.1: main.cppの一部をButtonManagerに移行
- Step 3.2: input.cppの一部をButtonManagerに移行
- 段階的な動作確認

### Day 4: Phase 4-5
- Step 4.1: 完全移行
- Step 4.2: 同時押し機能の実装
- 最適化とテスト

## 段階的実装のメリット

### 1. リスクの最小化
- 各ステップでコンパイルが通る状態を維持
- 既存機能を壊さずに段階的に移行
- 問題が発生した場合の早期発見と修正

### 2. 動作確認の容易さ
- 各ステップで動作確認が可能
- 問題の特定が容易
- 必要に応じてロールバック可能

### 3. 開発効率の向上
- 並行して開発とテストが可能
- 段階的なデバッグが可能
- 自信を持って進められる

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

1. **Step 1.1の実装開始**
   - 最小限のButtonManagerクラスの実装
   - コンパイル確認
   - 基本的なテスト

2. **既存コードの分析**
   - 現在のボタン処理の詳細分析
   - 移行対象の特定
   - 移行順序の決定

3. **テスト計画の策定**
   - 各ステップでのテスト項目の定義
   - 実機テストの計画
   - 回帰テストの計画 