#ifndef DEBOUNCE_MANAGER_H
#define DEBOUNCE_MANAGER_H

#include <map>
#include <string>
#include "../include/IDebounceManager.h"

// DebounceManagerクラス（IDebounceManagerインターフェース実装）
class DebounceManager : public IDebounceManager {
public:
    DebounceManager();
    DebounceManager(const DebounceManager&) = delete;
    DebounceManager& operator=(const DebounceManager&) = delete;
    
    // IDebounceManagerインターフェースの実装
    bool canProcessHardware(ButtonType buttonId, unsigned long (*getTime)()) override;
    bool canProcessOperation(const std::string& operationType, unsigned long (*getTime)()) override;
    bool canProcessModeChange(unsigned long (*getTime)()) override;

private:
    static const unsigned long DEFAULT_HARDWARE_DEBOUNCE = 50;
    static const unsigned long DEFAULT_OPERATION_DEBOUNCE = 200;
    static const unsigned long DEFAULT_MODE_CHANGE_DEBOUNCE = 300;

    std::map<std::string, unsigned long> lastOperationTimes;
    unsigned long lastModeChangeTime;
    std::map<ButtonType, unsigned long> lastButtonChangeTimes;
};

#endif // DEBOUNCE_MANAGER_H 