#pragma once
#include "StateManager.h"
#include "InputDisplayState.h"
#include "IMainDisplayView.h"

class MainDisplayState : public IState {
public:
    MainDisplayState(StateManager* mgr, InputDisplayState* inputState, IMainDisplayView* view = nullptr)
        : manager(mgr), inputDisplayState(inputState), view(view) {}
    void onEnter() override {
        if (view) {
            view->clear();
            view->showTitle("MAIN", 42, false);
            view->showHints("A:INPUT", "", "C:MENU");
        }
    }
    void onExit() override {}
    void onDraw() override {
        if (view) {
            // 仮実装: 現在時刻・進捗はダミー
            view->showTime("12:00");
            view->showProgress(0);
        }
    }
    void onButtonA() override {
        if (manager && inputDisplayState) {
            manager->setState(inputDisplayState);
        }
    }
    void onButtonB() override {}
    void onButtonC() override {}
    void onButtonALongPress() override {}
    void onButtonCLongPress() override {}
    void setView(IMainDisplayView* v) { view = v; }
private:
    StateManager* manager;
    InputDisplayState* inputDisplayState;
    IMainDisplayView* view;
}; 