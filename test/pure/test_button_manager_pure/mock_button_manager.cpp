#include "mock_button_manager.h"

MockButtonManager::MockButtonManager() {}
MockButtonManager::~MockButtonManager() {}

bool MockButtonManager::isPressed(int buttonId) {
    return pressedStates[buttonId];
}

bool MockButtonManager::isLongPressed(int buttonId) {
    return longPressedStates[buttonId];
}

void MockButtonManager::update() {
    // テスト用なので何もしない
}

void MockButtonManager::setPressed(int buttonId, bool pressed) {
    pressedStates[buttonId] = pressed;
}

void MockButtonManager::setLongPressed(int buttonId, bool longPressed) {
    longPressedStates[buttonId] = longPressed;
}

void MockButtonManager::reset() {
    pressedStates.clear();
    longPressedStates.clear();
} 