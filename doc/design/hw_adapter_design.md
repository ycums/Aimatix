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

## 5. DisplayAdapter（M5Unified対応）

### 責務
- IDisplayインターフェースを実装し、M5Unified/M5GFXのAPIに委譲

### 主API例
```cpp
class DisplayAdapter : public IDisplay {
public:
    void clear() override { M5.Display.fillScreen(TFT_BLACK); }
    
    void drawText(int x, int y, const char* text, int fontSize) override {
        M5.Display.setTextFont(fontSize);
        M5.Display.drawString(text, x, y);
    }
    
    void setTextColor(uint16_t color, uint16_t bgColor) override {
        M5.Display.setTextColor(color, bgColor);
    }
    
    void fillRect(int x, int y, int w, int h, uint16_t color) override {
        M5.Display.fillRect(x, y, w, h, color);
    }
    
    void drawRect(int x, int y, int w, int h, uint16_t color) override {
        M5.Display.drawRect(x, y, w, h, color);
    }
    
    void drawLine(int x0, int y0, int x1, int y1, uint16_t color) override {
        M5.Display.drawLine(x0, y0, x1, y1, color);
    }
    
    void fillProgressBarSprite(int x, int y, int w, int h, int percent) override {
        M5Canvas canvas(&M5.Display);
        canvas.createSprite(w, h);
        canvas.fillSprite(TFT_BLACK);
        canvas.drawRect(0, 0, w, h, AMBER_COLOR);
        
        int fillW = (w - 2) * percent / 100;
        if (fillW > 0) {
            canvas.fillRect(1, 1, fillW, h - 2, AMBER_COLOR);
        }
        
        canvas.pushSprite(x, y);
        canvas.deleteSprite();
    }
};
```

### 設計意図
- 純粋ロジック層はIDisplay経由で画面描画を行い、M5Unified依存を排除
- M5GFXの色型（uint16_t）に統一し、型の一貫性を保持
- テスト時はMockDisplayAdapter等に差し替え可能

### M5Unified移行の技術的考慮事項
- **色の型統一**: M5GFXでは色の型が`uint16_t`（16ビット）であるため、インターフェースも`uint16_t`に統一
- **型キャスト削除**: 内部での型変換を排除し、直接的な型一致を実現
- **色定数の統一**: `AMBER_COLOR`、`TFT_BLACK`等の色定数を統一使用

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
- M5Unified移行後は色の型を`uint16_t`に統一し、型の一貫性を保持 