# 状態遷移システム 使用方法ガイド

## 1. 概要

このガイドでは、Aimatixプロジェクトの状態遷移システムの使用方法について説明します。

## 2. 基本的な使用方法

### 2.1 初期化

main.cppで以下のインクルードを追加してください：

```cpp
#include "state_transition/button_event.h"
#include "state_transition/system_state.h"
#include "state_transition/transition_result.h"
#include "state_transition/transition_validator.h"
#include "state_transition/state_transition.h"
```

### 2.2 ボタン処理の実装

既存のボタン処理関数を以下のように置き換えてください：

```cpp
void handleButtons() {
    // 警告メッセージ表示中はボタン処理をスキップ
    if (isWarningMessageDisplayed("")) {
        return;
    }
    
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
        if (result.nextMode != currentMode) {
            currentMode = result.nextMode;
        }
        executeTransitionAction(result);
    } else {
        // エラー処理
        if (result.errorMessage != nullptr) {
            Serial.print("Invalid transition: ");
            Serial.println(result.errorMessage);
        }
    }
}
```

### 2.3 アクション実行関数の実装

遷移結果に基づいてアクションを実行する関数を実装してください：

```cpp
void executeTransitionAction(const TransitionResult& result) {
    switch (result.action) {
        case ACTION_RESET_INPUT:
            resetInput();
            break;
        case ACTION_ADD_ALARM:
            // アラーム追加処理
            break;
        case ACTION_DELETE_ALARM:
            // アラーム削除処理
            break;
        case ACTION_UPDATE_SETTINGS:
            saveSettings();
            break;
        case ACTION_PLAY_ALARM:
            playAlarm();
            break;
        case ACTION_STOP_ALARM:
            stopAlarm();
            break;
        case ACTION_SHOW_WARNING:
            showWarningMessage(result.errorMessage);
            break;
        case ACTION_CLEAR_WARNING:
            clearWarningMessage();
            break;
        case ACTION_NONE:
        default:
            break;
    }
}
```

## 3. 状態遷移パターン

### 3.1 メイン画面からの遷移

| ボタン操作 | 遷移先 | 説明 |
|-----------|--------|------|
| A短押し | ABS_TIME_INPUT | 絶対時刻入力モード |
| B短押し | REL_PLUS_TIME_INPUT | 相対時刻加算入力モード |
| C短押し | ALARM_MANAGEMENT | アラーム管理モード |
| A長押し | - | 数字+5の処理（入力モード時） |
| B長押し | - | 入力値リセット |
| C長押し | SETTINGS_MENU | 設定メニュー |

### 3.2 入力モードからの遷移

| ボタン操作 | 遷移先 | 説明 |
|-----------|--------|------|
| A短押し | - | 数字入力（0-9） |
| B短押し | - | 数字入力（0-9） |
| C短押し | MAIN_DISPLAY | メイン画面に戻る |
| A長押し | - | 数字+5の処理 |
| B長押し | - | 入力値リセット |
| C長押し | MAIN_DISPLAY | メイン画面に戻る |

### 3.3 アラーム管理からの遷移

| ボタン操作 | 遷移先 | 説明 |
|-----------|--------|------|
| A短押し | - | 前のアラームを選択 |
| B短押し | - | 次のアラームを選択 |
| C短押し | - | 選択中のアラームを削除 |
| A長押し | - | 前のアラームを選択 |
| B長押し | - | 次のアラームを選択 |
| C長押し | MAIN_DISPLAY | メイン画面に戻る |

## 4. エラーハンドリング

### 4.1 無効な遷移の処理

```cpp
// 遷移の妥当性を事前チェック
const char* errorMsg;
if (!TransitionValidator::validateTransition(currentMode, event, state, &errorMsg)) {
    // エラーメッセージを表示
    showWarningMessage(errorMsg);
    return;
}
```

### 4.2 警告表示中の処理

