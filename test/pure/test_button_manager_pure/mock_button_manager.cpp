#include "mock_button_manager.h"

MockButtonManager::MockButtonManager() {}
MockButtonManager::~MockButtonManager() {}

bool MockButtonManager::isPressed(ButtonType buttonId) {
    return pressedStates[buttonId];
}

bool MockButtonManager::isLongPressed(ButtonType buttonId) {
    return longPressedStates[buttonId];
}

void MockButtonManager::update() {
    // テスト用なので何もしない
}

void MockButtonManager::setPressed(ButtonType buttonId, bool pressed) {
    pressedStates[buttonId] = pressed;
}

void MockButtonManager::setLongPressed(ButtonType buttonId, bool longPressed) {
    longPressedStates[buttonId] = longPressed;
}

void MockButtonManager::reset() {
    pressedStates.clear();
    longPressedStates.clear();
} 