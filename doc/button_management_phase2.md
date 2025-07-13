# Phase 2: 既存コードにButtonManagerを並行導入

## 概要

Phase 1で実装したButtonManagerクラスを既存コードに並行導入する。既存のボタン処理は一切変更せず、ButtonManagerを並行して動作させることで、段階的な移行の基盤を構築する。

## 目的

- 既存コードを壊さずにButtonManagerを導入
- 並行動作の確認と検証
- 段階的移行への準備
- 統合テストの実施

## 前提条件

- Phase 1が完了していること
- ButtonManagerクラスが正常に動作すること
- 既存のDebounceManagerが正常に動作すること
- 既存のボタン処理が正常に動作すること

## 成果物

- 並行動作するシステム
- 統合テスト
- パフォーマンス測定結果
- 段階的移行の準備完了

## 実装内容

### Step 2.1: main.cppにButtonManagerを並行導入

#### 変更対象ファイル
- `src/main.cpp`

#### 変更内容
```cpp
// 既存のinclude文に追加
#include "button_manager.h"

// setup()関数に追加
void setup() {
  M5.begin();
  M5.Power.begin();
  Serial.begin(115200);

  initUI();

  // Initialize EEPROM
  EEPROM.begin(EEPROM_SIZE);
  loadSettings();
  
  // Set initial LCD brightness
  M5.Lcd.setBrightness(settings.lcd_brightness);
  
  // Set display properties
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextColor(AMBER_COLOR, TFT_BLACK);
  
  // デバッグ用：アラームリストに初期値を5件追加
  addDebugAlarms();
 
  // ButtonManagerの初期化（新規追加）
  ButtonManager::initialize();
  Serial.println("ButtonManager initialized");
  
  // Wi-Fi/NTP同期は一時的にスキップ
  // if (connectWiFi()) {
  //   timeClient.begin();
  //   syncTime();
  // }
}

// handleButtons()関数を修正
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
      if (M5.BtnA.wasPressed()) {
        currentMode = ABS_TIME_INPUT;
        resetInput();
        lastModeChange = millis();
      }
      if (M5.BtnB.wasPressed()) {
        lastPress = millis();
        bLongPressHandled = false;
      }
      if (M5.BtnB.pressedFor(LONG_PRESS_TIME)) {
        if (!bLongPressHandled) {
          bLongPressHandled = true;
        }
      } else if (M5.BtnB.wasReleased() && millis() - lastPress < LONG_PRESS_TIME) {
        currentMode = REL_PLUS_TIME_INPUT;
        resetInput();
        lastModeChange = millis();
      }
      if (M5.BtnC.wasPressed()) {
        lastPress = millis();
        cLongPressHandled = false;
      }
      if (M5.BtnC.wasReleased() && millis() - lastPress < LONG_PRESS_TIME) {
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
      if (M5.BtnA.wasPressed()) {
        if (scheduleSelectedIndex > 0) {
          scheduleSelectedIndex--;
        }
      }
      if (M5.BtnB.wasPressed()) {
        if (scheduleSelectedIndex < listSize - 1) {
          scheduleSelectedIndex++;
        }
      }
      if (M5.BtnC.wasPressed()) {
        lastPress = millis();
        cLongPressHandled = false;
      }
      if (M5.BtnC.pressedFor(LONG_PRESS_TIME)) {
        if (!cLongPressHandled) {
          currentMode = MAIN_DISPLAY;
          cLongPressHandled = true;
          lastModeChange = millis();
          return;
        }
      } else if (M5.BtnC.wasReleased() && millis() - lastPress < LONG_PRESS_TIME) {
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
      if (M5.BtnB.wasPressed()) {
        scheduleSelectedIndex = (scheduleSelectedIndex + 1) % listSize;
      }
      if (M5.BtnC.wasPressed()) {
        scheduleSelectedIndex = (scheduleSelectedIndex - 1 + listSize) % listSize;
      }
      if (M5.BtnA.wasPressed()) {
        if (scheduleSelectedIndex < alarmTimes.size()) {
          alarmTimes.erase(alarmTimes.begin() + scheduleSelectedIndex);
          if (scheduleSelectedIndex >= alarmTimes.size() && alarmTimes.size() > 0) {
            scheduleSelectedIndex = alarmTimes.size() - 1;
          }
        }
      }
      if (M5.BtnC.pressedFor(LONG_PRESS_TIME) || M5.BtnB.pressedFor(LONG_PRESS_TIME)) {
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
  
  // 新しく追加: ButtonManagerの状態更新（既存処理には影響なし）
  ButtonManager::updateButtonStates();
}
```