```cpp
// 警告メッセージ表示中は通常のボタン処理をスキップ
if (isWarningMessageDisplayed("")) {
    // 任意のボタンで警告メッセージを消す
    if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed() || M5.BtnC.wasPressed()) {
        clearWarningMessage();
    }
    return;
}
```

### 4.3 アラーム鳴動中の処理

```cpp
// アラーム鳴動中は遷移を制限
if (currentMode == ALARM_ACTIVE) {
    // アラーム停止以外の遷移は無効
    if (result.action != ACTION_STOP_ALARM) {
        return;
    }
}
```

## 5. テストの実行

### 5.1 自動テストの実行

```bash
python scripts/run_state_transition_tests.py
```

### 5.2 手動テスト

1. **ビルドテスト**
   ```bash
   pio run
   ```

2. **動作確認**
   - 各ボタンの短押し・長押し
   - 各モード間の遷移
   - エラーケースの処理

## 6. トラブルシューティング

### 6.1 よくある問題

#### ビルドエラー
```
undefined reference to `scheduleSelectedIndex'
```
**解決方法**: main.cppでグローバル変数を定義
```cpp
int scheduleSelectedIndex = 0;
time_t lastReleaseTime = 0;
```

#### 遷移が動作しない
**確認項目**:
1. ボタンイベントが正しく作成されているか
2. システム状態が正しく取得されているか
3. 遷移結果が正しく処理されているか

#### エラーメッセージが表示されない
**確認項目**:
1. `executeTransitionAction`関数が実装されているか
2. `showWarningMessage`関数が実装されているか

### 6.2 デバッグ方法

#### シリアル出力の確認
```cpp
// 遷移結果をデバッグ出力
if (result.isValid) {
    Serial.print("Mode changed to: ");
    Serial.println(result.nextMode);
} else {
    Serial.print("Invalid transition: ");
    Serial.println(result.errorMessage);
}
```

#### ボタンイベントの確認
```cpp
// ボタンイベントをデバッグ出力
ButtonEvent event = createButtonEventFromM5Stack();
Serial.print("Button: ");
Serial.print(event.button);
Serial.print(", Action: ");
Serial.println(event.action);
```

## 7. カスタマイズ

### 7.1 新しい遷移パターンの追加

1. `state_transition.cpp`の該当ハンドラーを修正
2. `transition_validator.cpp`で妥当性チェックを追加
3. テストケースを追加

### 7.2 新しいアクションの追加

1. `transition_result.h`で新しいアクションを定義
2. `executeTransitionAction`関数で処理を追加

### 7.3 エラーメッセージのカスタマイズ

`transition_validator.cpp`でエラーメッセージを変更できます。

## 8. パフォーマンス最適化

### 8.1 メモリ使用量の最適化

- 不要な変数の削除
- 静的配列の使用
- 動的メモリ確保の最小化

### 8.2 処理速度の最適化

- 早期リターンの活用
- 不要な計算の削除
- 効率的な条件分岐

## 9. ベストプラクティス

### 9.1 コーディング規約

- 関数名は英語、動詞+名詞
- 変数名はキャメルケース
- 定数名は大文字+アンダースコア
- 日本語コメントで機能説明

### 9.2 エラーハンドリング

- 境界値チェックの実装
- 適切なエラーメッセージの表示
- デバッグ情報の出力

### 9.3 テスト

- 単体テストの作成
- 統合テストの実行
- エッジケースの確認

## 10. サポート

### 10.1 ドキュメント

- `doc/api_specification.md`: API仕様書
- `doc/implementation_completion_report.md`: 実装完了報告書
- `doc/comprehensive_implementation_plan.md`: 実装計画書

### 10.2 テストファイル

- `test/test_state_transition.cpp`: 単体テスト
- `test/test_integration.cpp`: 統合テスト

### 10.3 自動化スクリプト

- `scripts/run_state_transition_tests.py`: テスト実行スクリプト

---

**作成日**: 2025年1月  
**バージョン**: 1.0.0  
**更新日**: 2025年1月 