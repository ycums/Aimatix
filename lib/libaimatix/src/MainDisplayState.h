#pragma once
#include "StateManager.h"
#include "InputDisplayState.h"
extern void drawMainDisplay();

class MainDisplayState : public IState {
public:
    MainDisplayState(StateManager* mgr, InputDisplayState* inputState)
        : manager(mgr), inputDisplayState(inputState) {}
    void onEnter() override {}
    void onExit() override {}
    void onDraw() override { drawMainDisplay(); }
    void onButtonA() override {
        if (manager && inputDisplayState) {
            manager->setState(inputDisplayState);
        }
    }
    void onButtonB() override {}
    void onButtonC() override {}
    void onButtonALongPress() override {}
    void onButtonCLongPress() override {}
private:
    StateManager* manager;
    InputDisplayState* inputDisplayState;
}; 