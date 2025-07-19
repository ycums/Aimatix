#ifndef IDEBOUNCE_MANAGER_H
#define IDEBOUNCE_MANAGER_H

#include "IButtonManager.h"

// DebounceManagerのインターフェース定義
class IDebounceManager {
public:
    virtual ~IDebounceManager() {}
    
    // ハードウェアレベルのデバウンス判定
    virtual bool canProcessHardware(ButtonType buttonId, unsigned long (*getTime)()) = 0;
    
    // 操作レベルのデバウンス判定
    virtual bool canProcessOperation(const std::string& operationType, unsigned long (*getTime)()) = 0;
    
    // 画面遷移レベルのデバウンス判定
    virtual bool canProcessModeChange(unsigned long (*getTime)()) = 0;
};

#endif // IDEBOUNCE_MANAGER_H 