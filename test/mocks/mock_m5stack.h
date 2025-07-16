#ifndef MOCK_M5STACK_H
#define MOCK_M5STACK_H

// テスト環境ではM5Stack.hをインクルードしない
#ifdef TEST_MODE
  #include <cstdint>
  #include <ctime>
  #include <cstdio>
  #include <cstring>
#else
  #include <M5Stack.h>
  #include <cstdint>
  #include <ctime>
  #include <cstdio>
  #include <cstring>
#endif

// Windows環境でのヘッダー競合を回避
#ifdef _WIN32
  // Windows環境ではtime_tを再定義しない
  #ifndef MOCK_TIME_T_DEFINED
    #define MOCK_TIME_T_DEFINED
  #endif
#else
  // 非Windows環境ではArduinoの定義を使用
  typedef long time_t;
#endif

// localtime_rの代替実装（Windows用）
#ifdef _WIN32
inline struct tm* localtime_r(const time_t* timep, struct tm* result) {
    if (localtime_s(result, timep) == 0) {
        return result;
    }
    return nullptr;
}
#endif

// Arduino関数のモック（先に宣言）
extern unsigned long mockMillis;
extern time_t mockTime;

unsigned long millis();
time_t time(time_t* t);
void delay(unsigned long ms);

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
    void stop() {}
    void mute() {}
  };
  
  // 電源管理クラス
  class Power {
  public:
    int getBatteryLevel() const { return 75; } // 75%を返す
    bool isCharging() const { return false; }
    void begin() {}
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

// 色定義（条件付きコンパイルで重複を回避）
#ifndef TFT_BLACK
#define TFT_BLACK 0x0000
#endif
#ifndef TFT_WHITE
#define TFT_WHITE 0xFFFF
#endif
#ifndef TFT_RED
#define TFT_RED 0xF800
#endif
#ifndef TFT_GREEN
#define TFT_GREEN 0x07E0
#endif
#ifndef TFT_BLUE
#define TFT_BLUE 0x001F
#endif
#ifndef TFT_YELLOW
#define TFT_YELLOW 0xFFE0
#endif
#ifndef TFT_CYAN
#define TFT_CYAN 0x07FF
#endif
#ifndef TFT_MAGENTA
#define TFT_MAGENTA 0xF81F
#endif
#ifndef TFT_ORANGE
#define TFT_ORANGE 0xFD20
#endif

// プロジェクト固有の色定義
#ifndef AMBER_COLOR
#define AMBER_COLOR 0xFB20
#endif
#ifndef FLASH_ORANGE
#define FLASH_ORANGE 0xF000
#endif
#ifndef DARKGREY
#define DARKGREY 0x4208
#endif

// フォント定義
#ifndef MC_DATUM
#define MC_DATUM 0
#endif
#ifndef TL_DATUM
#define TL_DATUM 1
#endif
#ifndef TR_DATUM
#define TR_DATUM 2
#endif
#ifndef ML_DATUM
#define ML_DATUM 3
#endif
#ifndef MR_DATUM
#define MR_DATUM 4
#endif
#ifndef BL_DATUM
#define BL_DATUM 5
#endif
#ifndef BR_DATUM
#define BR_DATUM 6
#endif
#ifndef BC_DATUM
#define BC_DATUM 7
#endif

// スプライトクラス（条件付きコンパイル）
#ifndef TFT_eSprite_DEFINED
#define TFT_eSprite_DEFINED
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
#endif

// シリアル通信のモック
class MockSerial {
public:
  void begin(unsigned long baud) {}
  void print(const char* str) { printf("%s", str); }
  void print(int value) { printf("%d", value); }
  void print(unsigned long value) { printf("%lu", value); }
  void print(long value) { printf("%ld", value); }
  void print(long long value) { printf("%lld", value); }
  void print(size_t value) { printf("%zu", value); }
  void println() { printf("\n"); }
  void println(const char* str) { printf("%s\n", str); }
  void println(int value) { printf("%d\n", value); }
  void println(unsigned long value) { printf("%lu\n", value); }
  void println(long value) { printf("%ld\n", value); }
  void println(long long value) { printf("%lld\n", value); }
  void println(size_t value) { printf("%zu\n", value); }
};

extern MockSerial Serial;

// EEPROMのモック（条件付きコンパイル）
#ifndef EEPROMClass_DEFINED
#define EEPROMClass_DEFINED
class MockEEPROM {
public:
  void begin(size_t size) {}
  uint8_t read(int address) { return 0; }
  void write(int address, uint8_t value) {}
  void commit() {}
  
  // get/putメソッドを追加
  template<typename T>
  T get(int address, T defaultValue) { return defaultValue; }
  
  template<typename T>
  void put(int address, const T& value) {}
};
#endif

extern MockEEPROM EEPROM;

// WiFi status constants（先に定義）
#ifndef WL_CONNECTED
#define WL_CONNECTED 3
#endif
#ifndef WL_DISCONNECTED
#define WL_DISCONNECTED 6
#endif

// WiFiのモック
class MockWiFi {
public:
  static void begin(const char* ssid, const char* password) {}
  static int status() { return WL_CONNECTED; }
};

// NTPClientのモック
class MockNTPClient {
public:
  MockNTPClient(void* udp, const char* poolServerName, int timeOffset, int updateInterval) {}
  void begin() {}
  void update() {}
  time_t getEpochTime() { return mockTime; }
};

// その他の必要な定数
#ifndef EEPROM_SIZE
#define EEPROM_SIZE 512
#endif
#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH 320
#endif
#ifndef SCREEN_HEIGHT
#define SCREEN_HEIGHT 240
#endif
#ifndef TITLE_HEIGHT
#define TITLE_HEIGHT 20
#endif
#ifndef HINT_HEIGHT
#define HINT_HEIGHT 20
#endif
#ifndef GRID_WIDTH
#define GRID_WIDTH 20
#endif
#ifndef GRID_HEIGHT
#define GRID_HEIGHT 20
#endif

// グリッド座標変換マクロ（条件付きコンパイル）
#ifndef GRID_X
#define GRID_X(x) ((x) * GRID_WIDTH)
#endif
#ifndef GRID_Y
#define GRID_Y(y) ((y) * GRID_HEIGHT)
#endif

#endif // MOCK_M5STACK_H 