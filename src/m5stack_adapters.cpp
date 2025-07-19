#include "m5stack_adapters.h"

// グローバルアダプターインスタンス
M5StackEEPROMAdapter eepromAdapter;
M5StackSpeakerAdapter speakerAdapter;
M5StackButtonManagerAdapter buttonManagerAdapter;

// DebounceManagerの実体インスタンス（Business Logic Layer）
#include <debounce_manager.h>
static DebounceManager debounceManagerInstance;

// M5StackDebounceManagerAdapterのグローバルインスタンス
M5StackDebounceManagerAdapter debounceManagerAdapter(&debounceManagerInstance); 

bool M5StackButtonManagerAdapter::isPressed(ButtonType buttonId) {
    switch (buttonId) {
        case BUTTON_TYPE_A: return M5.BtnA.isPressed();
        case BUTTON_TYPE_B: return M5.BtnB.isPressed();
        case BUTTON_TYPE_C: return M5.BtnC.isPressed();
        default: return false;
    }
}
bool M5StackButtonManagerAdapter::isLongPressed(ButtonType buttonId) {
    const unsigned long LONG_PRESS_TIME = 1000;
    switch (buttonId) {
        case BUTTON_TYPE_A: return M5.BtnA.pressedFor(LONG_PRESS_TIME);
        case BUTTON_TYPE_B: return M5.BtnB.pressedFor(LONG_PRESS_TIME);
        case BUTTON_TYPE_C: return M5.BtnC.pressedFor(LONG_PRESS_TIME);
        default: return false;
    }
} 

void M5StackDebounceManagerAdapter::reset() {
    // 必要ならdebounceManagerのリセット処理をここに実装
} 