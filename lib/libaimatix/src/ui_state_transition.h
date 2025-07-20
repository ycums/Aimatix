#ifndef UI_STATE_TRANSITION_H
#define UI_STATE_TRANSITION_H

#include "types.h"
#include "../include/IButtonManager.h"

// 画面遷移ロジック（純粋関数）
inline Mode nextMode(Mode current, ButtonType btn, ButtonAction act) {
    if (current == MAIN_DISPLAY && btn == BUTTON_TYPE_B && act == SHORT_PRESS) {
        return SETTINGS_MENU;
    }
    if (current == MAIN_DISPLAY && btn == BUTTON_TYPE_C && act == SHORT_PRESS) {
        return INFO_DISPLAY;
    }
    if (current == SETTINGS_MENU && btn == BUTTON_TYPE_C && act == LONG_PRESS) {
        return MAIN_DISPLAY;
    }
    // ...他の遷移も必要に応じて追加
    return current;
}

#endif // UI_STATE_TRANSITION_H 