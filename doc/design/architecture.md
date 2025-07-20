# Aimatix アーキテクチャ設計書

## 1. 概要

Aimatixのアーキテクチャは、依存性注入（Dependency Injection）パターンを採用し、純粋ロジックとハードウェア依存部分を明確に分離しています。これにより、テスト容易性、再利用性、保守性の向上を実現しています。

## 2. アーキテクチャ原則

### 2.1 設計原則
- **単一責任原則**: 各モジュールは単一の責任を持つ
- **依存性逆転原則**: 抽象に依存し、具象に依存しない
- **開放閉鎖原則**: 拡張に対して開いており、修正に対して閉じている
- **インターフェース分離原則**: クライアントは使用しないインターフェースに依存しない

### 2.2 アーキテクチャ目標
- **テスト容易性**: モック注入による単体テスト
- **再利用性**: 純粋ロジックの他のプロジェクトでの再利用
- **保守性**: 明確な責務分離による保守性向上
- **拡張性**: 将来的な機能追加への対応

## 3. レイヤーアーキテクチャ

### 3.1 全体構成
```
┌─────────────────────────────────────┐
│              UI Layer               │
│  (src/ui.cpp, src/main.cpp)        │
├─────────────────────────────────────┤
│           Business Logic            │
│    (lib/libaimatix/src/)           │
├─────────────────────────────────────┤
│         Hardware Adapters           │
│  (src/m5stack_adapters.cpp/h)      │
├─────────────────────────────────────┤
│           M5Stack Hardware          │
│     (M5Stack Library)              │
└─────────────────────────────────────┘
```

### 3.2 レイヤー詳細

#### 3.2.1 UI Layer
**責務**: ユーザーインターフェース、画面表示、ユーザー操作の受付
**ファイル**: `src/ui.cpp/h`, `src/main.cpp`
**依存**: Business Logic Layer

**主要コンポーネント**:
- **MainDisplay**: メイン画面の表示
- **InputDisplay**: 入力画面の表示
- **AlarmDisplay**: アラーム画面の表示
- **SettingsDisplay**: 設定画面の表示

#### 3.2.2 Business Logic Layer
**責務**: 純粋ロジック、ビジネスルール、データ処理
**ファイル**: `lib/libaimatix/src/`
**依存**: インターフェース（抽象）

**主要コンポーネント**:
- **TimeLogic**: 時刻計算、時間管理
- **AlarmLogic**: アラーム管理、鳴動制御
- **InputLogic**: 入力処理、バリデーション
- **SettingsLogic**: 設定管理、永続化
- **ButtonManager**: ボタン状態管理
- **DebounceManager**: デバウンス処理

#### 3.2.3 Hardware Adapters Layer
**責務**: ハードウェア抽象化、インターフェース実装
**ファイル**: `src/m5stack_adapters.cpp/h`
**依存**: M5Stack Hardware Layer

**主要コンポーネント**:
- **M5StackEEPROMAdapter**: EEPROMアクセス
- **M5StackSpeakerAdapter**: スピーカー制御
- **M5StackButtonManagerAdapter**: ボタン管理

#### 3.2.4 M5Stack Hardware Layer
**責務**: 物理ハードウェア制御
**ファイル**: M5Stack Library
**依存**: なし

**主要コンポーネント**:
- **M5.Lcd**: ディスプレイ制御
- **M5.Speaker**: スピーカー制御
- **M5.BtnA/B/C**: ボタン制御
- **EEPROM**: 永続化ストレージ

## 4. 依存性注入パターン

### 4.1 インターフェース定義

#### 4.1.1 IEEPROM
```cpp
class IEEPROM {
public:
    virtual ~IEEPROM() {}
    virtual uint8_t read(int address) = 0;
    virtual void write(int address, uint8_t value) = 0;
    virtual void commit() = 0;
};
```

#### 4.1.2 ISpeaker
```cpp
class ISpeaker {
public:
    virtual ~ISpeaker() {}
    virtual void beep(int freq, int durationMs) = 0;
    virtual void stop() = 0;
};
```

#### 4.1.3 IButtonManager
```cpp
class IButtonManager {
public:
    virtual ~IButtonManager() {}
    virtual bool isPressed(int buttonId) = 0;
    virtual bool isLongPressed(int buttonId) = 0;
    virtual void update() = 0;
};
```

### 4.2 アダプター実装

#### 4.2.1 M5StackEEPROMAdapter
```cpp
class M5StackEEPROMAdapter : public IEEPROM {
public:
    M5StackEEPROMAdapter() {
        EEPROM.begin(512);
    }
    
    uint8_t read(int address) override {
        return EEPROM.read(address);
    }
    
    void write(int address, uint8_t value) override {
        EEPROM.write(address, value);
    }
    
    void commit() override {
        EEPROM.commit();
    }
};
```

#### 4.2.2 M5StackSpeakerAdapter
```cpp
class M5StackSpeakerAdapter : public ISpeaker {
public:
    void beep(int freq, int durationMs) override {
        M5.Speaker.tone(freq, durationMs);
    }
    
    void stop() override {
        M5.Speaker.mute();
    }
};
```

#### 4.2.3 M5StackButtonManagerAdapter
```cpp
class M5StackButtonManagerAdapter : public IButtonManager {
public:
    bool isPressed(int buttonId) override {
        switch (buttonId) {
            case 0: return M5.BtnA.isPressed();
            case 1: return M5.BtnB.isPressed();
            case 2: return M5.BtnC.isPressed();
            default: return false;
        }
    }
    
    bool isLongPressed(int buttonId) override {
        const unsigned long LONG_PRESS_TIME = 1000;
        switch (buttonId) {
            case 0: return M5.BtnA.pressedFor(LONG_PRESS_TIME);
            case 1: return M5.BtnB.pressedFor(LONG_PRESS_TIME);
            case 2: return M5.BtnC.pressedFor(LONG_PRESS_TIME);
            default: return false;
        }
    }
    
    void update() override {
        M5.update();
    }
};
```

