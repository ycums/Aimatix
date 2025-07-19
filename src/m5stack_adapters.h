#ifndef M5STACK_ADAPTERS_H
#define M5STACK_ADAPTERS_H

#include <M5Stack.h>
#include <EEPROM.h>
#include "../lib/libaimatix/include/IButtonManager.h"
#include "../lib/libaimatix/include/ISpeaker.h"
#include "../lib/libaimatix/include/IEEPROM.h"

// M5Stack用EEPROMアダプター
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

// M5Stack用スピーカーアダプター
class M5StackSpeakerAdapter : public ISpeaker {
public:
    void beep(int freq, int durationMs) override {
        M5.Speaker.tone(freq, durationMs);
    }
    
    void stop() override {
        M5.Speaker.mute();
    }
};

// M5Stack用ボタン管理アダプター
class M5StackButtonManagerAdapter : public IButtonManager {
public:
    M5StackButtonManagerAdapter() : lastUpdateTime(0) {}
    
    bool isPressed(ButtonType buttonId) override {
        switch (buttonId) {
            case BUTTON_TYPE_A: return M5.BtnA.isPressed();
            case BUTTON_TYPE_B: return M5.BtnB.isPressed();
            case BUTTON_TYPE_C: return M5.BtnC.isPressed();
            default: return false;
        }
    }
    
    bool isLongPressed(ButtonType buttonId) override {
        const unsigned long LONG_PRESS_TIME = 1000;
        switch (buttonId) {
            case BUTTON_TYPE_A: return M5.BtnA.pressedFor(LONG_PRESS_TIME);
            case BUTTON_TYPE_B: return M5.BtnB.pressedFor(LONG_PRESS_TIME);
            case BUTTON_TYPE_C: return M5.BtnC.pressedFor(LONG_PRESS_TIME);
            default: return false;
        }
    }
    
    void update() override {
        M5.update();
        lastUpdateTime = millis();
    }
    
private:
    unsigned long lastUpdateTime;
};

// グローバルアダプターインスタンス
extern M5StackEEPROMAdapter eepromAdapter;
extern M5StackSpeakerAdapter speakerAdapter;
extern M5StackButtonManagerAdapter buttonManagerAdapter;

#endif // M5STACK_ADAPTERS_H 