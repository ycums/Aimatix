#pragma once
#include <cstdint>

class ButtonManager {
public:
    enum ButtonType { BUTTON_A, BUTTON_B, BUTTON_C };
    enum ButtonAction { NONE, SHORT_PRESS, LONG_PRESS };

    // ボタン状態を更新（押下/離上/時刻）
    void update(ButtonType btn, bool pressed, uint32_t now_ms);
    // 短押し判定
    bool isShortPress(ButtonType btn) const;
    // 長押し判定
    bool isLongPress(ButtonType btn) const;
    // 状態リセット
    void reset(ButtonType btn);
private:
    struct BtnState {
        bool pressed = false;
        uint32_t lastChange = 0;
        bool shortFired = false;
        bool longFired = false;
    } btnStates[3];
    static constexpr uint32_t LONG_PRESS_MS = 800;
    static constexpr uint32_t DEBOUNCE_MS = 50;
}; 