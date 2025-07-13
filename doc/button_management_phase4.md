# Phase 4: 完全移行と最適化

## 概要

Phase 3で段階的に移行したButtonManagerを完全に導入し、残りのボタン処理をすべて移行する。さらに、警告メッセージ機能と同時押し機能を実装し、統一されたボタン管理システムを完成させる。

## 目的

- 残りのボタン処理をすべてButtonManagerに移行
- 警告メッセージ機能の実装
- 同時押し機能の実装
- システムの最適化とパフォーマンス向上

## 前提条件

- Phase 1が完了していること（ButtonManagerクラスの実装）
- Phase 2が完了していること（並行導入）
- Phase 3が完了していること（段階的移行）
- 既存のボタン処理が正常に動作すること

## 成果物

- 完全移行されたシステム
- 警告メッセージ機能
- 同時押し機能
- 最適化されたパフォーマンス

## 実装内容

### Step 4.1: 残りのボタン処理を完全移行

#### 変更対象ファイル
- `src/main.cpp`
- `src/input.cpp`

#### 完全移行後のmain.cpp
```cpp
// handleButtons()関数を完全移行
void handleButtons() {
  ButtonManager::updateButtonStates();
  
  // デバウンスチェック（既存のDebounceManagerを使用）
  if (!DebounceManager::canProcessModeChange()) return;
  
  // 共通のCボタン長押し処理（ButtonManager）
  if (currentMode != ABS_TIME_INPUT && currentMode != REL_PLUS_TIME_INPUT && currentMode != REL_MINUS_TIME_INPUT) {
    if (ButtonManager::isLongPress(M5.BtnC, 1000)) {
      if (currentMode != MAIN_DISPLAY) {
        Serial.println("Main: C button long press - returning to main (ButtonManager)");
        currentMode = MAIN_DISPLAY;
        return;
      }
    }
  }
  
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

    case SCHEDULE_SELECT:
      handleScheduleSelectButtons();
      break;
    
    case INFO_DISPLAY:
      drawInfoDisplay();
      break;
  }
}

// メイン画面のボタン処理（完全移行）
void handleMainDisplayButtons() {
  // Aボタン短押し: 絶対時刻入力画面へ
  if (ButtonManager::isShortPress(M5.BtnA)) {
    currentMode = ABS_TIME_INPUT;
    resetInput();
    lastModeChange = millis();
  }
  
  // Bボタン短押し: 相対時刻入力画面へ
  if (ButtonManager::isShortPress(M5.BtnB)) {
    currentMode = REL_PLUS_TIME_INPUT;
    resetInput();
    lastModeChange = millis();
  }
  
  // Bボタン長押し: 相対時刻減算入力画面へ
  if (ButtonManager::isLongPress(M5.BtnB, 1000)) {
    currentMode = REL_MINUS_TIME_INPUT;
    resetInput();
    lastModeChange = millis();
  }
  
  // Cボタン短押し: アラーム管理画面へ
  if (ButtonManager::isShortPress(M5.BtnC)) {
    currentMode = ALARM_MANAGEMENT;
    lastModeChange = millis();
  }
  
  // Cボタン長押し: 設定メニューへ
  if (ButtonManager::isLongPress(M5.BtnC, 1000)) {
    currentMode = SETTINGS_MENU;
    lastModeChange = millis();
  }
}

// 入力モードのボタン処理（完全移行）
void handleInputModeButtons() {
  // 操作レベルのデバウンスチェック
  if (!DebounceManager::canProcessOperation("input_mode")) return;
  
  // Aボタン短押し: 現在の桁を1増加
  if (ButtonManager::isShortPress(M5.BtnA)) {
    incrementCurrentDigit(1);
  }
  
  // Aボタン長押し: 現在の桁を5増加
  if (ButtonManager::isLongPress(M5.BtnA, 500)) {
    incrementCurrentDigit(5);
  }
  
  // Bボタン短押し: 次の桁に移動
  if (ButtonManager::isShortPress(M5.BtnB)) {
    digitEditInput.cursor = (digitEditInput.cursor + 1) % 4;
  }
  
  // Bボタン長押し: リセット
  if (ButtonManager::isLongPress(M5.BtnB, 1000)) {
    resetInput();
  }
  
  // Cボタン短押し: 時刻確定
  if (ButtonManager::isShortPress(M5.BtnC)) {
    confirmTimeInput();
  }
  
  // Cボタン長押し: メイン画面に戻る
  if (ButtonManager::isLongPress(M5.BtnC, 1000)) {
    currentMode = MAIN_DISPLAY;
  }
}

// アラーム管理画面のボタン処理（完全移行）
void handleAlarmManagementButtons() {
  int listSize = alarmTimes.size();
  
  // Aボタン短押し: 前のアラームを選択
  if (ButtonManager::isShortPress(M5.BtnA)) {
    if (scheduleSelectedIndex > 0) {
      scheduleSelectedIndex--;
    }
  }
  
  // Bボタン短押し: 次のアラームを選択
  if (ButtonManager::isShortPress(M5.BtnB)) {
    if (scheduleSelectedIndex < listSize - 1) {
      scheduleSelectedIndex++;
    }
  }
  
  // Cボタン短押し: アラーム削除確認
  if (ButtonManager::isShortPress(M5.BtnC)) {
    if (scheduleSelectedIndex < alarmTimes.size()) {
      if (showYesNoDialog("DELETE ALARM?", getTimeString(alarmTimes[scheduleSelectedIndex]).c_str())) {
        alarmTimes.erase(alarmTimes.begin() + scheduleSelectedIndex);
        if (scheduleSelectedIndex >= alarmTimes.size() && alarmTimes.size() > 0) {
          scheduleSelectedIndex = alarmTimes.size() - 1;
        }
      }
    }
  }
  
  // Cボタン長押し: メイン画面に戻る
  if (ButtonManager::isLongPress(M5.BtnC, 1000)) {
    currentMode = MAIN_DISPLAY;
    lastModeChange = millis();
  }
}

// スケジュール選択画面のボタン処理（完全移行）
void handleScheduleSelectButtons() {
  int listSize = alarmTimes.size() + 1;
  
  // Aボタン短押し: 選択したアラームを削除
  if (ButtonManager::isShortPress(M5.BtnA)) {
    if (scheduleSelectedIndex < alarmTimes.size()) {
      alarmTimes.erase(alarmTimes.begin() + scheduleSelectedIndex);
      if (scheduleSelectedIndex >= alarmTimes.size() && alarmTimes.size() > 0) {
        scheduleSelectedIndex = alarmTimes.size() - 1;
      }
    }
  }
  
  // Bボタン短押し: 次の項目を選択
  if (ButtonManager::isShortPress(M5.BtnB)) {
    scheduleSelectedIndex = (scheduleSelectedIndex + 1) % listSize;
  }
  
  // Cボタン短押し: 前の項目を選択
  if (ButtonManager::isShortPress(M5.BtnC)) {
    scheduleSelectedIndex = (scheduleSelectedIndex - 1 + listSize) % listSize;
  }
  
  // B/Cボタン長押し: 画面遷移
  if (ButtonManager::isLongPress(M5.BtnB, 1000) || ButtonManager::isLongPress(M5.BtnC, 1000)) {
    if (scheduleSelectedIndex == alarmTimes.size()) {
      currentMode = SETTINGS_MENU;
    } else {
      currentMode = MAIN_DISPLAY;
    }
  }
}

// 設定メニューのボタン処理（完全移行）
void handleSettingsMenuButtons() {
  // 既存の設定メニュー処理をButtonManagerに移行
  // （既存のhandleSettingsMenu()関数の内容を移行）
}
```

