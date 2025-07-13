# アラーム鳴動状態詳細仕様書

## 概要
M5Stack Fire集中タイマーのアラーム鳴動状態に関する詳細仕様。フラッシュ点滅、オーバータイム表示、音制御の動作とUI仕様を定義する。

## 画面構成

### 全体レイアウト
```
┌─────────────────────────────────────┐
│ アラーム鳴動中（フラッシュ点滅）     │ ← フラッシュオレンジ⇔黒
├─────────────────────────────────────┤
│                                     │
│                                     │
│        オーバータイム表示           │ ← Font7, 中央配置
│        (+HH:MM:SS)                  │   グリッド(0,3)-(15,5)
│                                     │
│                                     │
│                                     │
│                                     │
│                                     │
│                                     │
├─────────────────────────────────────┤
│ Press any button to stop            │ ← Font2, 中央配置
└─────────────────────────────────────┘
```

### レイアウト詳細
- **全体画面**: フラッシュオレンジ⇔黒の点滅
- **表示時間**: 最大5秒間
- **停止条件**: 任意ボタン押下または5秒経過

## 表示要素仕様

### 1. フラッシュ点滅
- **点滅間隔**: 0.5秒
- **色**: フラッシュオレンジ(#F000)⇔黒(#0000)
- **範囲**: 画面全体
- **効果**: アラーム鳴動の視覚的警告

### 2. オーバータイム表示
- **表示位置**: グリッドセル(0,3)-(15,5)
- **フォント**: Font7（高さ48px）
- **色**: フラッシュオレンジ時は黒、黒時はフラッシュオレンジ
- **配置**: 水平中央寄せ
- **形式**: +HH:MM:SS
- **計算**: 現在時刻 - アラーム時刻

### 3. 停止指示
- **表示位置**: 画面下部
- **フォント**: Font2（高さ16px）
- **色**: フラッシュオレンジ時は黒、黒時はフラッシュオレンジ
- **内容**: "Press any button to stop"

## 音制御仕様

### 音の設定
- **周波数**: 880Hz
- **間隔**: 500ms間隔で間欠制御
- **設定**: 設定で無効化可能
- **停止条件**: 任意ボタン押下または5秒経過

### 音制御ロジック
```cpp
void playAlarm() {
  if (settings.sound_enabled) {
    M5.Speaker.tone(880, 500);  // 880Hz, 500ms
  }
}

void stopAlarm() {
  M5.Speaker.stop();
}
```

## 状態遷移仕様

### アラーム開始
- **トリガー**: アラーム時刻到達
- **遷移元**: 任意のモード
- **遷移先**: ALARM_ACTIVE
- **処理**: フラッシュ開始、音開始

### アラーム停止
- **トリガー**: 任意ボタン押下または5秒経過
- **遷移元**: ALARM_ACTIVE
- **遷移先**: MAIN_DISPLAY
- **処理**: フラッシュ停止、音停止、解除時刻更新

## データ処理仕様

### オーバータイム計算
```cpp
time_t getOvertime() {
  time_t now = time(NULL);
  time_t alarmTime = getNextAlarmTime();
  
  if (alarmTime == 0) return 0;
  return now - alarmTime;
}
```

### フラッシュ制御
```cpp
void updateAlarmFlash() {
  static bool flash = false;
  static unsigned long lastFlash = 0;
  static unsigned long alarmStart = 0;
  
  if (alarmStart == 0) alarmStart = millis();
  unsigned long elapsed = millis() - alarmStart;
  
  // 最大5秒間で制限
  if (elapsed >= 5000) {
    stopAlarm();
    return;
  }
  
  // 0.5秒ごとに点滅
  if (millis() - lastFlash >= 500) {
    flash = !flash;
    lastFlash = millis();
  }
  
  // 画面全体を点滅
  if (flash) {
    sprite.fillSprite(FLASH_ORANGE);
    sprite.setTextColor(TFT_BLACK, FLASH_ORANGE);
  } else {
    sprite.fillSprite(TFT_BLACK);
    sprite.setTextColor(FLASH_ORANGE, TFT_BLACK);
  }
}
```

## UI描画仕様

### オーバータイム表示描画
```cpp
void drawOvertimeDisplay() {
  time_t overtime = getOvertime();
  
  if (overtime > 0) {
    int hours = overtime / 3600;
    int minutes = (overtime % 3600) / 60;
    int seconds = overtime % 60;
    
    char overtimeStr[16];
    sprintf(overtimeStr, "+%02d:%02d:%02d", hours, minutes, seconds);
    
    sprite.setTextDatum(MC_DATUM);
    sprite.setTextFont(7);
    sprite.drawString(overtimeStr, SCREEN_WIDTH/2, GRID_Y(3) + GRID_HEIGHT * 1.5);
  }
}
```

### 停止指示描画
```cpp
void drawStopInstruction() {
  sprite.setTextDatum(MC_DATUM);
  sprite.setTextFont(2);
  sprite.drawString("Press any button to stop", SCREEN_WIDTH/2, SCREEN_HEIGHT - 30);
}
```

## 状態管理

### アラーム開始時刻
- **変数**: `alarmStartTime`（ローカル変数）
- **用途**: 5秒間の制限時間管理
- **初期化**: アラーム開始時に設定

### フラッシュ状態
- **変数**: `flashState`（ローカル変数）
- **用途**: 点滅状態の管理
- **更新**: 0.5秒間隔で切り替え

### 解除時刻更新
- **変数**: `lastReleaseTime`（グローバル変数）
- **更新タイミング**: アラーム停止時
- **用途**: 進捗バー計算の基準時刻

## エラー処理

### 時刻異常
- **負のオーバータイム**: 0として表示
- **異常に大きな値**: 表示範囲内に制限
- **時刻取得失敗**: デフォルト値を使用

### 音制御エラー
- **スピーカー初期化失敗**: 音なしで動作継続
- **設定読み込み失敗**: デフォルトで音有効

### 表示エラー
- **メモリ不足**: 最小限の表示のみ
- **描画失敗**: エラー状態をログ出力

## パフォーマンス仕様

### 更新頻度
- **フラッシュ更新**: 2Hz（0.5秒間隔）
- **オーバータイム更新**: 1Hz（1秒間隔）
- **音制御**: 2Hz（0.5秒間隔）

### メモリ使用
- **フラッシュ制御**: 16バイト
- **時刻計算**: 8バイト
- **UI描画**: スプライトバッファ使用

## 制限事項

### 機能制限
- **最大表示時間**: 5秒間
- **音の周波数**: 固定880Hz
- **点滅間隔**: 固定0.5秒

### ハードウェア制限
- **スピーカー**: 1W-0928（音質制限）
- **画面**: 320×240px（表示領域制限）
- **処理能力**: ESP32（計算制限）

## 将来拡張予定

### 機能拡張
- **複数音色**: 異なる周波数の音
- **振動パターン**: RGB LEDの点滅パターン
- **カスタムアラーム**: ユーザー設定可能な音

### UI改善
- **アニメーション**: スムーズな点滅
- **グラデーション**: 色の変化
- **3D効果**: 視覚的な奥行き

### 設定拡張
- **音量調整**: 0-100%の音量設定
- **音色選択**: 複数の音色から選択
- **表示時間**: ユーザー設定可能な表示時間 