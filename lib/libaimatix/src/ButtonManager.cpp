#include "ButtonManager.h"
#include <cstddef>

void ButtonManager::update(ButtonType btn, bool pressed, uint32_t now_ms) {
    if (btn < 0 || btn >= static_cast<ButtonType>(sizeof(btnStates)/sizeof(btnStates[0]))) {
        return; // 無効なボタンタイプ
    }
    BtnState& btn_state = btnStates[static_cast<size_t>(btn)];
    // デバウンス処理
    if (pressed != btn_state.pressed) {
        if ((now_ms - btn_state.lastChange) <= BM_DEBOUNCE_MS) {
            return;
        }
        btn_state.pressed = pressed;
        btn_state.lastChange = now_ms;
        if (pressed) {
            // 押下開始
            btn_state.pressStart = now_ms;
            btn_state.shortFired = false;
            btn_state.longFired = false;
            btn_state.fired = false;
        } else {
            // 離上
            if (!btn_state.fired && (now_ms - btn_state.pressStart) < BM_LONG_PRESS_MS) {
                btn_state.shortFired = true;
                btn_state.fired = true;
            }
        }
    }
    // fired==trueなら何もしない
    if (btn_state.fired) { return; }
    // 長押し判定（押下中に閾値を超えた瞬間のみ1回）
    if (btn_state.pressed && (now_ms - btn_state.pressStart) >= BM_LONG_PRESS_MS) {
        btn_state.longFired = true;
        btn_state.fired = true;
    }
}

auto ButtonManager::isShortPress(ButtonType btn) const -> bool {
    if (btn < 0 || btn >= static_cast<ButtonType>(sizeof(btnStates)/sizeof(btnStates[0]))) {
        return false; // 無効なボタンタイプ
    }
    const bool ret = btnStates[static_cast<size_t>(btn)].shortFired;
    const_cast<BtnState&>(btnStates[static_cast<size_t>(btn)]).shortFired = false;
    return ret;
}

auto ButtonManager::isLongPress(ButtonType btn) const -> bool {
    if (btn < 0 || btn >= static_cast<ButtonType>(sizeof(btnStates)/sizeof(btnStates[0]))) {
        return false; // 無効なボタンタイプ
    }
    const bool ret = btnStates[static_cast<size_t>(btn)].longFired;
    const_cast<BtnState&>(btnStates[static_cast<size_t>(btn)]).longFired = false;
    return ret;
}

void ButtonManager::reset(ButtonType btn) {
    if (btn < 0 || btn >= static_cast<ButtonType>(sizeof(btnStates)/sizeof(btnStates[0]))) {
        return; // 無効なボタンタイプ
    }
    btnStates[static_cast<size_t>(btn)] = BtnState();
} 