#### 完全移行後のinput.cpp
```cpp
// handleDigitEditInput()関数を削除（handleInputModeButtons()に統合）

// ヘルパー関数の追加
void incrementCurrentDigit(int increment) {
  if (digitEditInput.cursor == 0) {
    digitEditInput.hourTens = (digitEditInput.hourTens + increment) % 3;
  } else if (digitEditInput.cursor == 1) {
    digitEditInput.hourOnes = (digitEditInput.hourOnes + increment) % 10;
  } else if (digitEditInput.cursor == 2) {
    digitEditInput.minTens = (digitEditInput.minTens + increment) % 6;
  } else if (digitEditInput.cursor == 3) {
    digitEditInput.minOnes = (digitEditInput.minOnes + increment) % 10;
  }
}

void confirmTimeInput() {
  Serial.println("Input: C button short press - confirming time (ButtonManager)");
  
  // 時刻確定処理
  int hour = digitEditInput.hourTens * 10 + digitEditInput.hourOnes;
  int min = digitEditInput.minTens * 10 + digitEditInput.minOnes;
  
  // 時刻バリデーション
  if (hour > 23 || min > 59) {
    Serial.println("Input: Invalid time format");
    showWarningMessage("無効な時刻です");
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
  } else if (currentMode == REL_MINUS_TIME_INPUT) {
    alarmTime = now - (hour * 3600) - (min * 60);
  }
  
  // 重複チェック
  if (std::find(alarmTimes.begin(), alarmTimes.end(), alarmTime) != alarmTimes.end()) {
    Serial.println("Input: Duplicate alarm time");
    showWarningMessage("同じ時刻のアラームが既に存在します");
    return;
  }
  
  // 最大数チェック
  if (alarmTimes.size() >= 5) {
    Serial.println("Input: Maximum alarm count reached");
    showWarningMessage("アラームは最大5件までです");
    return;
  }
  
  // アラーム追加
  alarmTimes.push_back(alarmTime);
  std::sort(alarmTimes.begin(), alarmTimes.end());
  
  Serial.printf("Input: Alarm added - %02d:%02d\n", hour, min);
  
  // 入力状態リセット
  resetInput();
  
  // メイン画面に戻る
  currentMode = MAIN_DISPLAY;
}
```

