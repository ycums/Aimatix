#ifndef M5STACK_ADAPTERS_H
#define M5STACK_ADAPTERS_H

#include <M5Stack.h>
#include <EEPROM.h>
#include <lib/libaimatix/include/IEEPROM.h>
#include <lib/libaimatix/include/ISpeaker.h>
#include <lib/libaimatix/include/IButtonManager.h>
#include <lib/libaimatix/include/IDebounceManager.h>

// M5Stack EEPROMアダプター
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

// M5Stack スピーカーアダプター
class M5StackSpeakerAdapter : public ISpeaker {
public:
    void beep(int freq, int durationMs) override {
        M5.Speaker.tone(freq, durationMs);
    }
    
    void stop() override {
        M5.Speaker.mute();
    }
};

// M5Stack ボタン管理アダプター
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

// M5Stack デバウンス管理アダプター
class M5StackDebounceManagerAdapter : public IDebounceManager {
private:
    IDebounceManager* debounceManager;
    
public:
    M5StackDebounceManagerAdapter(IDebounceManager* manager) : debounceManager(manager) {}
    
    bool canProcessHardware(ButtonType buttonId, unsigned long (*getTime)()) override {
        return debounceManager->canProcessHardware(buttonId, getTime);
    }
    
    bool canProcessOperation(const std::string& operationType, unsigned long (*getTime)()) override {
        return debounceManager->canProcessOperation(operationType, getTime);
    }
    
    bool canProcessModeChange(unsigned long (*getTime)()) override {
        return debounceManager->canProcessModeChange(getTime);
    }
    
    void reset() override {
        debounceManager->reset();
    }
};

// グローバルアダプターインスタンス
extern M5StackEEPROMAdapter eepromAdapter;
extern M5StackSpeakerAdapter speakerAdapter;
extern M5StackButtonManagerAdapter buttonManagerAdapter;
extern M5StackDebounceManagerAdapter debounceManagerAdapter;

#endif // M5STACK_ADAPTERS_H 