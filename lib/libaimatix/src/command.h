#ifndef COMMAND_H
#define COMMAND_H

#include "settings.h"

enum class CommandType {
    UpdateUI,
    SaveSettings,
    SetBrightness,
    ShowWarning,
    // ... 必要に応じて拡張
};

struct Command {
    CommandType type;
    int intValue = 0;
    const char* message = nullptr;
    int screenId = 0; // 画面IDやUI更新対象など
    Settings settingsSnapshot; // 設定値のスナップショット
};

#endif // COMMAND_H 