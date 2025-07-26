#include "StateManager.h"

StateManager::StateManager() : currentState(nullptr) {}

void StateManager::setState(IState* state) {
    if (currentState != nullptr) {
        currentState->onExit();
    }
    currentState = state;
    if (currentState != nullptr) {
        currentState->onEnter();
    }
}

IState* StateManager::getCurrentState() const {
    return currentState;
}

void StateManager::handleButtonA() {
    if (currentState != nullptr) {
        currentState->onButtonA();
    }
}
void StateManager::handleButtonB() {
    if (currentState != nullptr) {
        currentState->onButtonB();
    }
}
void StateManager::handleButtonC() {
    if (currentState != nullptr) {
        currentState->onButtonC();
    }
}
void StateManager::handleButtonALongPress() {
    if (currentState != nullptr) {
        currentState->onButtonALongPress();
    }
}
void StateManager::handleButtonCLongPress() {
    if (currentState != nullptr) {
        currentState->onButtonCLongPress();
    }
}
void StateManager::handleButtonBLongPress() {
    if (currentState != nullptr) {
        currentState->onButtonBLongPress();
    }
} 