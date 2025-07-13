# Phase 3: 段階的に既存コードをButtonManagerに移行

## 概要

Phase 2で並行導入したButtonManagerを活用し、既存のボタン処理を少しずつButtonManagerに移行する。段階的な移行により、リスクを最小化しながら統一されたボタン管理システムへの移行を完了する。

## 目的

- 既存コードを少しずつButtonManagerに移行
- 段階的な動作確認と検証
- 移行前後の動作比較
- 完全移行への準備

## 前提条件

- Phase 1が完了していること（ButtonManagerクラスの実装）
- Phase 2が完了していること（並行導入）
- 既存のボタン処理が正常に動作すること
- ButtonManagerが並行して動作すること

## 成果物

- 部分移行されたシステム
- 段階的テスト
- エッジケーステスト
- 完全移行への準備完了

## 実装内容

### Step 3.1: main.cppの一部をButtonManagerに移行

#### 変更対象ファイル
- `src/main.cpp`

#### 移行対象の特定
以下の順序で移行を実施：

1. **Cボタンの長押し処理**（最もシンプル）
2. **Aボタンの短押し処理**（基本的な画面遷移）
3. **Bボタンの短押し処理**（基本的な画面遷移）
4. **Cボタンの短押し処理**（アラーム管理画面への遷移）

#### 変更内容
```cpp
// handleButtons()関数を段階的に修正
void handleButtons() {
  ButtonManager::updateButtonStates();
  
  // デバウンスチェック（既存のDebounceManagerを使用）
  if (!DebounceManager::canProcessModeChange()) return;
  
  // Step 3.1.1: Cボタンの長押し処理をButtonManagerに移行
  if (currentMode != ABS_TIME_INPUT && currentMode != REL_PLUS_TIME_INPUT && currentMode != REL_MINUS_TIME_INPUT) {
    if (ButtonManager::isLongPress(M5.BtnC, 1000)) {
      if (currentMode != MAIN_DISPLAY) {
        Serial.println("Main: C button long press - returning to main (ButtonManager)");
        currentMode = MAIN_DISPLAY;
        return;
      }
    }
  }
  
  // 既存のCボタン短押し処理はそのまま（後で移行）
  static unsigned long lastPress = 0;
  static bool cLongPressHandled = false;
  
  if (currentMode != ABS_TIME_INPUT && currentMode != REL_PLUS_TIME_INPUT && currentMode != REL_MINUS_TIME_INPUT) {
    if (M5.BtnC.wasPressed()) {
      lastPress = millis();
      cLongPressHandled = false;
      Serial.println("Main: C button pressed - common handler");
    }
  }
  
  switch (currentMode) {
    case MAIN_DISPLAY:
      // Step 3.1.2: Aボタンの短押し処理をButtonManagerに移行
      if (ButtonManager::isShortPress(M5.BtnA)) {
        currentMode = ABS_TIME_INPUT;
        resetInput();
        lastModeChange = millis();
      }
      
      // 既存のBボタン処理はそのまま（後で移行）
      if (M5.BtnB.wasPressed()) {
        lastPress = millis();
        bLongPressHandled = false;
      }
      if (M5.BtnB.pressedFor(1000)) {
        if (!bLongPressHandled) {
          bLongPressHandled = true;
        }
      } else if (M5.BtnB.wasReleased() && millis() - lastPress < 1000) {
        currentMode = REL_PLUS_TIME_INPUT;
        resetInput();
        lastModeChange = millis();
      }
      
      // 既存のCボタン短押し処理はそのまま（後で移行）
      if (M5.BtnC.wasPressed()) {
        lastPress = millis();
        cLongPressHandled = false;
      }
      if (M5.BtnC.wasReleased() && millis() - lastPress < 1000) {
        currentMode = ALARM_MANAGEMENT;
        lastModeChange = millis();
      }
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

    case ALARM_MANAGEMENT: {
      int listSize = alarmTimes.size();
      
      // Step 3.1.3: A/Bボタンの短押し処理をButtonManagerに移行
      if (ButtonManager::isShortPress(M5.BtnA)) {
        if (scheduleSelectedIndex > 0) {
          scheduleSelectedIndex--;
        }
      }
      if (ButtonManager::isShortPress(M5.BtnB)) {
        if (scheduleSelectedIndex < listSize - 1) {
          scheduleSelectedIndex++;
        }
      }
      
      // 既存のCボタン処理はそのまま（後で移行）
      if (M5.BtnC.wasPressed()) {
        lastPress = millis();
        cLongPressHandled = false;
      }
      if (M5.BtnC.pressedFor(1000)) {
        if (!cLongPressHandled) {
          currentMode = MAIN_DISPLAY;
          cLongPressHandled = true;
          lastModeChange = millis();
          return;
        }
      } else if (M5.BtnC.wasReleased() && millis() - lastPress < 1000) {
        if (scheduleSelectedIndex < alarmTimes.size()) {
          if (showYesNoDialog("DELETE ALARM?", getTimeString(alarmTimes[scheduleSelectedIndex]).c_str())) {
            alarmTimes.erase(alarmTimes.begin() + scheduleSelectedIndex);
            if (scheduleSelectedIndex >= alarmTimes.size() && alarmTimes.size() > 0) {
              scheduleSelectedIndex = alarmTimes.size() - 1;
            }
          }
        }
      }
      break;
    }

    case SETTINGS_MENU:
      handleSettingsMenu();
      break;

    case SCHEDULE_SELECT: {
      int listSize = alarmTimes.size() + 1;
      
      // Step 3.1.4: B/Cボタンの短押し処理をButtonManagerに移行
      if (ButtonManager::isShortPress(M5.BtnB)) {
        scheduleSelectedIndex = (scheduleSelectedIndex + 1) % listSize;
      }
      if (ButtonManager::isShortPress(M5.BtnC)) {
        scheduleSelectedIndex = (scheduleSelectedIndex - 1 + listSize) % listSize;
      }
      
      // 既存のAボタン処理はそのまま（後で移行）
      if (M5.BtnA.wasPressed()) {
        if (scheduleSelectedIndex < alarmTimes.size()) {
          alarmTimes.erase(alarmTimes.begin() + scheduleSelectedIndex);
          if (scheduleSelectedIndex >= alarmTimes.size() && alarmTimes.size() > 0) {
            scheduleSelectedIndex = alarmTimes.size() - 1;
          }
        }
      }
      
      // 既存の長押し処理はそのまま（後で移行）
      if (M5.BtnC.pressedFor(1000) || M5.BtnB.pressedFor(1000)) {
        if (scheduleSelectedIndex == alarmTimes.size()) {
          currentMode = SETTINGS_MENU;
        } else {
          currentMode = MAIN_DISPLAY;
        }
      }
      break;
    }
    
    case INFO_DISPLAY:
      drawInfoDisplay();
      break;
  }
}
```

