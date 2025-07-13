# 状態遷移表・遷移マトリクス詳細

## 1. ボタンイベント定義

### ButtonEvent型の設計
```cpp
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
};
```

## 2. 状態遷移表（完全版）

| 現在モード           | ボタンイベント  | 次モード                                      | 条件・制約     | 処理内容               |
| -------------------- | --------------- | --------------------------------------------- | -------------- | ---------------------- |
| MAIN_DISPLAY         | A短押し         | ABS_TIME_INPUT                                | -              | 入力状態リセット       |
| MAIN_DISPLAY         | B短押し         | REL_PLUS_TIME_INPUT                           | -              | 入力状態リセット       |
| MAIN_DISPLAY         | B長押し         | MAIN_DISPLAY                                  | -              | 何もしない             |
| MAIN_DISPLAY         | C短押し         | ALARM_MANAGEMENT                              | -              | アラーム管理画面へ     |
| MAIN_DISPLAY         | C長押し         | SETTINGS_MENU                                 | -              | 設定メニューへ         |
| ABS_TIME_INPUT       | A短押し         | ABS_TIME_INPUT                                | -              | 数字変更               |
| ABS_TIME_INPUT       | A長押し         | ABS_TIME_INPUT                                | -              | 数字を +5               |
| ABS_TIME_INPUT       | B短押し         | ABS_TIME_INPUT                                | -              | 桁送り                 |
| ABS_TIME_INPUT       | B長押し         | ABS_TIME_INPUT                                | -              | 入力値をリセット       |
| ABS_TIME_INPUT       | C短押し         | MAIN_DISPLAY                                  | 入力確定時     | アラーム追加           |
| ABS_TIME_INPUT       | C長押し         | MAIN_DISPLAY                                  | -              | キャンセル             |
| REL_PLUS_TIME_INPUT  | A短押し         | REL_PLUS_TIME_INPUT                           | -              | 数字変更               |
| REL_PLUS_TIME_INPUT  | A長押し         | REL_PLUS_TIME_INPUT                           | -              | 数字を +5               |
| REL_PLUS_TIME_INPUT  | B短押し         | REL_PLUS_TIME_INPUT                           | -              | 桁送り                 |
| REL_PLUS_TIME_INPUT  | B長押し         | REL_PLUS_TIME_INPUT                           | -              | 入力値をリセット       |
| REL_PLUS_TIME_INPUT  | C短押し         | MAIN_DISPLAY                                  | 入力確定時     | アラーム追加           |
| REL_PLUS_TIME_INPUT  | C長押し         | MAIN_DISPLAY                                  | -              | キャンセル             |
| ALARM_MANAGEMENT     | A短押し         | ALARM_MANAGEMENT                              | -              | 前の項目へ             |
| ALARM_MANAGEMENT     | B短押し         | ALARM_MANAGEMENT                              | -              | 次の項目へ             |
| ALARM_MANAGEMENT     | C短押し         | ALARM_MANAGEMENT                              | -              | 削除確認               |
| ALARM_MANAGEMENT     | C長押し         | MAIN_DISPLAY                                  | -              | メイン画面へ戻る       |
| SETTINGS_MENU        | A短押し         | SETTINGS_MENU                                 | -              | 前の項目へ             |
| SETTINGS_MENU        | B短押し         | SETTINGS_MENU                                 | -              | 次の項目へ             |
| SETTINGS_MENU        | C短押し         | SETTINGS_MENU/WARNING_COLOR_TEST/INFO_DISPLAY | 項目による     | 設定変更/画面遷移      |
| INFO_DISPLAY         | 任意ボタン      | MAIN_DISPLAY                                  | -              | メイン画面へ戻る       |
| WARNING_COLOR_TEST   | C短押し         | SETTINGS_MENU                                 | -              | 設定メニューへ戻る     |
| ALARM_ACTIVE         | 任意ボタン      | MAIN_DISPLAY                                  | -              | アラーム停止           |

## 3. 無効遷移の定義

### 制約条件
1. **警告メッセージ表示中**: 全ての状態遷移を無効化
2. **入力モード中**: Cボタンの共通処理（メイン画面への戻り）を無効化
3. **アラーム鳴動中**: 全ての状態遷移を無効化（任意ボタンで停止のみ許可）

### エッジケース
1. **連打**: デバウンス処理により無効化
2. **同時押し**: 現在は未対応（将来の拡張）
3. **境界値**: 最大アラーム数、選択インデックスの範囲外

## 4. 状態遷移関数の設計

### 純粋関数としての状態遷移ロジック
```cpp
struct TransitionResult {
  Mode nextMode;
  bool isValid;
  const char* errorMessage;
  void (*action)();  // 遷移時に実行する処理
};

TransitionResult handleStateTransition(
  Mode currentMode, 
  ButtonEvent event, 
  const SystemState& systemState
);
```

### SystemState構造体
```cpp
struct SystemState {
  int selectedIndex;           // 現在の選択インデックス
  size_t alarmCount;           // アラーム数
  bool warningDisplayed;       // 警告表示中か
  bool alarmActive;            // アラーム鳴動中か
  InputState inputState;       // 入力状態
  SettingsMenu settingsMenu;   // 設定メニュー状態
};
```

## 5. 分離方針

### 分離するファイル構成
1. **state_transition.h/cpp**: 純粋な状態遷移ロジック
2. **button_event.h/cpp**: ボタンイベントの定義と変換
3. **system_state.h/cpp**: システム状態の管理
4. **transition_validator.h/cpp**: 遷移の妥当性チェック

### 責務の分離
- **state_transition.cpp**: 状態遷移表の実装、純粋ロジック
- **button_event.cpp**: M5Stackボタン状態からButtonEventへの変換
- **system_state.cpp**: 現在のシステム状態の取得・更新
- **main.cpp**: 状態遷移関数の呼び出し、UI更新

### テスト戦略
1. **単体テスト**: 各状態遷移関数を独立してテスト
2. **統合テスト**: 実際のボタン操作シーケンスのテスト
3. **エッジケーステスト**: 無効遷移、連打、境界値のテスト

## 6. 実装順序

### Phase 1: 基盤構築
1. ButtonEvent型の定義
2. SystemState構造体の定義
3. 基本的な状態遷移関数の実装

### Phase 2: 詳細実装
1. 全状態遷移パターンの実装
2. 遷移妥当性チェックの実装
3. エラーハンドリングの実装

### Phase 3: 統合・テスト
1. main.cppのリファクタリング
2. テストケースの作成・実行
3. デバッグ・調整

## 7. 成功指標

- [ ] 状態遷移ロジックの100%分離
- [ ] 全遷移パターンのテストカバレッジ
- [ ] エッジケースの適切な処理
- [ ] main.cppの可読性向上
- [ ] テスト実行時間の短縮（native環境） 