### Step 4.2: 警告メッセージ機能の実装

#### 警告メッセージ機能の実装
```cpp
// ui.cppに追加
void showWarningMessage(const char* message) {
  // 現在の画面を保存
  drawCurrentScreen();
  
  // 警告メッセージを表示
  sprite.setTextFont(2);
  sprite.setTextColor(FLASH_ORANGE, TFT_BLACK);
  sprite.setTextDatum(MC_DATUM);
  
  // 画面下部に警告メッセージを表示
  int yPosition = SCREEN_HEIGHT - 30;
  sprite.drawString(message, SCREEN_WIDTH/2, yPosition);
  sprite.pushSprite(0, 0);
  
  // バイブレーション1回
  M5.setVibration(100);
  
  // 2秒間表示
  delay(2000);
  
  // 画面を再描画
  drawCurrentScreen();
}

void showSuccessMessage(const char* message) {
  // 成功メッセージを表示（緑色）
  sprite.setTextFont(2);
  sprite.setTextColor(TFT_GREEN, TFT_BLACK);
  sprite.setTextDatum(MC_DATUM);
  
  int yPosition = SCREEN_HEIGHT - 30;
  sprite.drawString(message, SCREEN_WIDTH/2, yPosition);
  sprite.pushSprite(0, 0);
  
  // 短いバイブレーション
  M5.setVibration(50);
  
  // 1秒間表示
  delay(1000);
  
  // 画面を再描画
  drawCurrentScreen();
}

void showInfoMessage(const char* message) {
  // 情報メッセージを表示（アンバー色）
  sprite.setTextFont(2);
  sprite.setTextColor(AMBER_COLOR, TFT_BLACK);
  sprite.setTextDatum(MC_DATUM);
  
  int yPosition = SCREEN_HEIGHT - 30;
  sprite.drawString(message, SCREEN_WIDTH/2, yPosition);
  sprite.pushSprite(0, 0);
  
  // 1.5秒間表示
  delay(1500);
  
  // 画面を再描画
  drawCurrentScreen();
}
```

### Step 4.3: 同時押し機能の実装

