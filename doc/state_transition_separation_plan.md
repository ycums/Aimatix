# 状態遷移ロジック分離・実装計画書

## 1. 現状分析

### 問題点の整理
1. **main.cppの肥大化**: handleButtons()関数が400行以上で複雑
2. **責務の混在**: ボタン処理、状態遷移、UI更新が混在
3. **テスト困難**: M5Stack依存のコードが純粋ロジックと密結合
4. **保守性の低下**: 状態遷移の変更が複数箇所に影響

### 現在の状態遷移ロジックの分布
- **main.cpp**: 約80%の状態遷移ロジック
- **input.cpp**: 入力モード関連の遷移ロジック
- **ui.cpp**: 設定メニュー関連の遷移ロジック

## 2. 分離アーキテクチャ設計

### 2.1 新しいファイル構成
```
src/
├── state_transition/
│   ├── state_transition.h      # 状態遷移ロジックのインターフェース
│   ├── state_transition.cpp    # 純粋な状態遷移ロジック
│   ├── button_event.h          # ボタンイベント定義
│   ├── button_event.cpp        # ボタンイベント変換
│   ├── system_state.h          # システム状態管理
│   ├── system_state.cpp        # システム状態取得・更新
│   ├── transition_validator.h  # 遷移妥当性チェック
│   └── transition_validator.cpp
└── main.cpp                    # リファクタリング後（シンプル化）
```

### 2.2 責務の明確な分離

#### state_transition.h/cpp
- **責務**: 純粋な状態遷移ロジック
- **依存**: なし（M5Stack非依存）
- **テスト**: 完全にnative環境でテスト可能

#### button_event.h/cpp
- **責務**: M5Stackボタン状態からButtonEventへの変換
- **依存**: M5Stack（最小限）
- **テスト**: モック化してテスト可能

#### system_state.h/cpp
- **責務**: 現在のシステム状態の取得・更新
- **依存**: グローバル変数（alarmTimes, settings等）
- **テスト**: モック化してテスト可能

#### transition_validator.h/cpp
- **責務**: 遷移の妥当性チェック
- **依存**: なし
- **テスト**: 完全にnative環境でテスト可能

## 3. インターフェース設計

### 3.1 ButtonEvent型
```cpp
// button_event.h
enum ButtonType {
  BUTTON_A,
  BUTTON_B, 
  BUTTON_C
};

enum ButtonAction {
  SHORT_PRESS,    // 短押し
  LONG_PRESS,     // 長押し（1秒以上）
  RELEASE         // リリース
};

struct ButtonEvent {
  ButtonType button;
  ButtonAction action;
  unsigned long timestamp;
  
  ButtonEvent(ButtonType btn, ButtonAction act) 
    : button(btn), action(act), timestamp(millis()) {}
    
  bool operator==(const ButtonEvent& other) const {
    return button == other.button && action == other.action;
  }
};
```

### 3.2 SystemState型
```cpp
// system_state.h
struct SystemState {
  Mode currentMode;
  int selectedIndex;           // 現在の選択インデックス
  size_t alarmCount;           // アラーム数
  bool warningDisplayed;       // 警告表示中か
  bool alarmActive;            // アラーム鳴動中か
  InputState inputState;       // 入力状態
  SettingsMenu settingsMenu;   // 設定メニュー状態
  
  // 境界値チェック用
  bool isValidIndex() const;
  bool canAddAlarm() const;
  bool canDeleteAlarm() const;
};
```

### 3.3 TransitionResult型
```cpp
// state_transition.h
struct TransitionResult {
  Mode nextMode;
  bool isValid;
  const char* errorMessage;
  TransitionAction action;
  
  TransitionResult(Mode mode, bool valid = true, const char* error = nullptr)
    : nextMode(mode), isValid(valid), errorMessage(error), action(ACTION_NONE) {}
};

enum TransitionAction {
  ACTION_NONE,
  ACTION_RESET_INPUT,
  ACTION_ADD_ALARM,
  ACTION_DELETE_ALARM,
  ACTION_UPDATE_SETTINGS,
  ACTION_STOP_ALARM
};
```

## 4. 実装戦略

### 4.1 Phase 1: 基盤構築（1-2日目）

#### 4.1.1 型定義の実装
```cpp
// button_event.h/cpp
class ButtonEventConverter {
public:
  static ButtonEvent fromM5Button(Button& button, ButtonAction action);
  static std::vector<ButtonEvent> getCurrentEvents();
};

// system_state.h/cpp
class SystemStateManager {
public:
  static SystemState getCurrentState();
  static void updateState(const SystemState& newState);
  static bool isValidTransition(Mode from, Mode to);
};
```

#### 4.1.2 基本的な状態遷移関数
```cpp
// state_transition.h/cpp
class StateTransitionManager {
public:
  static TransitionResult handleTransition(
    Mode currentMode, 
    ButtonEvent event, 
    const SystemState& systemState
  );
  
private:
  static TransitionResult handleMainDisplayTransition(
    ButtonEvent event, 
    const SystemState& systemState
  );
  
  static TransitionResult handleAbsTimeInputTransition(
    ButtonEvent event, 
    const SystemState& systemState
  );
  
  static TransitionResult handleRelPlusTimeInputTransition(
    ButtonEvent event, 
    const SystemState& systemState
  );
  
  static TransitionResult handleAlarmManagementTransition(
    ButtonEvent event, 
    const SystemState& systemState
  );
  
  static TransitionResult handleSettingsMenuTransition(
    ButtonEvent event, 
    const SystemState& systemState
  );
  
  // 他のモード用のハンドラーも同様に実装
};
```

