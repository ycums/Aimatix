# 依存抽象化インターフェース設計書（Step1）

## 目的・背景
M5Stack Fire固有の機能（LCD, スピーカー, EEPROM, ボタン管理）への依存を排除し、純粋ロジック層から抽象インターフェース経由でアクセスできるようにする。これにより、
- テスト容易性（mock注入によるnativeテスト）
- 再利用性・保守性の向上
- 将来的なハードウェア差し替え対応
を実現する。

---

## インターフェース設計方針
- 各機能ごとに責務を明確に分離した純粋仮想クラス（interface）を定義
- 本番環境ではM5Stack実装、テスト環境ではmock実装を注入
- 必要最小限のAPI設計とし、今後の拡張性も考慮

---

## インターフェース定義例

### 1. IDisplay（LCD表示用）
```cpp
// LCD表示の抽象インターフェース
class IDisplay {
public:
    virtual ~IDisplay() {}
    virtual void clear() = 0; // 画面クリア
    virtual void drawText(int x, int y, const char* text, int fontSize) = 0; // テキスト描画
    virtual void setTextColor(uint32_t color) = 0; // 文字色設定
    virtual void fillRect(int x, int y, int w, int h, uint32_t color) = 0; // 矩形塗りつぶし
    // 必要に応じて追加
};
```
- **責務**: 画面描画、色・フォント管理
- **設計意図**: M5.Lcd依存排除、テスト時はダミー描画

---

### 2. ISpeaker（スピーカー制御用）
```cpp
// スピーカー制御の抽象インターフェース
class ISpeaker {
public:
    virtual ~ISpeaker() {}
    virtual void beep(int freq, int durationMs) = 0; // ビープ音
    virtual void stop() = 0; // 停止
    // 必要に応じて追加
};
```
- **責務**: 音の再生・停止
- **設計意図**: M5.Speaker依存排除、テスト時はログ出力等

---

### 3. IEEPROM（EEPROMアクセス用）
```cpp
// EEPROMアクセスの抽象インターフェース
class IEEPROM {
public:
    virtual ~IEEPROM() {}
    virtual uint8_t read(int address) = 0; // 読み出し
    virtual void write(int address, uint8_t value) = 0; // 書き込み
    virtual void commit() = 0; // フラッシュ保存
    // 必要に応じて追加
};
```
- **責務**: 永続データの読み書き
- **設計意図**: EEPROM依存排除、テスト時はメモリ上で模擬

---

### 4. IButtonManager（ボタン状態管理用）
```cpp
// ボタン管理の抽象インターフェース
class IButtonManager {
public:
    virtual ~IButtonManager() {}
    virtual bool isPressed(int buttonId) = 0; // 押下状態取得
    virtual bool isLongPressed(int buttonId) = 0; // 長押し判定
    virtual void update() = 0; // 状態更新
    // 必要に応じて追加（同時押し、シーケンス等）
};
```
- **責務**: ボタン状態の一元管理、長押し/短押し/同時押し等の判定
- **設計意図**: M5.BtnA/B/C等の直接参照排除、テスト時は状態を任意に制御

---

## 今後の拡張・注意点
- 必要に応じてインターフェースを拡張（例：バイブレーション、バックライト制御等）
- APIは最小限から開始し、実装・テスト段階で追加検討
- 各インターフェースは単一責務を厳守
- 本番実装・mock実装の両立を常に意識

---

## 補足：コマンド/イベント駆動・副作用一元管理アーキテクチャへの拡張

- 本設計書のインターフェース群（IDisplay, ISpeaker, IEEPROM, IButtonManager等）は、各種副作用（UI描画、保存、通信など）を単一責任で抽象化する思想に基づく。
- より大規模・複雑なシステムや副作用の拡張性を重視する場合、
  - 状態遷移の結果として「UI更新」「EEPROM保存」「ネットワーク通信」などのコマンド/イベントをリストで返し、
  - Effect/Command Dispatcherがそれぞれのインターフェース実装（UiRenderer, EepromWriter, NetworkSender等）に処理を振り分ける
  方式が有効。
- この拡張方式により、UI以外の副作用も同じ仕組みで柔軟に追加・管理できる。
- 既存のインターフェース設計思想（単一責任・DI）はそのまま活かせる。

---

# インターフェース設計指針（責務分離）

- ロジック層（InputLogic, SettingsLogic等）は純粋ロジックのみ担当し、UI遷移や画面制御は一切行わない。
- UI層が「入力確定/保存」イベントを受けて画面遷移・UI反映を制御する。

## nextMode API設計例
```cpp
Mode nextMode(Mode current, ButtonType btn, ButtonAction act);
```
- 画面遷移ロジックはこの純粋関数で一元管理し、main.cppでcurrentModeを更新する。

## SettingsLogic API設計例
```cpp
void loadSettings(IEEPROM* eeprom, Settings& settings);
void saveSettings(IEEPROM* eeprom, const Settings& settings);
void resetSettings(IEEPROM* eeprom, Settings& settings);
bool validateSettings(const Settings& settings);
```
- 設定のロード・保存・リセット・バリデーションのみを担当し、UI遷移や画面制御は一切行わない。

## UIイベントハンドラ設計例
```cpp
if (buttonManager.isShortPress(BUTTON_TYPE_B)) {
  currentMode = nextMode(currentMode, BUTTON_TYPE_B, SHORT_PRESS);
}
if (currentMode == SETTINGS_MENU) {
  // 設定保存・反映処理
}
```

（本設計書はStep1成果物。以降、実装・mock作成・テスト修正へ進む） 