#pragma once
#include "../../lib/aimatix_lib/include/IButtonManager.h"
#include <map>

class MockButtonManager : public IButtonManager {
public:
    MockButtonManager();
    ~MockButtonManager() override;
    // IButtonManager実装
    bool isPressed(int buttonId) override;
    bool isLongPressed(int buttonId) override;
    void update() override;
    // テスト用API
    void setPressed(int buttonId, bool pressed);
    void setLongPressed(int buttonId, bool longPressed);
    void reset();
private:
    std::map<int, bool> pressedStates;
    std::map<int, bool> longPressedStates;
}; 