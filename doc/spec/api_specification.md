# 状態遷移システム API仕様書

## 1. 概要

状態遷移システムは、M5Stack Fireのボタン操作による画面遷移・モード遷移を管理する純粋関数ベースのシステムです。
**Aimatixではコマンド/イベント駆動設計を標準とし、状態遷移の結果として副作用（UI更新・保存・通信等）をコマンド/イベントリストで一元管理します。**

## 2. 型定義

### 2.1 ButtonEvent型

```cpp
struct ButtonEvent {
    ButtonType button;    // ボタン種別
    ButtonAction action;  // アクション種別
    
    ButtonEvent();  // デフォルトコンストラクタ
    ButtonEvent(ButtonType btn, ButtonAction act);  // パラメータ付きコンストラクタ
};
```

#### ButtonType列挙型
```cpp
enum ButtonType {
    BUTTON_TYPE_A,  // Aボタン
    BUTTON_TYPE_B,  // Bボタン
    BUTTON_TYPE_C   // Cボタン
};
```

#### ButtonAction列挙型
```cpp
enum ButtonAction {
    SHORT_PRESS,    // 短押し
    LONG_PRESS      // 長押し（1秒以上）
};
```

### 2.2 SystemState型

```cpp
struct SystemState {
    int selectedIndex;           // 選択インデックス
    int alarmCount;             // アラーム数
    bool isWarningDisplayed;    // 警告表示中フラグ
    bool isAlarmActive;         // アラーム鳴動中フラグ
    time_t currentTime;         // 現在時刻
    
    SystemState();  // デフォルトコンストラクタ
};
```

### 2.3 Command/Effect型（コマンド/イベント駆動設計・標準）

```cpp
// コマンドの種類
enum class CommandType {
    UpdateUI,
    SaveSettings,
    ShowWarning,
    PlayAlarm,
    StopAlarm,
    // ... 必要に応じて拡張
};

// コマンド構造体
struct Command {
    CommandType type;
    // 必要に応じてパラメータを追加
    // 例: UI更新内容、保存対象データ、警告メッセージ等
};

// 状態遷移結果（コマンド/イベント駆動設計用）
struct TransitionEffect {
    Mode nextMode;                  // 次のモード
    std::vector<Command> commands;  // 副作用コマンドリスト
    const char* errorMessage;       // エラーメッセージ（必要時）
};
```

---

## 3. 主要関数

### 3.1 ボタンイベント関連

#### isValidButtonEvent
```cpp
bool isValidButtonEvent(const ButtonEvent& event);
```
**説明**: ボタンイベントの妥当性をチェック
**引数**: 
- `event`: チェック対象のボタンイベント
**戻り値**: 妥当な場合はtrue、そうでなければfalse

#### createButtonEventFromM5Stack
```cpp
ButtonEvent createButtonEventFromM5Stack();
```
**説明**: M5Stackボタン状態からボタンイベントを作成
**戻り値**: 作成されたボタンイベント（無効な場合はデフォルト値）

### 3.2 システム状態関連

#### getCurrentSystemState
```cpp
SystemState getCurrentSystemState();
```
**説明**: 現在のシステム状態を取得
**戻り値**: 現在のシステム状態

### 3.3 状態遷移関連（コマンド/イベント駆動設計・標準）

#### StateTransitionManager::handleStateTransition
```cpp
static TransitionEffect handleStateTransition(
    Mode currentMode,
    const ButtonEvent& event,
    const SystemState& state
);
```
**説明**: 状態遷移を処理し、次モードと副作用コマンドリストを返す
**引数**:
- `currentMode`: 現在のモード
- `event`: ボタンイベント
- `state`: システム状態
**戻り値**: 遷移結果（次モード・コマンドリスト・エラー）

### 3.4 遷移妥当性チェック

#### TransitionValidator::validateTransition
```cpp
static bool validateTransition(
    Mode currentMode,
    const ButtonEvent& event,
    const SystemState& state,
    const char** errorMessage = nullptr
);
```
**説明**: 遷移の妥当性をチェック
**引数**:
- `currentMode`: 現在のモード
- `event`: ボタンイベント
- `state`: システム状態
- `errorMessage`: エラーメッセージの出力先（オプション）
**戻り値**: 妥当な場合はtrue、そうでなければfalse

---

## 4. 使用例

### 4.1 コマンド/イベント駆動設計の使用例

```cpp
#include "state_transition/button_event.h"
#include "state_transition/system_state.h"
#include "state_transition/state_transition.h"

void handleButtons() {
    ButtonEvent event = createButtonEventFromM5Stack();
    if (!isValidButtonEvent(event)) return;
    SystemState currentState = getCurrentSystemState();
    TransitionEffect effect = StateTransitionManager::handleStateTransition(
        currentMode, event, currentState
    );
    currentMode = effect.nextMode;
    for (const auto& cmd : effect.commands) {
        switch (cmd.type) {
            case CommandType::UpdateUI:
                drawMainDisplay();
                break;
            case CommandType::SaveSettings:
                saveSettings(settings, &eepromAdapter);
                break;
            case CommandType::ShowWarning:
                showWarning(cmd.message);
                break;
            // ... 必要に応じて拡張
        }
    }
    if (effect.errorMessage) {
        Serial.println(effect.errorMessage);
    }
}
```

### 4.2 エラーハンドリング例

```cpp
const char* errorMsg;
if (!TransitionValidator::validateTransition(currentMode, event, state, &errorMsg)) {
    Serial.print("Invalid transition: ");
    Serial.println(errorMsg);
    return;
}
```

---

## 5. エラーメッセージ

### 5.1 一般的なエラーメッセージ

- `"Invalid button event"`: 無効なボタンイベント
- `"Invalid transition"`: 無効な遷移
- `"Warning displayed"`: 警告表示中の遷移
- `"Alarm active"`: アラーム鳴動中の遷移
- `"Index out of range"`: インデックス範囲外
- `"No alarms available"`: アラームが存在しない

### 5.2 モード固有のエラーメッセージ

- `"Invalid input mode"`: 無効な入力モード
- `"Settings not available"`: 設定が利用できない
- `"Alarm management not available"`: アラーム管理が利用できない

---

## 6. 注意事項

### 6.1 スレッドセーフティ
- このAPIはスレッドセーフではありません
- 複数のスレッドから同時に呼び出す場合は適切な同期が必要です

### 6.2 メモリ管理
- TransitionResultのerrorMessageは静的文字列を指します
- 動的にメモリを確保・解放する必要はありません

### 6.3 パフォーマンス
- 状態遷移処理は軽量で高速です
- ボタンイベントの作成は毎フレーム実行可能です

---

## 7. バージョン情報

- **バージョン**: 1.0.0
- **作成日**: 2025年1月
- **互換性**: M5Stack Fire v2.7以降 