## 5. 状態遷移アーキテクチャ

### 5.1 状態遷移システム
```
┌─────────────────────────────────────┐
│         State Transition            │
│         (src/state_transition/)     │
├─────────────────────────────────────┤
│         Button Event                │
│         (ButtonEvent)               │
├─────────────────────────────────────┤
│         System State                │
│         (SystemState)               │
├─────────────────────────────────────┤
│         Transition Result           │
│         (TransitionResult)          │
└─────────────────────────────────────┘
```

### 5.2 状態遷移フロー
1. **ボタンイベント生成**: ハードウェアボタンからイベント生成
2. **状態取得**: 現在のシステム状態を取得
3. **遷移処理**: 状態遷移ロジックで遷移を処理
4. **結果処理**: 遷移結果に基づいてアクション実行

## 6. データフローアーキテクチャ

### 6.1 時刻データフロー
```
NTP Server → WiFi → TimeSync → TimeLogic → UI Display
```

### 6.2 アラームデータフロー
```
User Input → InputLogic → AlarmLogic → Settings → EEPROM
AlarmLogic → UI Display → Speaker
```

### 6.3 ボタンデータフロー
```
Hardware Button → ButtonManager → StateTransition → UI Update
```

## 7. モジュール間依存関係

### 7.1 依存関係図
```
UI Layer
├── Business Logic Layer
│   ├── TimeLogic
│   ├── AlarmLogic
│   ├── InputLogic
│   ├── SettingsLogic
│   ├── ButtonManager
│   └── DebounceManager
├── Hardware Adapters Layer
│   ├── M5StackEEPROMAdapter
│   ├── M5StackSpeakerAdapter
│   └── M5StackButtonManagerAdapter
└── M5Stack Hardware Layer
    ├── M5.Lcd
    ├── M5.Speaker
    ├── M5.BtnA/B/C
    └── EEPROM
```

### 7.2 依存性ルール
- **上位レイヤー**: 下位レイヤーに依存可能
- **下位レイヤー**: 上位レイヤーに依存不可
- **同レイヤー内**: 相互依存可能
- **インターフェース**: 抽象に依存、具象に依存しない

## 8. テストアーキテクチャ

### 8.1 テスト戦略
- **Unit Test**: 純粋ロジックのテスト（native環境）
- **統合テスト**: モジュール間の連携テスト
- **実機テスト**: ハードウェア動作確認

### 8.2 モック戦略
- **Mock Adapters**: ハードウェアアダプターのモック
- **Mock Interfaces**: インターフェースのモック実装
- **Test Doubles**: テスト用の代替実装

## 9. パフォーマンスアーキテクチャ

### 9.1 メモリ管理
- **静的メモリ**: 固定サイズのデータ構造
- **動的メモリ**: 最小限の使用
- **メモリプール**: 効率的なメモリ管理

### 9.2 処理最適化
- **早期リターン**: 不要な処理の回避
- **効率的なアルゴリズム**: 最適な計算方法
- **キャッシュ**: 頻繁に使用されるデータのキャッシュ

## 10. セキュリティアーキテクチャ

### 10.1 入力検証
- **境界値チェック**: 入力値の範囲チェック
- **型チェック**: データ型の妥当性チェック
- **フォーマットチェック**: データ形式のチェック

### 10.2 エラーハンドリング
- **例外処理**: 適切な例外処理
- **ログ出力**: デバッグ情報の記録
- **復旧機能**: 自動復旧機能

## 11. コマンド/イベント駆動・副作用一元管理アーキテクチャ（拡張）

### 11.1 背景・目的
- 状態遷移の結果として発生する「UI更新」「EEPROM保存」「ネットワーク通信」などの副作用を、コマンド/イベントとして一元的に管理・実行することで、拡張性・保守性・テスト容易性をさらに高める。
- UI以外の副作用も柔軟に追加可能。

### 11.2 コマンド/イベント駆動の全体像
```
[EventProcessor]
    │
    ▼
[StateManager]
    │
    ├─ 状態更新
    └─ [UI更新, EEPROM保存, ...] などのコマンドリストを返す
    │
    ▼
[Command Dispatcher]
    ├─ UiRenderer（UI描画）
    ├─ EepromWriter（設定保存）
    └─ NetworkSender（通信）
```

### 11.3 コマンド/イベント型の設計例
```cpp
// コマンドの種類
enum class CommandType {
    UpdateUI,
    SaveSettings,
    SendNetwork,
    // ... 必要に応じて拡張
};

// コマンド構造体
struct Command {
    CommandType type;
    // 必要に応じてパラメータを追加
    // 例: UI更新内容、保存対象データ、送信先アドレス等
};
```

### 11.4 Effect/Command Dispatcherの責務
- StateManagerから返されたコマンドリストを受け取り、
  - UiRenderer（UI描画）
  - EepromWriter（設定保存）
  - NetworkSender（通信）
  などの各担当に処理を振り分ける
- 各Effect Handlerは単一責任を厳守

### 11.5 メリット
- UI以外の副作用も同じ仕組みで拡張可能
- テスト容易性：StateManagerの出力（コマンドリスト）を検証するだけでロジックテストが可能
- 責務分離・保守性・拡張性のさらなる向上

### 11.6 既存設計との関係
- 既存のレイヤー分離・依存抽象化・DI設計はそのまま活かせる
- 本方式は現状設計の“発展形”であり、より大規模・複雑なシステムにも適用可能

---

**作成日**: 2025年1月  
**バージョン**: 1.0.0  
**更新日**: 2025年1月 