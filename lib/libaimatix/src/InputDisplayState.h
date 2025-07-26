#pragma once
#include "StateManager.h"
#include "InputLogic.h"
#include "IInputDisplayView.h"
#include <stdio.h>
#include <vector>
#include <ctime>
#include "AlarmLogic.h"

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
            view->showHints("+1/+5", "Next/Reset", "Set/Cancel");
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
#ifndef ARDUINO
            printf("[InputDisplay] A button pressed - before increment\n");
            printf("[InputDisplay] Current state: %d%d:%d%d (entered: %d%d%d%d)\n", 
                inputLogic->getDigit(0), inputLogic->getDigit(1), 
                inputLogic->getDigit(2), inputLogic->getDigit(3),
                inputLogic->isEntered(0), inputLogic->isEntered(1), 
                inputLogic->isEntered(2), inputLogic->isEntered(3));
#endif
            inputLogic->incrementInput(1);
#ifndef ARDUINO
            printf("[InputDisplay] A button pressed - after increment\n");
            printf("[InputDisplay] New state: %d%d:%d%d (entered: %d%d%d%d)\n", 
                inputLogic->getDigit(0), inputLogic->getDigit(1), 
                inputLogic->getDigit(2), inputLogic->getDigit(3),
                inputLogic->isEntered(0), inputLogic->isEntered(1), 
                inputLogic->isEntered(2), inputLogic->isEntered(3));
#endif
        }
        onDraw();
    }
    void onButtonB() override {
        if (inputLogic) {
#ifndef ARDUINO
            printf("[InputDisplay] B button pressed - before shift\n");
            printf("[InputDisplay] Current state: %d%d:%d%d (entered: %d%d%d%d)\n", 
                inputLogic->getDigit(0), inputLogic->getDigit(1), 
                inputLogic->getDigit(2), inputLogic->getDigit(3),
                inputLogic->isEntered(0), inputLogic->isEntered(1), 
                inputLogic->isEntered(2), inputLogic->isEntered(3));
#endif
            // 桁送りを試行
            bool success = inputLogic->shiftDigits();
#ifndef ARDUINO
            printf("[InputDisplay] B button pressed - shift result: %s\n", success ? "SUCCESS" : "FAILED");
            if (success) {
                printf("[InputDisplay] New state: %d%d:%d%d (entered: %d%d%d%d)\n", 
                    inputLogic->getDigit(0), inputLogic->getDigit(1), 
                    inputLogic->getDigit(2), inputLogic->getDigit(3),
                    inputLogic->isEntered(0), inputLogic->isEntered(1), 
                    inputLogic->isEntered(2), inputLogic->isEntered(3));
            }
#endif
            // 成功時のみUI反映（失敗時は何もしない）
            if (success) {
                onDraw();
            }
        }
    }
    void onButtonC() override {
        if (!inputLogic || !view) return;
        
        // 部分的な入力状態を直接取得
        const int* digits = inputLogic->getDigits();
        const bool* entered = inputLogic->getEntered();
        
        extern std::vector<time_t> alarmTimes;
        time_t now = time(nullptr);
        AlarmLogic::AddAlarmResult result;
        std::string msg;
        
        // 新しいAPIを使用して部分的な入力状態からアラーム追加
        bool ok = AlarmLogic::addAlarmFromPartialInput(alarmTimes, now, digits, entered, result, msg);
        
        if (ok) {
            if (manager && mainDisplayState) {
                manager->setState(mainDisplayState);
            }
        } else {
            // エラーメッセージを英語でプレビュー表示
            view->showPreview(msg.c_str());
            // TODO: 効果音/バイブ（ハード依存部で実装）
        }
    }
    void onButtonALongPress() override {
        if (inputLogic) {
            inputLogic->incrementInput(5);
        }
        onDraw();
    }
    void onButtonBLongPress() override {
        if (inputLogic) {
            // リセット: 入力値を空 (__:__) に戻し、カーソル位置を3に戻す
            inputLogic->reset();
            onDraw();
        }
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