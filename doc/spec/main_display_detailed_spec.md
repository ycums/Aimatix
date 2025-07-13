# メイン表示画面詳細仕様書

## 概要
M5Stack Fire集中タイマーのメイン表示画面に関する詳細仕様。残り時間表示、進捗バー、アラームリストの表示とUI仕様を定義する。

## 画面構成

### 全体レイアウト
```
┌─────────────────────────────────────┐
│ タイトルバー: MAIN + バッテリー     │ ← Font2, アンバー色
├─────────────────────────────────────┤
│                                     │
│        現在時刻 (HH:MM)             │ ← Font4, 中央配置
│                                     │   グリッド(0,1)-(15,2)
│                                     │
│        残り時間 (HH:MM:SS)          │ ← Font7, 中央配置
│                                     │   グリッド(0,3)-(15,5)
│                                     │
│        進捗バー                     │ ← 高さ12px
│                                     │   グリッド(0,6)-(15,7)
│                                     │
│        アラームリスト               │ ← Font2, 水平配置
│                                     │   グリッド(0,8)-(15,9)
├─────────────────────────────────────┤
│ ABS  REL+  MGMT                     │ ← Font2, アンバー色
└─────────────────────────────────────┘
```

### レイアウト詳細
- **全体グリッド**: 16×12（320×240px）
- **タイトルバー**: 上部固定（1グリッド高さ、20px）
- **コンテンツエリア**: 16×10グリッド（座標系: (0,0)-(15,9)）
- **ボタンヒント**: 下部固定（1グリッド高さ、20px）

## 表示要素仕様

### 1. 現在時刻表示
- **表示位置**: グリッドセル(0,1)-(15,2)
- **フォント**: Font4（高さ26px）
- **色**: アンバー色（#FB20）
- **配置**: 水平中央寄せ
- **形式**: HH:MM
- **更新頻度**: 1秒間隔

### 2. 残り時間表示
- **表示位置**: グリッドセル(0,3)-(15,5)
- **フォント**: Font7（高さ48px）
- **色**: アンバー色（#FB20）
- **配置**: 水平中央寄せ
- **形式**: HH:MM:SS
- **計算ロジック**: 次アラーム時刻 - 現在時刻
- **更新頻度**: 1秒間隔
- **特殊表示**: アラームなし時は "00:00:00"

### 3. 進捗バー
- **表示位置**: グリッドセル(0,6)-(15,7)
- **高さ**: 12px
- **色**: アンバー色（#FB20）
- **背景**: 黒色（#0000）
- **進捗率計算**: (現在時刻 - 解除時刻) / (次アラーム時刻 - 解除時刻)
- **境界値処理**: 0.0-1.0の範囲に制限
- **更新頻度**: 1秒間隔

### 4. アラームリスト
- **表示位置**: グリッドセル(0,8)-(15,9)
- **フォント**: Font2（高さ16px）
- **色**: アンバー色（#FB20）
- **配置**: 水平配置、5等分
- **最大表示数**: 5件
- **形式**: HH:MM
- **表示対象**: 現在時刻以降のアラームのみ
- **更新タイミング**: アラーム追加・削除時

## データ処理仕様

### 残り時間計算
```cpp
time_t getRemainingTime() {
  time_t now = time(NULL);
  time_t nextAlarm = getNextAlarmTime();
  
  if (nextAlarm == 0) return 0;
  return nextAlarm - now;
}
```

### 進捗率計算
```cpp
float getProgressRate() {
  time_t now = time(NULL);
  time_t nextAlarm = getNextAlarmTime();
  time_t lastRelease = getLastReleaseTime();
  
  if (nextAlarm <= lastRelease) return 0.0f;
  
  float total = nextAlarm - lastRelease;
  float done = now - lastRelease;
  
  if (total <= 0) return 0.0f;
  if (done < 0) return 0.0f;
  if (done > total) return 1.0f;
  
  return done / total;
}
```