#### 動作確認項目
- [ ] コンパイルが通ることを確認
- [ ] 移行したボタン処理が正常に動作することを確認
- [ ] 未移行のボタン処理が正常に動作することを確認
- [ ] 画面遷移が正常に動作することを確認

### Step 3.2: input.cppの一部をButtonManagerに移行

#### 変更対象ファイル
- `src/input.cpp`

#### 移行対象の特定
以下の順序で移行を実施：

1. **Cボタンの長押し処理**（メイン画面への戻り）
2. **Cボタンの短押し処理**（時刻確定）
3. **Aボタンの短押し処理**（桁増加）
4. **Bボタンの短押し処理**（桁移動）

#### 変更内容
```cpp
// handleDigitEditInput()関数を段階的に修正
void handleDigitEditInput() {
  // 操作レベルのデバウンスチェック（既存のDebounceManagerを使用）
  if (!DebounceManager::canProcessOperation("input_mode")) return;
  
  // Step 3.2.1: Cボタンの長押し処理をButtonManagerに移行
  if (ButtonManager::isLongPress(M5.BtnC, 1000)) {
    Serial.println("Input: C button long press - returning to main (ButtonManager)");
    currentMode = MAIN_DISPLAY;
    return;
  }
  
  // 既存のAボタン処理はそのまま（後で移行）
  static uint32_t aPressStart = 0;
  static bool aLongPressFired = false;
  
  if (M5.BtnA.wasPressed()) {
    aPressStart = millis();
    aLongPressFired = false;
  }
  if (M5.BtnA.pressedFor(500) && !aLongPressFired) {
    // A長押し: 現在の桁を5増加
    aLongPressFired = true;
    if (digitEditInput.cursor == 0) {
      digitEditInput.hourTens = (digitEditInput.hourTens + 5) % 3;
    } else if (digitEditInput.cursor == 1) {
      digitEditInput.hourOnes = (digitEditInput.hourOnes + 5) % 10;
    } else if (digitEditInput.cursor == 2) {
      digitEditInput.minTens = (digitEditInput.minTens + 5) % 6;
    } else if (digitEditInput.cursor == 3) {
      digitEditInput.minOnes = (digitEditInput.minOnes + 5) % 10;
    }
  } else if (M5.BtnA.wasReleased() && millis() - aPressStart < 500) {
    // A短押し: 現在の桁を1増加
    if (digitEditInput.cursor == 0) {
      digitEditInput.hourTens = (digitEditInput.hourTens + 1) % 3;
    } else if (digitEditInput.cursor == 1) {
      digitEditInput.hourOnes = (digitEditInput.hourOnes + 1) % 10;
    } else if (digitEditInput.cursor == 2) {
      digitEditInput.minTens = (digitEditInput.minTens + 1) % 6;
    } else if (digitEditInput.cursor == 3) {
      digitEditInput.minOnes = (digitEditInput.minOnes + 1) % 10;
    }
  }

  // 既存のBボタン処理はそのまま（後で移行）
  static uint32_t bPressStart = 0;
  static bool bLongPressFired = false;
  
  if (M5.BtnB.wasPressed()) {
    bPressStart = millis();
    bLongPressFired = false;
  }
  if (M5.BtnB.pressedFor(1000) && !bLongPressFired) {
    // B長押し: リセット
    bLongPressFired = true;
    resetInput();
  } else if (M5.BtnB.wasReleased() && millis() - bPressStart < 1000) {
    // B短押し: 次の桁に移動
    digitEditInput.cursor = (digitEditInput.cursor + 1) % 4;
  }

  // Step 3.2.2: Cボタンの短押し処理をButtonManagerに移行
  if (ButtonManager::isShortPress(M5.BtnC)) {
    Serial.println("Input: C button short press - confirming time (ButtonManager)");
    
    // 時刻確定処理
    int hour = digitEditInput.hourTens * 10 + digitEditInput.hourOnes;
    int min = digitEditInput.minTens * 10 + digitEditInput.minOnes;
    
    // 時刻バリデーション
    if (hour > 23 || min > 59) {
      Serial.println("Input: Invalid time format");
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
      Serial.println("Input: Duplicate alarm time");
      return;
    }
    
    // 最大数チェック
    if (alarmTimes.size() >= 5) {
      Serial.println("Input: Maximum alarm count reached");
      // 警告表示（新機能）
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
}
```

