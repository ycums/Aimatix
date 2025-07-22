#pragma once
#include "StateManager.h"
#include "InputLogic.h"
#include "IInputDisplayView.h"
#include <stdio.h>

class InputDisplayState : public IState {
public:
    InputDisplayState(InputLogic* logic = nullptr, IInputDisplayView* view = nullptr)
        : inputLogic(logic), view(view), lastValue(-1), manager(nullptr), mainDisplayState(nullptr) {}
    void onEnter() override {
        if (inputLogic) inputLogic->reset();
        if (view) {
            view->clear();
            view->showTitle("INPUT", 42, false);
            view->showHints("OK", "", "CANCEL");
            lastValue = InputLogic::LAST_VALUE_INIT;
        }
    }
    void onExit() override {}
    void onDraw() override {
        if (inputLogic && view) {
            view->showValue(inputLogic->getDigits(), inputLogic->getEntered());
        }
        // プレビュー表示
        int value = inputLogic ? inputLogic->getValue() : InputLogic::EMPTY_VALUE;
        if (view) {
            char preview[16] = "";
            if (value != InputLogic::EMPTY_VALUE) {
                snprintf(preview, sizeof(preview), "プレビュー: %02d:%02d", value/100, value%100);
            }
            view->showPreview(preview);
        }
#ifndef ARDUINO
        // テスト用: 標準出力に値を出すだけ
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
    int lastValue;
    StateManager* manager;
    IState* mainDisplayState;
}; 