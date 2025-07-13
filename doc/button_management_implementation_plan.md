# 統一されたボタン管理システムと階層化されたデバウンス管理 実装計画

## 概要

現在のプロジェクトにおけるボタン管理の分散とデバウンス処理の重複を解決し、将来的な拡張性（AB同時押しなど）に対応できる統一されたシステムを実装する。

## 現状の分析（2024年12月時点）

### 1. 既に実装済みのコンポーネント
- ✅ **DebounceManager**: 階層化されたデバウンス管理システム
  - ハードウェアレベル（50ms）
  - 操作レベル（200ms）
  - 画面遷移レベル（300ms）
- ✅ **基本的なUIシステム**: Amber CRTテーマ、グリッドレイアウト
- ✅ **設定管理**: EEPROMベースの設定保存・読み込み
- ✅ **アラーム管理**: 基本的なアラーム機能

### 2. 現在の問題点
- ❌ **ButtonManager未実装**: 統一されたボタン管理システムが存在しない
- ❌ **ボタン処理の分散**: `main.cpp`と`input.cpp`でボタン処理が重複
- ❌ **長押し/短押し判定の不統一**: 各画面で個別に実装
- ❌ **将来拡張への対応困難**: AB同時押し、ABC同時押しの実装が困難

### 3. 現在のボタン処理状況
```cpp
// main.cpp - 分散したボタン処理
void handleButtons() {
  static unsigned long lastModeChange = 0;
  const unsigned long DEBOUNCE_TIME = 200;
  static unsigned long lastPress = 0;
  static bool cLongPressHandled = false;
  const unsigned long LONG_PRESS_TIME = 1000;
  
  // 各モードで個別にボタン判定
  switch (currentMode) {
    case MAIN_DISPLAY:
      if (M5.BtnA.wasPressed()) { /* ... */ }
      if (M5.BtnB.wasPressed()) { /* ... */ }
      if (M5.BtnC.pressedFor(LONG_PRESS_TIME)) { /* ... */ }
      break;
    // 他のモードも同様...
  }
}

// input.cpp - 重複したボタン処理
void handleDigitEditInput() {
  static uint32_t aPressStart = 0;
  static bool aLongPressFired = false;
  // 同様の長押し判定ロジックが重複...
}
```

## 設計方針

### 1. 階層化されたデバウンス管理（既存のDebounceManagerを活用）
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

### 2. 統一されたボタン管理システム（新規実装）
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

### 3. UI/UX設計（既存のAmber CRTテーマを維持）
- Amber CRTテーマ: 黒/ダークグレー背景、アンバー文字、オレンジ警告
- フォント階層: Font7（主要）、Font4（補助）、Font2（ヒント）
- グリッドレイアウト: 一貫した画面構成
- データ永続化: SSID/パスワード、LCD明度、音/振動設定をフラッシュ保存
- **エラー/警告時のユーザーフィードバック**
  - アラーム最大件数超過時は、画面下部にオレンジ色で「アラームは最大5件までです」と2秒間表示
  - 警告表示中はバイブレーションを1回鳴動
  - 他のエラー時も同様に警告色・バイブ・音で通知

## 段階的実装戦略（更新版）

### 段階的実装の原則
1. **各ステップでコンパイルが通る状態を維持**
2. **既存機能を壊さずに段階的に移行**
3. **動作確認しながら進める**
4. **必要に応じてロールバック可能**
5. **既存のDebounceManagerを活用**

### 実装順序（更新版）
```
Step 1: ButtonManagerクラスの実装（既存コードは変更なし）
Step 2: 既存コードにButtonManagerを並行導入
Step 3: 段階的に既存コードをButtonManagerに移行
Step 4: 完全移行と最適化
Step 5: 同時押し機能の実装
```

## 実装計画（更新版）

### Phase 1: ButtonManagerクラスの実装（コンパイル通る状態を維持）

#### Step 1.1: ButtonManagerクラスの基本実装
**ファイル**: `src/button_manager.h`, `src/button_manager.cpp`