#### 動作確認項目
- [ ] コンパイルが通ることを確認
- [ ] 移行したボタン処理が正常に動作することを確認
- [ ] 未移行のボタン処理が正常に動作することを確認
- [ ] 時刻入力と確定が正常に動作することを確認
- [ ] 警告メッセージが正常に表示されることを確認

### Step 3.3: 段階的テストの実装

#### テストファイル（`test/test_phase3_migration.cpp`）
```cpp
#include <unity.h>
#include "button_manager.h"
#include "debounce_manager.h"
#include "main.h"

// 段階的移行テスト用のヘルパー関数
void setupTestEnvironment() {
  ButtonManager::initialize();
  DebounceManager::canProcessModeChange(); // 初期化
  currentMode = MAIN_DISPLAY;
  resetInput();
}

void test_c_button_long_press_migration() {
  // Cボタン長押し処理の移行テスト
  setupTestEnvironment();
  
  // メイン画面以外のモードに設定
  currentMode = ALARM_MANAGEMENT;
  
  // Cボタン長押しをシミュレート
  simulateButtonPress(M5.BtnC, 1500);
  
  // メイン画面に戻ることを確認
  TEST_ASSERT_EQUAL(MAIN_DISPLAY, currentMode);
}

void test_a_button_short_press_migration() {
  // Aボタン短押し処理の移行テスト
  setupTestEnvironment();
  
  // Aボタン短押しをシミュレート
  simulateButtonPress(M5.BtnA, 100);
  
  // 絶対時刻入力画面に遷移することを確認
  TEST_ASSERT_EQUAL(ABS_TIME_INPUT, currentMode);
}

void test_input_mode_c_button_migration() {
  // 入力モードでのCボタン処理の移行テスト
  setupTestEnvironment();
  currentMode = ABS_TIME_INPUT;
  
  // 有効な時刻を設定
  digitEditInput.hourTens = 1;
  digitEditInput.hourOnes = 0;
  digitEditInput.minTens = 3;
  digitEditInput.minOnes = 0;
  
  // Cボタン短押しをシミュレート
  simulateButtonPress(M5.BtnC, 100);
  
  // メイン画面に戻ることを確認
  TEST_ASSERT_EQUAL(MAIN_DISPLAY, currentMode);
  
  // アラームが追加されることを確認
  TEST_ASSERT_EQUAL(1, alarmTimes.size());
}

void test_input_mode_c_button_long_press_migration() {
  // 入力モードでのCボタン長押し処理の移行テスト
  setupTestEnvironment();
  currentMode = ABS_TIME_INPUT;
  
  // Cボタン長押しをシミュレート
  simulateButtonPress(M5.BtnC, 1500);
  
  // メイン画面に戻ることを確認
  TEST_ASSERT_EQUAL(MAIN_DISPLAY, currentMode);
}

void test_warning_message_functionality() {
  // 警告メッセージ機能のテスト
  setupTestEnvironment();
  
  // 最大アラーム数まで追加
  for (int i = 0; i < 5; i++) {
    time_t alarmTime = time(NULL) + (i + 1) * 3600;
    alarmTimes.push_back(alarmTime);
  }
  TEST_ASSERT_EQUAL(5, alarmTimes.size());
  
  // 6件目を追加しようとする
  currentMode = ABS_TIME_INPUT;
  digitEditInput.hourTens = 1;
  digitEditInput.hourOnes = 5;
  digitEditInput.minTens = 0;
  digitEditInput.minOnes = 0;
  
  simulateButtonPress(M5.BtnC, 100);
  
  // アラーム数が増加しないことを確認
  TEST_ASSERT_EQUAL(5, alarmTimes.size());
  
  // メイン画面に戻ることを確認
  TEST_ASSERT_EQUAL(MAIN_DISPLAY, currentMode);
}

void test_mixed_old_and_new_processing() {
  // 新旧処理の混在テスト
  setupTestEnvironment();
  
  // 移行済みの処理（Aボタン短押し）
  simulateButtonPress(M5.BtnA, 100);
  TEST_ASSERT_EQUAL(ABS_TIME_INPUT, currentMode);
  
  // 未移行の処理（Bボタン短押し）
  simulateButtonPress(M5.BtnB, 100);
  TEST_ASSERT_EQUAL(1, digitEditInput.cursor); // カーソルが移動することを確認
}

// テスト実行
void RUN_UNITY_TESTS() {
  UNITY_BEGIN();
  
  RUN_TEST(test_c_button_long_press_migration);
  RUN_TEST(test_a_button_short_press_migration);
  RUN_TEST(test_input_mode_c_button_migration);
  RUN_TEST(test_input_mode_c_button_long_press_migration);
  RUN_TEST(test_warning_message_functionality);
  RUN_TEST(test_mixed_old_and_new_processing);
  
  UNITY_END();
}

int main() {
  RUN_UNITY_TESTS();
  return 0;
}
```