### 4.2 Phase 2: 詳細実装（3-4日目）

#### 4.2.1 全状態遷移パターンの実装
- 各モードごとの遷移ハンドラーを実装
- エッジケースの処理を追加
- エラーハンドリングを強化

#### 4.2.2 遷移妥当性チェックの実装
```cpp
// transition_validator.h/cpp
class TransitionValidator {
public:
  static bool isValidTransition(Mode from, Mode to, const SystemState& state);
  static bool isButtonEventValid(ButtonEvent event, Mode currentMode);
  static const char* getValidationError(Mode from, Mode to, const SystemState& state);
  
private:
  static bool checkWarningDisplayed(const SystemState& state);
  static bool checkAlarmActive(const SystemState& state);
  static bool checkIndexBounds(const SystemState& state);
};
```

### 4.3 Phase 3: 統合・テスト（5-6日目）

#### 4.3.1 main.cppのリファクタリング
```cpp
// main.cpp（リファクタリング後）
void handleButtons() {
  // 警告メッセージ表示中は処理をスキップ
  if (isWarningMessageDisplayed("")) {
    return;
  }
  
  // 現在のシステム状態を取得
  SystemState currentState = SystemStateManager::getCurrentState();
  
  // ボタンイベントを取得
  std::vector<ButtonEvent> events = ButtonEventConverter::getCurrentEvents();
  
  // 各イベントを処理
  for (const ButtonEvent& event : events) {
    TransitionResult result = StateTransitionManager::handleTransition(
      currentState.currentMode, 
      event, 
      currentState
    );
    
    if (result.isValid) {
      // 状態遷移を実行
      currentState.currentMode = result.nextMode;
      SystemStateManager::updateState(currentState);
      
      // アクションを実行
      executeTransitionAction(result.action);
    } else {
      // エラー処理（ログ出力等）
      Serial.println(result.errorMessage);
    }
  }
}

void executeTransitionAction(TransitionAction action) {
  switch (action) {
    case ACTION_RESET_INPUT:
      resetInput();
      break;
    case ACTION_ADD_ALARM:
      // アラーム追加処理
      break;
    case ACTION_DELETE_ALARM:
      // アラーム削除処理
      break;
    // 他のアクションも同様に実装
  }
}
```

## 5. テスト戦略

### 5.1 単体テスト
```cpp
// test_state_transition.cpp
void testMainDisplayTransitions() {
  SystemState state;
  state.currentMode = MAIN_DISPLAY;
  state.warningDisplayed = false;
  state.alarmActive = false;
  
  // A短押し → ABS_TIME_INPUT
  ButtonEvent event(BUTTON_A, SHORT_PRESS);
  TransitionResult result = StateTransitionManager::handleTransition(
    MAIN_DISPLAY, event, state
  );
  
  assert(result.isValid);
  assert(result.nextMode == ABS_TIME_INPUT);
  assert(result.action == ACTION_RESET_INPUT);
}

void testInvalidTransitions() {
  SystemState state;
  state.currentMode = MAIN_DISPLAY;
  state.warningDisplayed = true;  // 警告表示中
  
  ButtonEvent event(BUTTON_A, SHORT_PRESS);
  TransitionResult result = StateTransitionManager::handleTransition(
    MAIN_DISPLAY, event, state
  );
  
  assert(!result.isValid);
  assert(strstr(result.errorMessage, "warning") != nullptr);
}
```

### 5.2 統合テスト
```cpp
// test_integration.cpp
void testCompleteUserFlow() {
  // メイン画面 → 絶対時刻入力 → 確定 → メイン画面
  SystemState state;
  state.currentMode = MAIN_DISPLAY;
  
  // 1. A短押しで絶対時刻入力へ
  TransitionResult result1 = StateTransitionManager::handleTransition(
    MAIN_DISPLAY, ButtonEvent(BUTTON_A, SHORT_PRESS), state
  );
  assert(result1.nextMode == ABS_TIME_INPUT);
  
  // 2. C短押しで確定
  state.currentMode = ABS_TIME_INPUT;
  TransitionResult result2 = StateTransitionManager::handleTransition(
    ABS_TIME_INPUT, ButtonEvent(BUTTON_C, SHORT_PRESS), state
  );
  assert(result2.nextMode == MAIN_DISPLAY);
}
```

## 6. 移行計画

### 6.1 段階的移行
1. **Step 1**: 新しいファイルを作成し、基本的な型定義を実装
2. **Step 2**: 単純な遷移（メイン画面のA/B/C短押し）を新しいシステムで実装
3. **Step 3**: 既存のhandleButtons()関数を段階的に置き換え
4. **Step 4**: テストケースを作成・実行
5. **Step 5**: 古いコードを削除

### 6.2 リスク管理
- **ビルドエラー**: 段階的移行により最小化
- **機能退行**: テストケースで検出
- **パフォーマンス**: プロファイリングで確認

## 7. 成功指標

### 7.1 技術的指標
- [ ] main.cppの行数削減（400行 → 100行以下）
- [ ] 状態遷移ロジックの100%分離
- [ ] テストカバレッジ20%台到達
- [ ] ビルド時間の短縮

### 7.2 品質指標
- [ ] 全状態遷移パターンのテスト通過
- [ ] エッジケースの適切な処理
- [ ] エラーメッセージの明確化
- [ ] コードの可読性向上

### 7.3 保守性指標
- [ ] 新機能追加時の変更箇所の明確化
- [ ] バグ修正時の影響範囲の限定
- [ ] テスト実行時間の短縮（native環境）
- [ ] デバッグの容易性向上 