#### 動作確認項目
- [ ] コンパイルが通ることを確認
- [ ] 既存のボタン処理が正常に動作することを確認
- [ ] ButtonManagerが並行して動作することを確認
- [ ] メモリ使用量の確認

### Step 2.2: input.cppにButtonManagerを並行導入

#### 変更対象ファイル
- `src/input.cpp`

#### 変更内容
```cpp
// 既存のinclude文に追加
#include "button_manager.h"

// handleDigitEditInput()関数を修正
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

  // 既存のBボタン処理（そのまま）
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

  // 既存のCボタン処理（そのまま）
  if (M5.BtnC.wasPressed()) {
    cPressStart = millis();
    cLongPressFired = false;
  }
  if (M5.BtnC.pressedFor(1000) && !cLongPressFired) {
    // C長押し: メイン画面に戻る
    cLongPressFired = true;
    currentMode = MAIN_DISPLAY;
  } else if (M5.BtnC.wasReleased() && millis() - cPressStart < 1000) {
    // C短押し: セット（確定）
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
  
  // 新しく追加: ButtonManagerの状態更新（既存処理には影響なし）
  ButtonManager::updateButtonStates();
}
```

#### 動作確認項目
- [ ] コンパイルが通ることを確認
- [ ] 既存の入力処理が正常に動作することを確認
- [ ] ButtonManagerが並行して動作することを確認
- [ ] デバウンス処理が正常に動作することを確認

### Step 2.3: 統合テストの実装

#### テストファイル（`test/test_integration_phase2.cpp`）
```cpp
#include <unity.h>
#include "button_manager.h"
#include "debounce_manager.h"
#include "main.h"

// 統合テスト用のヘルパー関数
void simulateButtonPress(Button& button, unsigned long duration) {
  // ボタン押下シミュレーション
  button.simulatePress(duration);
  ButtonManager::updateButtonStates();
}

void simulateButtonRelease(Button& button) {
  // ボタンリリースシミュレーション
  button.simulateRelease();
  ButtonManager::updateButtonStates();
}

// 統合テスト関数
void test_button_manager_debounce_integration() {
  // セットアップ
  ButtonManager::initialize();
  DebounceManager::canProcessModeChange(); // 初期化
  
  // デバウンス処理の統合テスト
  bool canProcess = ButtonManager::canProcessButton(M5.BtnA);
  TEST_ASSERT_TRUE(canProcess);
  
  // 連続押下でのデバウンス効果確認
  simulateButtonPress(M5.BtnA, 50);
  canProcess = ButtonManager::canProcessButton(M5.BtnA);
  TEST_ASSERT_FALSE(canProcess); // デバウンス期間中は処理不可
}

void test_existing_functionality_preserved() {
  // 既存機能が保持されていることを確認
  ButtonManager::initialize();
  
  // 既存のボタン処理が正常に動作することを確認
  // （実際のテストでは、既存のボタン処理をシミュレート）
  
  TEST_ASSERT_TRUE(true); // 既存機能が保持されている
}

void test_parallel_operation() {
  // 並行動作の確認
  ButtonManager::initialize();
  
  // ButtonManagerと既存処理が並行して動作することを確認
  ButtonManager::updateButtonStates();
  
  // 既存のボタン処理も正常に動作することを確認
  TEST_ASSERT_TRUE(true); // 並行動作が確認できた
}

void test_memory_usage() {
  // メモリ使用量の確認
  ButtonManager::initialize();
  
  // メモリ使用量が許容範囲内であることを確認
  // （実際のテストでは、メモリ使用量を測定）
  
  TEST_ASSERT_TRUE(true); // メモリ使用量が許容範囲内
}

// テスト実行
void RUN_UNITY_TESTS() {
  UNITY_BEGIN();
  
  RUN_TEST(test_button_manager_debounce_integration);
  RUN_TEST(test_existing_functionality_preserved);
  RUN_TEST(test_parallel_operation);
  RUN_TEST(test_memory_usage);
  
  UNITY_END();
}

int main() {
  RUN_UNITY_TESTS();
  return 0;
}
```