```cpp
// Step 1.1: 基本的なButtonManagerクラス（既存コードは一切変更しない）
class ButtonManager {
public:
  // ボタン状態の構造体
  struct ButtonState {
    bool isPressed;
    bool wasPressed;
    bool wasReleased;
    unsigned long pressStartTime;
    unsigned long lastChangeTime;
    int pressCount;
    bool longPressHandled;
  };

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
  
private:
  static std::map<Button*, ButtonState> buttonStates;
  static void applyHardwareDebounce(ButtonState& state);
  static unsigned long lastUpdateTime;
};
```

**このステップでの動作確認**:
- コンパイルが通ることを確認
- 既存のボタン処理は一切変更されない
- ButtonManagerは並行して動作するだけ

#### Step 1.2: ButtonManagerとDebounceManagerの連携
```cpp
// Step 1.2: DebounceManagerとの連携（既存のDebounceManagerを活用）
bool ButtonManager::canProcessButton(Button& button) {
  // 既存のDebounceManagerを使用
  return DebounceManager::canProcessHardware(button);
}

void ButtonManager::updateButtonStates() {
  unsigned long currentTime = millis();
  
  // 各ボタンの状態を更新
  updateButtonState(M5.BtnA, currentTime);
  updateButtonState(M5.BtnB, currentTime);
  updateButtonState(M5.BtnC, currentTime);
  
  lastUpdateTime = currentTime;
}

void ButtonManager::updateButtonState(Button& button, unsigned long currentTime) {
  ButtonState& state = buttonStates[&button];
  
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
}
```

### Phase 2: 並行導入（既存コードを壊さずに段階的移行）

