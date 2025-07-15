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

（本設計書はStep1成果物。以降、実装・mock作成・テスト修正へ進む） 