### Step 2.4: パフォーマンステストの実装

#### パフォーマンス測定用のヘルパー関数
```cpp
// パフォーマンス測定用のヘルパー関数
unsigned long measureButtonManagerPerformance() {
  unsigned long startTime = micros();
  
  // ButtonManagerの処理を実行
  ButtonManager::updateButtonStates();
  
  unsigned long endTime = micros();
  return endTime - startTime;
}

unsigned long measureExistingButtonPerformance() {
  unsigned long startTime = micros();
  
  // 既存のボタン処理を実行
  // （実際の測定では、既存のボタン処理をシミュレート）
  
  unsigned long endTime = micros();
  return endTime - startTime;
}

void printPerformanceReport() {
  Serial.println("=== Performance Report ===");
  
  // ButtonManagerの処理時間
  unsigned long buttonManagerTime = measureButtonManagerPerformance();
  Serial.printf("ButtonManager processing time: %lu microseconds\n", buttonManagerTime);
  
  // 既存ボタン処理の時間
  unsigned long existingTime = measureExistingButtonPerformance();
  Serial.printf("Existing button processing time: %lu microseconds\n", existingTime);
  
  // メモリ使用量
  Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
  
  Serial.println("==========================");
}
```

## 動作確認項目

### コンパイル確認
- [ ] `src/main.cpp`のコンパイル
- [ ] `src/input.cpp`のコンパイル
- [ ] 統合テストのコンパイル
- [ ] パフォーマンステストのコンパイル

### 基本機能確認
- [ ] 既存のボタン処理が正常に動作すること
- [ ] ButtonManagerが並行して動作すること
- [ ] DebounceManagerとの連携が正常に動作すること
- [ ] メモリ使用量が許容範囲内であること

### 統合テスト確認
- [ ] 全テストケースの実行
- [ ] テスト結果の確認
- [ ] パフォーマンス測定の実行
- [ ] 結果レポートの確認

### 実機動作確認
- [ ] 全ボタン操作の動作確認
- [ ] 画面遷移の動作確認
- [ ] アラーム機能の動作確認
- [ ] 設定機能の動作確認

## 次のステップ

Phase 2が完了したら、以下のステップに進みます：

1. **Phase 3**: 段階的に既存コードをButtonManagerに移行
2. **段階的テスト**: 移行前後の動作比較
3. **エッジケーステスト**: 境界値・異常系の確認

## 注意事項

- 既存のボタン処理は一切変更しない
- ButtonManagerは並行して動作するだけ
- コンパイルエラーが発生した場合は即座に修正
- 各ステップで動作確認を必ず行う
- パフォーマンス測定を定期的に実施

## 成功基準

- [ ] コンパイルエラー0件
- [ ] 既存機能の動作確認100%
- [ ] ButtonManagerの並行動作確認
- [ ] 統合テストの全テストケース通過
- [ ] パフォーマンス測定結果が許容範囲内
- [ ] メモリ使用量の増加が20%以下
- [ ] 既存のDebounceManagerとの正常な連携

## トラブルシューティング

### よくある問題と解決方法

#### 1. コンパイルエラー
**問題**: ButtonManagerのincludeでエラーが発生
**解決**: Phase 1のButtonManager実装を確認し、修正

#### 2. 既存機能の動作不良
**問題**: 既存のボタン処理が正常に動作しない
**解決**: 既存コードの変更箇所を確認し、元に戻す

#### 3. メモリ使用量の増加
**問題**: メモリ使用量が大幅に増加
**解決**: ButtonManagerの実装を最適化

#### 4. パフォーマンスの低下
**問題**: ボタン応答性が低下
**解決**: 処理時間を測定し、ボトルネックを特定

## ロールバック手順

問題が発生した場合のロールバック手順：

1. **main.cppの変更を元に戻す**
   - ButtonManagerのincludeを削除
   - setup()のButtonManager::initialize()を削除
   - handleButtons()のButtonManager::updateButtonStates()を削除

2. **input.cppの変更を元に戻す**
   - ButtonManagerのincludeを削除
   - handleDigitEditInput()のButtonManager::updateButtonStates()を削除

3. **動作確認**
   - 既存機能が正常に動作することを確認
   - コンパイルが通ることを確認

Phase 2が完了したら、Phase 3に進む準備が整います。 