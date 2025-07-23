#pragma once
#include "StateManager.h"
#include "InputLogic.h"
#include "IInputDisplayView.h"
#include <stdio.h>

class InputDisplayState : public IState {
public:
    InputDisplayState(InputLogic* logic = nullptr, IInputDisplayView* view = nullptr)
        : inputLogic(logic), view(view), manager(nullptr), mainDisplayState(nullptr) {
        for (int i = 0; i < 4; ++i) { lastDigits[i] = -1; lastEntered[i] = false; }
    }
    void onEnter() override {
        if (inputLogic) inputLogic->reset();
        if (view) {
            view->clear();
            view->showTitle("INPUT", 42, false);
            view->showHints("OK", "", "CANCEL");
            for (int i = 0; i < 4; ++i) { lastDigits[i] = -1; lastEntered[i] = false; }
        }
    }
    void onExit() override {}
    void onDraw() override {
        if (inputLogic && view) {
            const int* digits = inputLogic->getDigits();
            const bool* entered = inputLogic->getEntered();
            for (int i = 0; i < 4; ++i) {
                if (digits[i] != lastDigits[i] || entered[i] != lastEntered[i]) {
                    view->showDigit(i, digits[i], entered[i]);
                    lastDigits[i] = digits[i];
                    lastEntered[i] = entered[i];
                }
            }
        }
        // プレビュー表示などはそのまま
        int value = inputLogic ? inputLogic->getValue() : InputLogic::EMPTY_VALUE;
        if (view) {
            char preview[16] = "";
            if (value != InputLogic::EMPTY_VALUE) {
                snprintf(preview, sizeof(preview), "プレビュー: %02d:%02d", value/100, value%100);
            }
            view->showPreview(preview);
            view->showColon();
        }
#ifndef ARDUINO
        printf("[InputDisplay] value=%d\n", value);
#endif
    }
    void onButtonA() override {
        if (inputLogic) {
            inputLogic->incrementAtCursor(1);
        }
        onDraw();
    }
    void onButtonB() override {}
    void onButtonC() override {}
    void onButtonALongPress() override {
        if (inputLogic) {
            inputLogic->incrementAtCursor(5);
        }
        onDraw();
    }
    void onButtonBLongPress() override {
        // 必要ならリセット等の処理をここに
    }
    void onButtonCLongPress() override {
        if (manager && mainDisplayState) {
            manager->setState(mainDisplayState);
        }
    }
    void setView(IInputDisplayView* v) { view = v; }
    // StateManager, MainDisplayStateのsetterを追加
    void setManager(StateManager* m) { manager = m; }
    void setMainDisplayState(IState* mainState) { mainDisplayState = mainState; }
private:
    InputLogic* inputLogic;
    IInputDisplayView* view;
    int lastDigits[4] = {-1,-1,-1,-1};
    bool lastEntered[4] = {false,false,false,false};
    StateManager* manager;
    IState* mainDisplayState;
}; 