#### ButtonManagerの拡張
```cpp
// button_manager.hに追加
class ButtonManager {
public:
  // 同時押し判定
  static bool isSimultaneousPress(Button& button1, Button& button2, unsigned long tolerance = 100);
  static bool isAllButtonsPressed();
  static bool isCombinationPress(const std::vector<Button*>& buttons);
  
  // 連続押し判定（将来の拡張）
  static bool isDoublePress(Button& button, unsigned long interval = 300);
  static bool isTriplePress(Button& button, unsigned long interval = 300);
  
private:
  static std::vector<Button*> getPressedButtons();
  static bool isButtonInCombination(Button& button, const std::vector<Button*>& combination);
};

// button_manager.cppに実装
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

bool ButtonManager::isCombinationPress(const std::vector<Button*>& buttons) {
  if (buttons.empty()) return false;
  
  // 全てのボタンが押されているかチェック
  for (Button* button : buttons) {
    if (!button->isPressed()) return false;
  }
  
  // 押下開始時刻の差が許容範囲内かチェック
  unsigned long firstPressTime = buttonStates[buttons[0]].pressStartTime;
  for (Button* button : buttons) {
    ButtonState& state = buttonStates[button];
    unsigned long timeDiff = abs((long)(state.pressStartTime - firstPressTime));
    if (timeDiff > 100) return false; // 100ms以内
  }
  
  return true;
}

std::vector<Button*> ButtonManager::getPressedButtons() {
  std::vector<Button*> pressedButtons;
  
  if (M5.BtnA.isPressed()) pressedButtons.push_back(&M5.BtnA);
  if (M5.BtnB.isPressed()) pressedButtons.push_back(&M5.BtnB);
  if (M5.BtnC.isPressed()) pressedButtons.push_back(&M5.BtnC);
  
  return pressedButtons;
}
```

#### 同時押し機能の活用例
```cpp
// main.cppに追加
void handleSpecialCombinations() {
  // AB同時押し: 全アラーム削除確認
  if (ButtonManager::isSimultaneousPress(M5.BtnA, M5.BtnB, 100)) {
    if (showYesNoDialog("DELETE ALL ALARMS?", "Are you sure?")) {
      alarmTimes.clear();
      showSuccessMessage("全アラームを削除しました");
    }
  }
  
  // ABC同時押し: システムリセット確認
  if (ButtonManager::isAllButtonsPressed()) {
    if (showYesNoDialog("SYSTEM RESET?", "All data will be lost")) {
      // システムリセット処理
      resetSystem();
      showSuccessMessage("システムをリセットしました");
    }
  }
  
  // AC同時押し: デバッグ情報表示
  if (ButtonManager::isSimultaneousPress(M5.BtnA, M5.BtnC, 100)) {
    showDebugInfo();
  }
}

// handleButtons()関数に追加
void handleButtons() {
  ButtonManager::updateButtonStates();
  
  // デバウンスチェック
  if (!DebounceManager::canProcessModeChange()) return;
  
  // 特殊な組み合わせ処理
  handleSpecialCombinations();
  
  // 既存の処理...
}
```

### Step 4.4: システム最適化

#### パフォーマンス最適化
```cpp
// ButtonManagerの最適化
class ButtonManager {
private:
  // キャッシュ機能の追加
  static std::map<Button*, bool> lastButtonStates;
  static unsigned long lastOptimizationTime;
  
public:
  // 最適化された状態更新
  static void updateButtonStatesOptimized() {
    unsigned long currentTime = millis();
    
    // 定期的な最適化処理
    if (currentTime - lastOptimizationTime > 1000) {
      optimizeMemoryUsage();
      lastOptimizationTime = currentTime;
    }
    
    // 状態変化がある場合のみ更新
    updateButtonStateIfChanged(M5.BtnA, currentTime);
    updateButtonStateIfChanged(M5.BtnB, currentTime);
    updateButtonStateIfChanged(M5.BtnC, currentTime);
  }
  
private:
  static void updateButtonStateIfChanged(Button& button, unsigned long currentTime) {
    bool currentState = button.isPressed();
    bool& lastState = lastButtonStates[&button];
    
    if (currentState != lastState) {
      updateButtonState(button, currentTime);
      lastState = currentState;
    }
  }
  
  static void optimizeMemoryUsage() {
    // 不要な状態データのクリーンアップ
    for (auto it = buttonStates.begin(); it != buttonStates.end();) {
      if (currentTime - it->second.lastChangeTime > 5000) {
        it = buttonStates.erase(it);
      } else {
        ++it;
      }
    }
  }
};
```

