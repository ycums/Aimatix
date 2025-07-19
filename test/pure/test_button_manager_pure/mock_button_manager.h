#pragma once
#include "../../lib/libaimatix/include/IButtonManager.h"
#include <map>

class MockButtonManager : public IButtonManager {
public:
    MockButtonManager();
    ~MockButtonManager() override;
    // IButtonManager実装
    bool isPressed(ButtonType buttonId) override;
    bool isLongPressed(ButtonType buttonId) override;
    void update() override;
    // テスト用API
    void setPressed(ButtonType buttonId, bool pressed);
    void setLongPressed(ButtonType buttonId, bool longPressed);
    void reset();
private:
    std::map<ButtonType, bool> pressedStates;
    std::map<ButtonType, bool> longPressedStates;
}; 