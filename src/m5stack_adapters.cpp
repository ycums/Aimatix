#include "m5stack_adapters.h"

// グローバルアダプターインスタンス
M5StackEEPROMAdapter eepromAdapter;
M5StackSpeakerAdapter speakerAdapter;
M5StackButtonManagerAdapter buttonManagerAdapter;

// DebounceManagerの実体インスタンス（Business Logic Layer）
#include <lib/libaimatix/src/debounce_manager.h>
static DebounceManager debounceManagerInstance;

// M5StackDebounceManagerAdapterのグローバルインスタンス
M5StackDebounceManagerAdapter debounceManagerAdapter(&debounceManagerInstance); 