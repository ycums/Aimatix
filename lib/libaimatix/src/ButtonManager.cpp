#include "ButtonManager.h"

void ButtonManager::update(ButtonType btn, bool pressed, uint32_t now_ms) {
    BtnState& s = btnStates[btn];
    if (pressed != s.pressed && (now_ms - s.lastChange) > DEBOUNCE_MS) {
        s.pressed = pressed;
        s.lastChange = now_ms;
        if (!pressed) {
            // 離した瞬間
            if (!s.shortFired && (now_ms - s.lastChange) < LONG_PRESS_MS) {
                s.shortFired = true;
            }
            s.longFired = false;
        } else {
            // 押した瞬間
            s.shortFired = false;
            s.longFired = false;
        }
    }
    // 長押し判定
    if (s.pressed && !s.longFired && (now_ms - s.lastChange) >= LONG_PRESS_MS) {
        s.longFired = true;
    }
}

bool ButtonManager::isShortPress(ButtonType btn) const {
    return btnStates[btn].shortFired;
}

bool ButtonManager::isLongPress(ButtonType btn) const {
    return btnStates[btn].longFired;
}

void ButtonManager::reset(ButtonType btn) {
    btnStates[btn] = BtnState();
} 