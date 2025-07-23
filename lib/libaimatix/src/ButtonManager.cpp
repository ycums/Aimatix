#include "ButtonManager.h"

void ButtonManager::update(ButtonType btn, bool pressed, uint32_t now_ms) {
    BtnState& s = btnStates[btn];
    // デバウンス処理
    if (pressed != s.pressed) {
        if ((now_ms - s.lastChange) <= DEBOUNCE_MS) {
            return;
        }
        s.pressed = pressed;
        s.lastChange = now_ms;
        if (pressed) {
            // 押下開始
            s.pressStart = now_ms;
            s.shortFired = false;
            s.longFired = false;
            s.fired = false;
        } else {
            // 離上
            if (!s.fired && (now_ms - s.pressStart) < LONG_PRESS_MS) {
                s.shortFired = true;
                s.fired = true;
            }
        }
    }
    // fired==trueなら何もしない
    if (s.fired) return;
    // 長押し判定（押下中に閾値を超えた瞬間のみ1回）
    if (s.pressed && (now_ms - s.pressStart) >= LONG_PRESS_MS) {
        s.longFired = true;
        s.fired = true;
    }
}

bool ButtonManager::isShortPress(ButtonType btn) const {
    bool ret = btnStates[btn].shortFired;
    const_cast<BtnState&>(btnStates[btn]).shortFired = false;
    return ret;
}

bool ButtonManager::isLongPress(ButtonType btn) const {
    bool ret = btnStates[btn].longFired;
    const_cast<BtnState&>(btnStates[btn]).longFired = false;
    return ret;
}

void ButtonManager::reset(ButtonType btn) {
    btnStates[btn] = BtnState();
} 