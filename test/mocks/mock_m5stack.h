#ifndef MOCK_M5STACK_H
#define MOCK_M5STACK_H

#include <cstdint>

// テスト用のM5Stackモッククラス
class MockM5Stack {
public:
  // ボタンクラス
  class Button {
  public:
    bool isPressed() const { return pressed; }
    bool wasPressed() const { return wasPressedFlag; }
    bool wasReleased() const { return wasReleasedFlag; }
    bool pressedFor(unsigned long duration) const { return pressed && (millis() - pressStartTime >= duration); }
    
    void simulatePress() {
      pressed = true;
      wasPressedFlag = true;
      pressStartTime = millis();
    }
    
    void simulateRelease() {
      pressed = false;
      wasReleasedFlag = true;
    }
    
    void reset() {
      pressed = false;
      wasPressedFlag = false;
      wasReleasedFlag = false;
      pressStartTime = 0;
    }
    
  private:
    bool pressed = false;
    bool wasPressedFlag = false;
    bool wasReleasedFlag = false;
    unsigned long pressStartTime = 0;
  };
  
  // ディスプレイクラス
  class Display {
  public:
    void fillScreen(uint16_t color) {}
    void setTextColor(uint16_t color, uint16_t bgColor = 0) {}
    void setTextFont(uint8_t font) {}
    void setTextDatum(uint8_t datum) {}
    void drawString(const char* str, int x, int y, uint8_t font = 2) {}
    void drawLine(int x0, int y0, int x1, int y1, uint16_t color) {}
    void drawRect(int x, int y, int w, int h, uint16_t color) {}
    void fillRect(int x, int y, int w, int h, uint16_t color) {}
    void setBrightness(uint8_t brightness) {}
  };
  
  // スピーカークラス
  class Speaker {
  public:
    void tone(uint16_t frequency, uint32_t duration) {}
    void mute() {}
  };
  
  // 電源管理クラス
  class Power {
  public:
    int getBatteryLevel() const { return 75; } // 75%を返す
    bool isCharging() const { return false; }
  };
  
  // メソッド
  void begin() {}
  void update() {}
  
  // メンバー変数
  Button BtnA, BtnB, BtnC;
  Display Lcd;
  Speaker Speaker;
  Power Power;
};

// グローバルなM5Stackインスタンス
extern MockM5Stack M5;

// 色定義
#define TFT_BLACK 0x0000
#define TFT_WHITE 0xFFFF
#define TFT_RED 0xF800
#define TFT_GREEN 0x07E0
#define TFT_BLUE 0x001F

// フォント定義
#define MC_DATUM 0
#define TL_DATUM 1
#define TR_DATUM 2
#define ML_DATUM 3
#define MR_DATUM 4
#define BL_DATUM 5
#define BR_DATUM 6
#define BC_DATUM 7

// スプライトクラス
class TFT_eSprite {
public:
  TFT_eSprite(MockM5Stack::Display* display) {}
  
  bool createSprite(int width, int height) { return true; }
  void fillSprite(uint16_t color) {}
  void setTextColor(uint16_t color, uint16_t bgColor = 0) {}
  void setTextFont(uint8_t font) {}
  void setTextDatum(uint8_t datum) {}
  void drawString(const char* str, int x, int y) {}
  void drawLine(int x0, int y0, int x1, int y1, uint16_t color) {}
  void drawRect(int x, int y, int w, int h, uint16_t color) {}
  void fillRect(int x, int y, int w, int h, uint16_t color) {}
  void pushSprite(int x, int y) {}
  uint8_t getTextDatum() const { return MC_DATUM; }
  int fontHeight(uint8_t font) const { return 16; }
};

#endif // MOCK_M5STACK_H 