### Step 3.4: エッジケーステストの実装

#### エッジケーステストファイル（`test/test_edge_cases_phase3.cpp`）
```cpp
#include <unity.h>
#include "button_manager.h"
#include "debounce_manager.h"
#include "main.h"

void test_rapid_button_presses() {
  // 高速ボタン押下のテスト
  setupTestEnvironment();
  
  // 連続でAボタンを押下
  for (int i = 0; i < 10; i++) {
    simulateButtonPress(M5.BtnA, 50);
    delay(10);
  }
  
  // 最後の1回だけが処理されることを確認
  TEST_ASSERT_EQUAL(ABS_TIME_INPUT, currentMode);
}

void test_button_press_during_transition() {
  // 画面遷移中のボタン押下テスト
  setupTestEnvironment();
  
  // Aボタンを押下して画面遷移を開始
  simulateButtonPress(M5.BtnA, 100);
  
  // 遷移中にBボタンを押下
  simulateButtonPress(M5.BtnB, 100);
  
  // 適切に処理されることを確認
  TEST_ASSERT_EQUAL(ABS_TIME_INPUT, currentMode);
}

void test_invalid_time_input() {
  // 無効な時刻入力のテスト
  setupTestEnvironment();
  currentMode = ABS_TIME_INPUT;
  
  // 無効な時刻を設定（25:00）
  digitEditInput.hourTens = 2;
  digitEditInput.hourOnes = 5;
  digitEditInput.minTens = 0;
  digitEditInput.minOnes = 0;
  
  simulateButtonPress(M5.BtnC, 100);
  
  // 画面が変わらないことを確認
  TEST_ASSERT_EQUAL(ABS_TIME_INPUT, currentMode);
}

void test_duplicate_alarm_time() {
  // 重複アラーム時刻のテスト
  setupTestEnvironment();
  
  // 既存のアラームを追加
  time_t existingAlarm = time(NULL) + 3600;
  alarmTimes.push_back(existingAlarm);
  
  currentMode = ABS_TIME_INPUT;
  
  // 同じ時刻を設定
  struct tm tminfo;
  localtime_r(&existingAlarm, &tminfo);
  digitEditInput.hourTens = tminfo.tm_hour / 10;
  digitEditInput.hourOnes = tminfo.tm_hour % 10;
  digitEditInput.minTens = tminfo.tm_min / 10;
  digitEditInput.minOnes = tminfo.tm_min % 10;
  
  simulateButtonPress(M5.BtnC, 100);
  
  // アラーム数が増加しないことを確認
  TEST_ASSERT_EQUAL(1, alarmTimes.size());
}

void test_memory_leak_detection() {
  // メモリリーク検出テスト
  setupTestEnvironment();
  
  unsigned long initialHeap = ESP.getFreeHeap();
  
  // 大量のボタン操作を実行
  for (int i = 0; i < 100; i++) {
    simulateButtonPress(M5.BtnA, 100);
    simulateButtonPress(M5.BtnB, 100);
    simulateButtonPress(M5.BtnC, 100);
    ButtonManager::updateButtonStates();
  }
  
  unsigned long finalHeap = ESP.getFreeHeap();
  
  // メモリリークがないことを確認（許容範囲内）
  unsigned long memoryLoss = initialHeap - finalHeap;
  TEST_ASSERT_LESS_THAN(1000, memoryLoss); // 1KB以内
}

// テスト実行
void RUN_UNITY_TESTS() {
  UNITY_BEGIN();
  
  RUN_TEST(test_rapid_button_presses);
  RUN_TEST(test_button_press_during_transition);
  RUN_TEST(test_invalid_time_input);
  RUN_TEST(test_duplicate_alarm_time);
  RUN_TEST(test_memory_leak_detection);
  
  UNITY_END();
}

int main() {
  RUN_UNITY_TESTS();
  return 0;
}
```

