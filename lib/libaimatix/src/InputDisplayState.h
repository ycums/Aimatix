#pragma once
#include "StateManager.h"
#include "InputLogic.h"
#include "IInputDisplayView.h"
#include <stdio.h>

class InputDisplayState : public IState {
public:
    InputDisplayState(InputLogic* logic = nullptr, IInputDisplayView* view = nullptr)
        : inputLogic(logic), view(view), lastValue(-1) {}
    void onEnter() override {
        if (view) {
            view->clear();
            view->showTitle("INPUT", 42, false);
            view->showHints("OK", "", "CANCEL");
            lastValue = -1;
        }
    }
    void onExit() override {}
    void onDraw() override {
        int value = inputLogic ? inputLogic->getValue() : 0;
        if (view && value != lastValue) {
            view->showValue(value);
            lastValue = value;
        }
#ifndef ARDUINO
        // テスト用: 標準出力に値を出すだけ
        printf("[InputDisplay] value=%d\n", value);
#endif
    }
    void onButtonA() override {}
    void onButtonB() override {}
    void onButtonC() override {}
    void onButtonALongPress() override {}
    void onButtonCLongPress() override {}
    void setView(IInputDisplayView* v) { view = v; }
private:
    InputLogic* inputLogic;
    IInputDisplayView* view;
    int lastValue;
}; 