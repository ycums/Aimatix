# 状態遷移システム API仕様書

## 1. 概要

状態遷移システムは、M5Stack Fireのボタン操作による画面遷移・モード遷移を管理する純粋関数ベースのシステムです。

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

### 2.3 TransitionResult型

```cpp
struct TransitionResult {
    bool isValid;                    // 遷移の妥当性
    Mode nextMode;                   // 次のモード
    TransitionAction action;         // 実行アクション
    const char* errorMessage;        // エラーメッセージ
    
    TransitionResult();  // デフォルトコンストラクタ
    TransitionResult(bool valid, Mode mode, TransitionAction act, const char* msg = nullptr);
};
```

#### TransitionAction列挙型
```cpp
enum TransitionAction {
    ACTION_NONE,           // アクションなし
    ACTION_RESET_INPUT,    // 入力リセット
    ACTION_ADD_ALARM,      // アラーム追加
    ACTION_DELETE_ALARM,   // アラーム削除
    ACTION_UPDATE_SETTINGS, // 設定更新
    ACTION_PLAY_ALARM,     // アラーム再生
    ACTION_STOP_ALARM,     // アラーム停止
    ACTION_SHOW_WARNING,   // 警告表示
    ACTION_CLEAR_WARNING   // 警告クリア
};
```

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

### 3.3 状態遷移関連

#### StateTransitionManager::handleStateTransition
```cpp
static TransitionResult handleStateTransition(
    Mode currentMode,
    const ButtonEvent& event,
    const SystemState& state
);
```
**説明**: 状態遷移を処理
**引数**:
- `currentMode`: 現在のモード
- `event`: ボタンイベント
- `state`: システム状態
**戻り値**: 遷移結果

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

## 4. 使用例

### 4.1 基本的な使用例

```cpp
#include "state_transition/button_event.h"
#include "state_transition/system_state.h"
#include "state_transition/state_transition.h"

void handleButtons() {
    // ボタンイベントを作成
    ButtonEvent event = createButtonEventFromM5Stack();
    
    // 無効なイベントの場合は処理をスキップ
    if (!isValidButtonEvent(event)) {
        return;
    }
    
    // 現在のシステム状態を取得
    SystemState currentState = getCurrentSystemState();
    
    // 状態遷移を処理
    TransitionResult result = StateTransitionManager::handleStateTransition(
        currentMode, event, currentState
    );
    
    // 遷移結果を処理
    if (result.isValid) {
        currentMode = result.nextMode;
        executeTransitionAction(result);
    } else {
        // エラー処理
        Serial.println(result.errorMessage);
    }
}
```

### 4.2 エラーハンドリング例

```cpp
// 遷移の妥当性を事前チェック
const char* errorMsg;
if (!TransitionValidator::validateTransition(currentMode, event, state, &errorMsg)) {
    Serial.print("Invalid transition: ");
    Serial.println(errorMsg);
    return;
}
```

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

## 7. バージョン情報

- **バージョン**: 1.0.0
- **作成日**: 2025年1月
- **互換性**: M5Stack Fire v2.7以降 