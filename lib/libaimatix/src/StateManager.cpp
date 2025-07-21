#include "StateManager.h"

StateManager::StateManager() : currentState(nullptr) {}

void StateManager::setState(IState* state) {
    if (currentState) currentState->onExit();
    currentState = state;
    if (currentState) currentState->onEnter();
}

IState* StateManager::getCurrentState() const {
    return currentState;
}

void StateManager::handleButtonA() {
    if (currentState) currentState->onButtonA();
}
void StateManager::handleButtonB() {
    if (currentState) currentState->onButtonB();
}
void StateManager::handleButtonC() {
    if (currentState) currentState->onButtonC();
}
void StateManager::handleButtonALongPress() {
    if (currentState) currentState->onButtonALongPress();
}
void StateManager::handleButtonCLongPress() {
    if (currentState) currentState->onButtonCLongPress();
} 