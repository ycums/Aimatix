# ハードウェアアダプタ設計書

本ドキュメントは、Aimatixのハードウェアアダプタ（M5StackEEPROMAdapter, M5StackSpeakerAdapter, M5StackButtonManagerAdapter, M5StackDisplayAdapter等）の設計方針・主API・実装例を明文化するものです。

---

## 1. 設計方針
- 純粋ロジック層からハード依存を排除し、抽象インターフェース（IEEPROM, ISpeaker, IButtonManager, IDisplay等）経由でアクセス
- 本番環境ではM5Stack実装、テスト環境ではmock実装をDIで切替
- 各アダプタは単一責任を厳守

---

## 2. M5StackEEPROMAdapter

### 責務
- IEEPROMインターフェースを実装し、M5StackのEEPROM APIに委譲

### 主API例
```cpp
class M5StackEEPROMAdapter : public IEEPROM {
public:
    M5StackEEPROMAdapter() { EEPROM.begin(512); }
    uint8_t read(int address) override { return EEPROM.read(address); }
    void write(int address, uint8_t value) override { EEPROM.write(address, value); }
    void commit() override { EEPROM.commit(); }
};
```

### 設計意図
- 純粋ロジック層はIEEPROM経由で永続化処理を行い、M5Stack依存を排除
- テスト時はMockEEPROMAdapter等に差し替え可能

---

## 3. M5StackSpeakerAdapter

### 責務
- ISpeakerインターフェースを実装し、M5StackのSpeaker APIに委譲

### 主API例
```cpp
class M5StackSpeakerAdapter : public ISpeaker {
public:
    void beep(int freq, int durationMs) override { M5.Speaker.tone(freq, durationMs); }
    void stop() override { M5.Speaker.mute(); }
};
```

### 設計意図
- 純粋ロジック層はISpeaker経由で音制御を行い、M5Stack依存を排除
- テスト時はMockSpeakerAdapter等に差し替え可能

---

## 4. M5StackButtonManagerAdapter

### 責務
- IButtonManagerインターフェースを実装し、M5StackのボタンAPIに委譲

### 主API例
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
    void update() override { M5.update(); }
};
```

### 設計意図
- 純粋ロジック層はIButtonManager経由でボタン状態を取得し、M5Stack依存を排除
- テスト時はMockButtonManagerAdapter等に差し替え可能

---

## 5. M5StackDisplayAdapter（例）

### 責務
- IDisplayインターフェースを実装し、M5StackのLCD APIに委譲

### 主API例
```cpp
class M5StackDisplayAdapter : public IDisplay {
public:
    void clear() override { M5.Lcd.clear(); }
    void drawText(int x, int y, const char* text, int fontSize) override { M5.Lcd.drawString(text, x, y, fontSize); }
    void setTextColor(uint32_t color) override { M5.Lcd.setTextColor(color); }
    void fillRect(int x, int y, int w, int h, uint32_t color) override { M5.Lcd.fillRect(x, y, w, h, color); }
};
```

### 設計意図
- 純粋ロジック層はIDisplay経由で画面描画を行い、M5Stack依存を排除
- テスト時はMockDisplayAdapter等に差し替え可能

---

## 6. 本番/テスト実装の切替方針
- DI（依存性注入）により、main.cpp等で本番アダプタ/モックアダプタを切替
- 例：
```cpp
// 本番
IEEPROM* eeprom = new M5StackEEPROMAdapter();
// テスト
IEEPROM* eeprom = new MockEEPROMAdapter();
```
- テスト容易性・拡張性・保守性を重視

---

## 補足
- すべてのアダプタは「単一責任」「DIによるテスト容易性」「拡張性」を重視
- 実装・テスト段階でAPI追加・修正が必要な場合は本ドキュメントを随時更新 