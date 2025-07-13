#ifndef M5STACK_H
#define M5STACK_H

#ifdef MOCK_M5STACK

// Mock M5Stack library for native testing
#include "Arduino.h"
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

// Arduino types are defined in Arduino.h

// Mock Button class
class Button {
public:
    Button() : pressed(false), wasPressedFlag(false), wasReleasedFlag(false), pressStartTime(0) {}
    
    bool isPressed() const { return pressed; }
    bool wasPressed() { bool result = wasPressedFlag; wasPressedFlag = false; return result; }
    bool wasReleased() { bool result = wasReleasedFlag; wasReleasedFlag = false; return result; }
    bool pressedFor(unsigned long duration) { 
        if (pressed && (millis() - pressStartTime) >= duration) return true;
        return false;
    }
    
    // For testing
    void setPressed(bool state) { 
        if (state && !pressed) {
            wasPressedFlag = true;
            pressStartTime = millis();
        }
        if (!state && pressed) wasReleasedFlag = true;
        pressed = state; 
    }
    
private:
    bool pressed;
    bool wasPressedFlag;
    bool wasReleasedFlag;
    unsigned long pressStartTime;
};

// Mock M5Stack class
class M5Stack {
public:
    M5Stack() {}
    void begin() {}
    void update() {}
    
    // Mock button states
    
    Button BtnA;
    Button BtnB;
    Button BtnC;
    
    // Mock display methods
    void setCursor(int16_t x, int16_t y) {}
    void setTextSize(uint8_t size) {}
    void setTextColor(uint16_t color) {}
    void fillScreen(uint16_t color) {}
    void drawString(const char* str, int16_t x, int16_t y) {}
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {}
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {}
    
    // Mock Lcd class
    class Lcd {
    public:
        Lcd() {}
        void setBrightness(uint8_t brightness) {}
        void fillScreen(uint16_t color) {}
        void setTextColor(uint16_t color, uint16_t bgcolor) {}
        void drawString(const char* str, int16_t x, int16_t y, uint8_t font = 4) {}
    };
    
    Lcd Lcd;
    
    // Mock Sprite class
    class Sprite {
    public:
        Sprite() {}
        void pushSprite(int16_t x, int16_t y) {}
        void fillSprite(uint16_t color) {}
        void setTextColor(uint16_t color, uint16_t bgcolor) {}
        void drawString(const char* str, int16_t x, int16_t y, uint8_t font = 4) {}
    };
    
    Sprite sprite;
    
    // Mock TFT_eSprite class (alias for Sprite)
    typedef Sprite TFT_eSprite;
    
    // Mock speaker
    class Speaker {
    public:
        Speaker() {}
        void tone(uint16_t frequency, uint32_t duration = 0) {}
        void mute() {}
    };
    
    Speaker Speaker;
    
    // Mock battery
    class Power {
    public:
        Power() {}
        void begin() {}
        uint8_t getBatteryLevel() { return 100; }
    };
    
    Power Power;
    
    // Mock RTC
    void Rtc() {}
    void setTime(uint32_t time) {}
    uint32_t getTime() { return 0; }
};

// Global M5Stack instance
extern M5Stack M5;

// Mock colors
#define TFT_BLACK     0x0000
#define TFT_NAVY      0x000F
#define TFT_DARKGREEN 0x03E0
#define TFT_DARKCYAN  0x03EF
#define TFT_MAROON    0x7800
#define TFT_PURPLE    0x780F
#define TFT_OLIVE     0x7BE0
#define TFT_LIGHTGREY 0xC618
#define TFT_DARKGREY  0x7BEF
#define TFT_BLUE      0x001F
#define TFT_GREEN     0x07E0
#define TFT_CYAN      0x07FF
#define TFT_RED       0xF800
#define TFT_MAGENTA   0xF81F
#define TFT_YELLOW    0xFFE0
#define TFT_WHITE     0xFFFF
#define TFT_ORANGE    0xFDA0
#define TFT_GREENYELLOW 0xB7E0
#define TFT_PINK      0xFC9F

// Mock Arduino functions
unsigned long millis();
void delay(unsigned long ms);

#else
// Include real M5Stack library
#include <M5Stack.h>
#endif

#endif // M5STACK_H 