#### メモリ使用量の最適化
```cpp
// メモリ使用量監視機能
class MemoryMonitor {
public:
  static void printMemoryUsage() {
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("Largest free block: %d bytes\n", ESP.getMaxAllocHeap());
    Serial.printf("ButtonManager memory: %d bytes\n", getButtonManagerMemoryUsage());
  }
  
  static int getButtonManagerMemoryUsage() {
    return buttonStates.size() * sizeof(ButtonState) + 
           lastButtonStates.size() * sizeof(bool);
  }
  
  static bool checkMemoryHealth() {
    int freeHeap = ESP.getFreeHeap();
    return freeHeap > 10000; // 10KB以上
  }
};
```

## 動作確認項目

### コンパイル確認
- [ ] 完全移行後のコンパイル
- [ ] 警告メッセージ機能のコンパイル
- [ ] 同時押し機能のコンパイル
- [ ] 最適化機能のコンパイル

### 基本機能確認
- [ ] 全ボタン処理が正常に動作すること
- [ ] 画面遷移が正常に動作すること
- [ ] アラーム機能が正常に動作すること
- [ ] 設定機能が正常に動作すること

### 新機能確認
- [ ] 警告メッセージが正常に表示されること
- [ ] 同時押し機能が正常に動作すること
- [ ] 特殊な組み合わせが正常に動作すること
- [ ] メモリ使用量が最適化されていること

### パフォーマンス確認
- [ ] ボタン応答性が向上していること
- [ ] メモリ使用量が削減されていること
- [ ] 処理速度が向上していること
- [ ] 安定性が向上していること

## 次のステップ

Phase 4が完了したら、以下のステップに進みます：

1. **最終テスト**: 全機能の統合テスト
2. **ドキュメント更新**: 実装完了の記録
3. **将来拡張の準備**: さらなる機能拡張への準備

## 注意事項

- 完全移行により既存のボタン処理がすべて変更される
- 新機能の実装によりシステムが複雑になる
- パフォーマンス最適化により動作が変わる可能性がある
- 十分なテストが必要

## 成功基準

- [ ] コンパイルエラー0件
- [ ] 全機能の動作確認100%
- [ ] 新機能の動作確認100%
- [ ] パフォーマンスの向上確認
- [ ] メモリ使用量の最適化確認
- [ ] 安定性の向上確認

## トラブルシューティング

### よくある問題と解決方法

#### 1. 完全移行後の動作不良
**問題**: 移行後に一部の機能が動作しない
**解決**: 段階的に移行を確認し、問題箇所を特定

#### 2. 新機能の動作不良
**問題**: 警告メッセージや同時押し機能が動作しない
**解決**: 新機能の実装を確認し、デバッグ出力を追加

#### 3. パフォーマンスの低下
**問題**: 最適化後にパフォーマンスが低下
**解決**: 最適化処理を見直し、適切な最適化を実施

#### 4. メモリ使用量の増加
**問題**: 新機能によりメモリ使用量が増加
**解決**: メモリ使用量を監視し、不要な処理を削除

## ロールバック手順

問題が発生した場合のロールバック手順：

1. **新機能の無効化**
   - 警告メッセージ機能を無効化
   - 同時押し機能を無効化

2. **部分的なロールバック**
   - 問題のある部分のみを元に戻す
   - 段階的にロールバック

3. **動作確認**
   - 基本機能が正常に動作することを確認
   - コンパイルが通ることを確認

Phase 4が完了したら、統一されたボタン管理システムが完成します。 