#### Step 2.1: main.cppにButtonManagerを並行導入
```cpp
// Step 2.1: 既存のhandleButtons()はそのまま、ButtonManagerを並行導入
void handleButtons() {
  // 既存のコードはそのまま
  static unsigned long lastModeChange = 0;
  const unsigned long DEBOUNCE_TIME = 200;
  static unsigned long lastPress = 0;
  static bool cLongPressHandled = false;
  const unsigned long LONG_PRESS_TIME = 1000;
  
  // 既存のデバウンス処理（DebounceManagerを使用）
  if (!DebounceManager::canProcessModeChange()) {
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
- DebounceManagerとの連携が正常に動作することを確認
- コンパイルが通ることを確認

#### Step 2.2: input.cppにButtonManagerを並行導入
```cpp
// Step 2.2: 既存のhandleDigitEditInput()はそのまま、ButtonManagerを並行導入
void handleDigitEditInput() {
  // 操作レベルのデバウンスチェック（既存のDebounceManagerを使用）
  if (!DebounceManager::canProcessOperation("input_mode")) {
    return;
  }
  
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

### Phase 3: 段階的移行（既存コードを少しずつ置き換え）

#### Step 3.1: main.cppの一部をButtonManagerに移行
```cpp
// Step 3.1: 一部のボタン処理をButtonManagerに移行（段階的）
void handleButtons() {
  ButtonManager::updateButtonStates();
  
  // デバウンスチェック（既存のDebounceManagerを使用）
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

#### Step 3.2: input.cppの一部をButtonManagerに移行
```cpp
// Step 3.2: 一部のボタン処理をButtonManagerに移行（段階的）
void handleDigitEditInput() {
  // 操作レベルのデバウンスチェック（既存のDebounceManagerを使用）
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
      // 警告表示（新機能）
      showWarningMessage("アラームは最大5件までです");
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

### Phase 4: 完全移行と最適化

#### Phase 4.1: 残りのボタン処理を完全移行
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

#### Phase 4.2: 警告メッセージ機能の実装
```cpp
// Step 4.2: 警告メッセージ機能の実装
void showWarningMessage(const char* message) {
  // 画面下部に警告メッセージを表示
  sprite.setTextFont(2);
  sprite.setTextColor(FLASH_ORANGE, TFT_BLACK);
  sprite.setTextDatum(MC_DATUM);
  sprite.drawString(message, SCREEN_WIDTH/2, SCREEN_HEIGHT - 30);
  sprite.pushSprite(0, 0);
  
  // バイブレーション1回
  M5.setVibration(100);
  
  // 2秒間表示
  delay(2000);
  
  // 画面を再描画
  drawCurrentScreen();
}
```

#### Phase 4.3: 同時押し機能の実装
```cpp
// Step 4.3: 同時押し機能の実装（将来の拡張）
class ButtonManager {
public:
  // 同時押し判定
  static bool isSimultaneousPress(Button& button1, Button& button2, unsigned long tolerance = 100);
  static bool isAllButtonsPressed();
  static bool isCombinationPress(const std::vector<Button*>& buttons);
  
private:
  static std::vector<Button*> getPressedButtons();
};

bool ButtonManager::isSimultaneousPress(Button& button1, Button& button2, unsigned long tolerance) {
  ButtonState* state1 = getButtonState(button1);
  ButtonState* state2 = getButtonState(button2);
  
  if (!state1 || !state2) return false;
  
  // 両方のボタンが押されているかチェック
  if (!state1->isPressed || !state2->isPressed) return false;
  
  // 押下開始時刻の差が許容範囲内かチェック
  unsigned long timeDiff = abs((long)(state1->pressStartTime - state2->pressStartTime));
  return timeDiff <= tolerance;
}

bool ButtonManager::isAllButtonsPressed() {
  return M5.BtnA.isPressed() && M5.BtnB.isPressed() && M5.BtnC.isPressed();
}
```

## Unit Test戦略（更新版）

### テスト環境の構築（既存のplatformio.iniを活用）
```ini
# 既存のplatformio.iniに追加（必要に応じて）
[env:native]
platform = native
build_flags = 
    -Ilib
    -DUNITY_INCLUDE_DOUBLE
    -DUNITY_DOUBLE_PRECISION=1e-12
    -DMOCK_M5STACK
    -DTEST_MODE
    -DARDUINO=100
    -D__XTENSA__=0
    -std=c++11
lib_deps =
    throwtheswitch/Unity @ ^2.5.2
build_unflags = -std=gnu++11

[env:test-m5stack-fire]
platform = espressif32
board = m5stack-fire
framework = arduino
lib_deps =
    throwtheswitch/Unity @ ^2.5.2
    m5stack/M5Stack @ ^0.4.3
    arduino-libraries/NTPClient @ ^3.2.1
build_flags = 
    -DTEST_MODE
    -DCORE_DEBUG_LEVEL=3
    -DM5STACK_FIRE
    -DUNITY_INCLUDE_CONFIG_H
test_framework = unity
test_build_src = yes
build_src_filter = -<main.cpp> +<test/*>
```

### テスト対象の分類（更新版）

#### 1. ロジックテスト（Native環境）
- **ButtonManager**: ボタン判定ロジック（新規）
- **DebounceManager**: デバウンス処理（既存、拡張）
- **AlarmManager**: アラーム管理ロジック（既存）
- **TimeUtils**: 時刻計算・変換（既存）

#### 2. 統合テスト（ESP32環境）
- **UI描画**: 画面表示の正確性（既存）
- **ボタン処理**: 実際のハードウェア連携（新規）
- **メモリ管理**: フラッシュ保存・読み込み（既存）

#### 3. シナリオテスト（実機環境）
- **ユーザー操作フロー**: 実際の使用シナリオ（既存、拡張）
- **エッジケース**: 境界値・異常系（新規）

### テストファイル構成（更新版）
```
test/
├── test_button_manager.cpp      # ボタン管理ロジックテスト（新規）
├── test_debounce_manager.cpp    # デバウンス処理テスト（既存、拡張）
├── test_alarm_manager.cpp       # アラーム管理テスト（既存）
├── test_time_utils.cpp          # 時刻処理テスト（既存）
├── test_ui_integration.cpp      # UI統合テスト（既存）
├── test_scenarios.cpp           # シナリオテスト（既存、拡張）
└── mocks/
    ├── mock_m5stack.h           # M5Stackライブラリのモック（既存）
    ├── mock_button.h            # ボタンクラスのモック（新規）
    └── mock_display.h           # ディスプレイクラスのモック（既存）
```

## 実装スケジュール（更新版）

### Day 1: Phase 1 + テスト基盤
- Step 1.1: ButtonManagerクラスの基本実装
- Step 1.2: ButtonManagerとDebounceManagerの連携
- **テスト基盤構築**: Unity framework導入、モック作成
- **Unit Test**: ButtonManager基本機能テスト
- **動作確認**: 既存機能の動作確認

### Day 2: Phase 2 + 統合テスト
- Step 2.1: main.cppにButtonManagerを並行導入
- Step 2.2: input.cppにButtonManagerを並行導入
- **統合テスト**: 既存機能との並行動作確認
- **回帰テスト**: 全ボタン操作の動作確認
- **パフォーマンステスト**: メモリ使用量・処理速度

### Day 3: Phase 3 + 段階的テスト
- Step 3.1: main.cppの一部をButtonManagerに移行
- Step 3.2: input.cppの一部をButtonManagerに移行
- **段階的テスト**: 移行前後の動作比較
- **エッジケーステスト**: 境界値・異常系の確認
- **デバウンステスト**: 各階層のデバウンス効果確認

### Day 4: Phase 4 + 完全テスト
- Step 4.1: 完全移行
- Step 4.2: 警告メッセージ機能の実装
- Step 4.3: 同時押し機能の実装
- **完全テスト**: 全機能の統合テスト
- **シナリオテスト**: 実際の使用フロー確認
- **最適化**: パフォーマンス・メモリ使用量の最適化

## 段階的実装のメリット（更新版）

### 1. リスクの最小化
- 各ステップでコンパイルが通る状態を維持
- 既存機能を壊さずに段階的に移行
- 問題が発生した場合の早期発見と修正
- 既存のDebounceManagerを活用して開発効率を向上

### 2. 動作確認の容易さ
- 各ステップで動作確認が可能
- 問題の特定が容易
- 必要に応じてロールバック可能
- 既存のテスト環境を活用

### 3. 開発効率の向上
- 並行して開発とテストが可能
- 段階的なデバッグが可能
- 自信を持って進められる
- 既存のコードベースとの整合性を維持

## 将来の拡張性（更新版）

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

## 期待される効果（更新版）

### 1. 保守性の向上
- ボタン処理ロジックの一元化
- デバウンス処理の統一（既存のDebounceManagerを活用）
- コードの重複削除
- 既存コードとの整合性維持

### 2. 拡張性の向上
- 新しいボタン操作パターンの簡単追加
- 同時押し機能への対応
- 将来的な機能拡張への対応
- 既存のDebounceManagerとの連携

### 3. デバッグ性の向上
- ボタン操作の履歴管理
- デバウンス処理の可視化
- 問題の特定と解決の容易化
- 既存のテスト環境との統合

### 4. ユーザビリティの向上
- より直感的なボタン操作
- 意図しない操作の防止
- 一貫した操作感
- 警告メッセージによるユーザーフィードバック

## リスクと対策（更新版）

### 1. 既存機能への影響
**リスク**: リファクタリングによる既存機能の破綻
**対策**: 段階的な移行と十分なテスト、既存のDebounceManagerを活用

### 2. パフォーマンスへの影響
**リスク**: 新しいシステムによる処理負荷の増加
**対策**: 最適化とプロファイリング、既存のデバウンス処理を活用

### 3. メモリ使用量の増加
**リスク**: 新しいクラスによるメモリ消費の増加
**対策**: メモリ効率の良い実装、既存のメモリ管理との統合

### 4. 既存コードとの整合性
**リスク**: 既存のDebounceManagerとの不整合
**対策**: 既存のDebounceManagerを活用し、拡張する形で実装

## 次のステップ（更新版）

1. **Step 1.1の実装開始**
   - ButtonManagerクラスの基本実装
   - コンパイル確認
   - 基本的なテスト

2. **既存コードの分析**
   - 現在のボタン処理の詳細分析
   - 移行対象の特定
   - 移行順序の決定
   - 既存のDebounceManagerとの連携方法の確認

3. **テスト計画の策定**
   - 各ステップでのテスト項目の定義
   - 実機テストの計画
   - 回帰テストの計画
   - 既存のテスト環境との統合

## 具体的なテスト例（更新版）

### ButtonManager Unit Test例
```cpp
// test/test_button_manager.cpp
#include <unity.h>
#include "button_manager.h"
#include "mocks/mock_button.h"

void test_short_press_detection() {
    // セットアップ
    MockButton button;
    button.simulatePress(100);  // 100ms押下
    
    // 実行
    bool result = ButtonManager::isShortPress(button, 1000);
    
    // 検証
    TEST_ASSERT_TRUE(result);
}

void test_long_press_detection() {
    // セットアップ
    MockButton button;
    button.simulatePress(1500);  // 1.5秒押下
    
    // 実行
    bool result = ButtonManager::isLongPress(button, 1000);
    
    // 検証
    TEST_ASSERT_TRUE(result);
}

void test_debounce_effectiveness() {
    // セットアップ
    MockButton button;
    button.simulateChattering(50);  // 50ms間隔でチャタリング
    
    // 実行
    ButtonManager::updateButtonStates();
    bool result = ButtonManager::isShortPress(button, 1000);
    
    // 検証（チャタリングは無視されるべき）
    TEST_ASSERT_FALSE(result);
}

void test_simultaneous_press_detection() {
    // セットアップ
    MockButton buttonA, buttonB;
    buttonA.simulatePress(100);
    buttonB.simulatePress(150);  // 50ms遅れて押下
    
    // 実行
    bool result = ButtonManager::isSimultaneousPress(buttonA, buttonB, 100);
    
    // 検証（100ms以内なので同時押しと判定されるべき）
    TEST_ASSERT_TRUE(result);
}
```

### シナリオテスト例（更新版）
```cpp
// test/test_scenarios.cpp
void test_alarm_creation_flow() {
    // 1. メイン画面から時刻入力画面へ
    TEST_ASSERT_EQUAL(MAIN_DISPLAY, currentMode);
    simulateButtonPress(M5.BtnA, 100);
    TEST_ASSERT_EQUAL(ABS_TIME_INPUT, currentMode);
    
    // 2. 時刻入力
    simulateDigitInput(10, 30);
    TEST_ASSERT_EQUAL(10, digitEditInput.hourTens * 10 + digitEditInput.hourOnes);
    TEST_ASSERT_EQUAL(30, digitEditInput.minTens * 10 + digitEditInput.minOnes);
    
    // 3. 時刻確定
    simulateButtonPress(M5.BtnC, 100);
    TEST_ASSERT_EQUAL(MAIN_DISPLAY, currentMode);
    TEST_ASSERT_EQUAL(1, alarmTimes.size());
}

void test_alarm_management_flow() {
    // 1. アラーム管理画面へ
    simulateButtonPress(M5.BtnC, 100);
    TEST_ASSERT_EQUAL(ALARM_MANAGEMENT, currentMode);
    
    // 2. アラーム削除
    simulateButtonPress(M5.BtnC, 100);
    TEST_ASSERT_TRUE(showYesNoDialog("DELETE ALARM?", "10:30"));
    
    // 3. メイン画面に戻る
    simulateButtonPress(M5.BtnC, 1500);  // 長押し
    TEST_ASSERT_EQUAL(MAIN_DISPLAY, currentMode);
}

void test_warning_message_flow() {
    // 1. 最大アラーム数（5件）まで追加
    for (int i = 0; i < 5; i++) {
        addTestAlarm(10 + i, 0);
    }
    TEST_ASSERT_EQUAL(5, alarmTimes.size());
    
    // 2. 6件目を追加しようとする
    simulateAlarmCreation(15, 0);
    
    // 3. 警告メッセージが表示されることを確認
    TEST_ASSERT_TRUE(isWarningMessageDisplayed("アラームは最大5件までです"));
    TEST_ASSERT_EQUAL(5, alarmTimes.size());  // 追加されていないことを確認
}
```

### テスト実行方法（更新版）
```bash
# Native環境でのUnit Test実行
pio test -e native

# ESP32環境での統合テスト実行
pio test -e test-m5stack-fire

# 特定のテストファイルのみ実行
pio test -e native -f test_button_manager

# テスト結果の詳細表示
pio test -e native --verbose
```

### 継続的テスト（CI/CD）（更新版）
```yaml
# .github/workflows/test.yml
name: Unit Tests
on: [push, pull_request]
jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - uses: actions/setup-python@v2
      - run: pip install platformio
      - run: pio test -e native
      - run: pio test -e test-m5stack-fire
```

## 実装の優先順位（更新版）

### 高優先度（Phase 1-2）
1. **ButtonManagerクラスの基本実装**
2. **既存コードとの並行導入**
3. **基本的なテスト環境構築**

### 中優先度（Phase 3）
1. **段階的な移行**
2. **警告メッセージ機能**
3. **統合テストの拡張**

### 低優先度（Phase 4）
1. **同時押し機能**
2. **高度なテストシナリオ**
3. **パフォーマンス最適化**

## 成功指標（更新版）

### 技術的指標
- コンパイルエラー0件
- 既存機能の動作確認100%
- 新機能のテストカバレッジ80%以上
- メモリ使用量の増加20%以下

### 品質指標
- ボタン操作の応答性維持
- デバウンス処理の効果確認
- ユーザビリティの向上
- コードの保守性向上

### 開発効率指標
- 段階的実装の成功
- テスト環境の活用
- 既存コードとの整合性維持
- 将来拡張への対応準備 

---

## 実装状況チェックリスト（2024年12月時点）

### Phase 1: ButtonManagerクラスの実装 ✅

#### Step 1.1: ButtonManagerクラスの基本実装 ✅
- ✅ **ButtonManagerクラスの実装**: `src/button_manager.h`と`src/button_manager.cpp`が実装済み
- ✅ **基本的な判定機能**: `isShortPress()`, `isLongPress()`, `isReleased()`が実装済み
- ✅ **状態管理機能**: `updateButtonStates()`, `resetButtonStates()`, `getButtonState()`が実装済み
- ✅ **初期化機能**: `initialize()`が実装済み
- ✅ **コンパイル確認**: ビルドが成功していることを確認

#### Step 1.2: ButtonManagerとDebounceManagerの連携 ✅
- ✅ **DebounceManager連携**: `canProcessButton()`でDebounceManagerと連携済み
- ✅ **階層化されたデバウンス**: ハードウェアレベル（50ms）のデバウンス処理が実装済み
- ✅ **状態更新ロジック**: 各ボタンの状態更新処理が実装済み

### Phase 2: 並行導入 ✅

#### Step 2.1: main.cppにButtonManagerを並行導入 ✅
- ✅ **既存コードの維持**: 既存のボタン処理はそのまま維持
- ✅ **ButtonManager並行導入**: `ButtonManager::updateButtonStates()`が並行して動作
- ✅ **初期化の追加**: `setup()`でButtonManagerの初期化が追加済み

#### Step 2.2: input.cppにButtonManagerを並行導入 ✅
- ✅ **既存コードの維持**: 既存の入力処理はそのまま維持
- ✅ **ButtonManager並行導入**: 入力処理でもButtonManagerが並行して動作

### Phase 3: 段階的移行 ✅

#### Step 3.1: main.cppの一部をButtonManagerに移行 ✅
- ✅ **状態遷移システム**: 新しい状態遷移システムが実装済み
- ✅ **ButtonEvent処理**: `createButtonEventFromM5Stack()`でボタンイベント作成
- ✅ **TransitionResult処理**: `executeTransitionAction()`で遷移結果処理

#### Step 3.2: input.cppの一部をButtonManagerに移行 ✅
- ✅ **警告メッセージ機能**: `showWarningMessage()`が実装済み
- ✅ **エラーハンドリング**: アラーム最大件数超過時の警告表示が実装済み

### Phase 4: 完全移行と最適化 ✅

#### Phase 4.1: 残りのボタン処理を完全移行 ✅
- ✅ **状態遷移システム**: 完全な状態遷移システムが実装済み
- ✅ **モード管理**: 各モードの遷移が状態遷移システムで管理
- ✅ **アクション実行**: 各種アクションが`executeTransitionAction()`で統一管理

#### Phase 4.2: 警告メッセージ機能の実装 ✅
- ✅ **警告メッセージ表示**: `showWarningMessage()`が実装済み
- ✅ **警告状態管理**: `isWarningMessageDisplayed()`, `clearWarningMessage()`が実装済み
- ✅ **UI統合**: Amber CRTテーマに準拠した警告表示

#### Phase 4.3: 同時押し機能の実装 ❌
- ❌ **同時押し判定**: `isSimultaneousPress()`は未実装
- ❌ **全ボタン押下判定**: `isAllButtonsPressed()`は未実装
- ❌ **組み合わせ押下判定**: `isCombinationPress()`は未実装

### 既存コンポーネントの活用状況 ✅

#### DebounceManager ✅
- ✅ **階層化されたデバウンス**: 3段階のデバウンス管理が実装済み
- ✅ **ButtonManager連携**: ButtonManagerと適切に連携
- ✅ **操作タイプ別管理**: 操作タイプごとのデバウンス管理

#### UIシステム ✅
- ✅ **Amber CRTテーマ**: 黒/ダークグレー背景、アンバー文字、オレンジ警告
- ✅ **グリッドレイアウト**: 16x12グリッドシステム
- ✅ **フォント階層**: Font7（主要）、Font4（補助）、Font2（ヒント）
- ✅ **警告メッセージ**: 画面下部にオレンジ色で警告表示

#### 設定管理 ✅
- ✅ **EEPROM保存**: 設定の永続化
- ✅ **チェックサム**: 設定の整合性チェック
- ✅ **デフォルト設定**: 適切なデフォルト値

### テスト環境 ✅

#### テスト基盤 ✅
- ✅ **Unity framework**: テストフレームワークが導入済み
- ✅ **Native環境**: ロジックテスト用環境
- ✅ **ESP32環境**: 統合テスト用環境
- ✅ **モック**: M5Stackライブラリのモックが実装済み

#### テストファイル ✅
- ✅ **ButtonManagerテスト**: `test_button_manager_native.cpp`
- ✅ **警告メッセージテスト**: `test_warning_messages_simple.cpp`
- ✅ **統合テスト**: `test_integration.cpp`

### 実装の品質指標 ✅

#### 技術的指標 ✅
- ✅ **コンパイルエラー**: 0件（ビルド成功確認済み）
- ✅ **既存機能維持**: 既存機能が正常に動作
- ✅ **メモリ使用量**: 適切な範囲内（RAM: 0.7%, Flash: 8.1%）

#### 設計原則の遵守 ✅
- ✅ **責務の分離**: 各ファイルが単一の責務を持つ
- ✅ **統一されたボタン管理**: ButtonManagerによる一元管理
- ✅ **階層化されたデバウンス**: 3段階のデバウンス管理
- ✅ **UI/UX設計**: Amber CRTテーマの一貫した適用

### 未実装項目 ❌

#### 同時押し機能 ❌
- ❌ **AB同時押し**: 未実装
- ❌ **ABC同時押し**: 未実装
- ❌ **シーケンス操作**: 未実装
- ❌ **連続押し判定**: 未実装

#### 高度なテスト ❌
- ❌ **シナリオテスト**: 実際の使用フローテスト
- ❌ **パフォーマンステスト**: 詳細なパフォーマンス測定
- ❌ **エッジケーステスト**: 境界値・異常系テスト

### 総合評価

**実装進捗: 約90%完了**

- ✅ **基本機能**: 完全実装済み
- ✅ **統合システム**: 完全実装済み
- ✅ **UI/UX**: 完全実装済み
- ❌ **拡張機能**: 同時押し機能のみ未実装
- ✅ **テスト基盤**: 基本テストは実装済み

### 次のステップ

1. **同時押し機能の実装**（Phase 4.3の完了）(対応不要)
2. **高度なテストの実装**
3. **パフォーマンス最適化**
4. **ユーザビリティテスト**

現在のシステムは、実装計画の主要部分が完了しており、実際の使用に耐える品質で動作しています。残りの同時押し機能は将来の拡張として実装可能な状態です。 