### アラームリスト取得
```cpp
std::vector<time_t> getFutureAlarms() {
  time_t now = time(NULL);
  std::vector<time_t> futureAlarms;
  
  for (time_t alarm : alarmTimes) {
    if (alarm > now) {
      futureAlarms.push_back(alarm);
    }
  }
  
  std::sort(futureAlarms.begin(), futureAlarms.end());
  return futureAlarms;
}
```

## UI描画仕様

### 進捗バー描画
```cpp
void drawProgressBar(float progress) {
  int x = GRID_X(0);
  int y = GRID_Y(6) + (2 * GRID_HEIGHT - 12) / 2; // 12px高さで中央寄せ
  int width = 16 * GRID_WIDTH;
  int height = 12;
  
  // 境界線
  sprite.drawRect(x, y, width, height, AMBER_COLOR);
  
  // 背景
  sprite.fillRect(x + 1, y + 1, width - 2, height - 2, TFT_BLACK);
  
  // 進捗
  int progressWidth = (width - 2) * progress;
  if (progressWidth > 0) {
    sprite.fillRect(x + 1, y + 1, progressWidth, height - 2, AMBER_COLOR);
  }
}
```

### アラームリスト描画
```cpp
void drawAlarmList() {
  std::vector<time_t> futureAlarms = getFutureAlarms();
  
  sprite.setTextFont(2);
  sprite.setTextColor(AMBER_COLOR, TFT_BLACK);
  sprite.setTextDatum(ML_DATUM);
  
  int count = 0;
  for (time_t alarm : futureAlarms) {
    if (count >= 5) break;
    
    int x = GRID_X(1) + count * (14 * GRID_WIDTH / 5);
    int y = GRID_Y(8) + GRID_HEIGHT / 2;
    
    sprite.drawString(getTimeString(alarm), x, y);
    count++;
  }
}
```

## 状態管理

### 解除時刻の管理
- **変数**: `lastReleaseTime`（グローバル変数）
- **更新タイミング**: アラーム停止時
- **初期値**: 0（アラーム未設定時）

### アラーム時刻の管理
- **データ構造**: `std::vector<time_t> alarmTimes`
- **最大数**: 5件
- **自動ソート**: 時刻順に自動ソート
- **過去アラーム削除**: 自動的に削除

## エラー処理

### データ不整合
- **進捗率異常**: 0.0-1.0の範囲に制限
- **時刻異常**: 現在時刻以前のアラームは表示しない
- **メモリ不足**: 最大5件で制限

### 表示エラー
- **フォントサイズ**: 画面サイズに合わせて調整
- **文字列長**: 最大長を制限
- **色範囲**: 有効な色値のみ使用

## パフォーマンス仕様

### 更新頻度
- **メインループ**: 100Hz（10ms間隔）
- **画面更新**: 100ms間隔
- **時刻更新**: 1秒間隔
- **アラームチェック**: 1秒間隔

### メモリ使用
- **アラームリスト**: 最大5件 × 8バイト = 40バイト
- **時刻計算**: 一時変数 16バイト
- **UI描画**: スプライトバッファ 320×240×2 = 153,600バイト

## 制限事項

### 機能制限
- **最大アラーム数**: 5件
- **表示精度**: 秒単位
- **時刻範囲**: 現在時刻から24時間以内

### ハードウェア制限
- **画面サイズ**: 320×240px固定
- **色深度**: RGB565（65,536色）
- **更新速度**: 最大60fps

## 将来拡張予定

### 機能拡張
- **複数アラーム表示**: スクロール機能
- **詳細情報表示**: アラーム名、繰り返し設定
- **統計情報**: 集中時間の統計

### UI改善
- **アニメーション**: スムーズな進捗表示
- **テーマ変更**: 複数のカラーテーマ
- **カスタマイズ**: ユーザー設定可能なレイアウト 