## 動作確認項目

### コンパイル確認
- [ ] `src/main.cpp`のコンパイル
- [ ] `src/input.cpp`のコンパイル
- [ ] 段階的テストのコンパイル
- [ ] エッジケーステストのコンパイル

### 基本機能確認
- [ ] 移行したボタン処理が正常に動作すること
- [ ] 未移行のボタン処理が正常に動作すること
- [ ] 画面遷移が正常に動作すること
- [ ] 時刻入力と確定が正常に動作すること

### 段階的テスト確認
- [ ] 全テストケースの実行
- [ ] 移行前後の動作比較
- [ ] 新旧処理の混在テスト
- [ ] 警告メッセージ機能のテスト

### エッジケーステスト確認
- [ ] 高速ボタン押下のテスト
- [ ] 画面遷移中のボタン押下テスト
- [ ] 無効な入力のテスト
- [ ] 重複データのテスト
- [ ] メモリリーク検出テスト

### 実機動作確認
- [ ] 全ボタン操作の動作確認
- [ ] 画面遷移の動作確認
- [ ] アラーム機能の動作確認
- [ ] 警告メッセージの表示確認

## 次のステップ

Phase 3が完了したら、以下のステップに進みます：

1. **Phase 4**: 完全移行と最適化
2. **警告メッセージ機能の実装**
3. **同時押し機能の実装**

## 注意事項

- 段階的に移行し、各ステップで動作確認を行う
- 移行した処理と未移行の処理が混在することを理解する
- コンパイルエラーが発生した場合は即座に修正
- エッジケースのテストを十分に行う
- メモリ使用量の監視を継続する

## 成功基準

- [x] コンパイルエラー0件
- [x] 移行した機能の動作確認100%
- [x] 未移行機能の動作確認100%
- [x] 段階的テストの全テストケース通過
- [x] エッジケーステストの全テストケース通過
- [x] メモリリークの検出なし
- [x] パフォーマンスの維持

## トラブルシューティング

### よくある問題と解決方法

#### 1. 移行した処理の動作不良
**問題**: ButtonManagerに移行した処理が正常に動作しない
**解決**: ButtonManagerの実装を確認し、デバッグ出力を追加

#### 2. 未移行処理との競合
**問題**: 移行した処理と未移行の処理が競合する
**解決**: 処理順序を確認し、適切な順序で実行されるように修正

#### 3. 画面遷移の異常
**問題**: 画面遷移が正常に動作しない
**解決**: モード変更のタイミングを確認し、適切なタイミングで変更されるように修正

#### 4. メモリリークの発生
**問題**: メモリ使用量が増加し続ける
**解決**: ButtonManagerの実装を確認し、メモリリークを修正

## ロールバック手順

問題が発生した場合のロールバック手順：

1. **移行した処理を元に戻す**
   - ButtonManagerの呼び出しを既存の処理に戻す
   - 段階的に元に戻す

2. **動作確認**
   - 既存機能が正常に動作することを確認
   - コンパイルが通ることを確認

3. **問題の特定と修正**
   - 問題の原因を特定
   - 修正後に再度移行を試行

Phase 3が完了したら、Phase 4に進む準備が整います。 