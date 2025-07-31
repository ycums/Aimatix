#pragma once
#include "StateManager.h"
#include "InputLogic.h"
#include "IInputDisplayView.h"
#include "PartialInputLogic.h"
#include <stdio.h>
#include <vector>
#include "AlarmLogic.h"
#include <cstring>

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
class InputDisplayState : public IState {
public:
    // InputDisplayStateのコンストラクタ
    InputDisplayState(InputLogic* logic = nullptr, IInputDisplayView* view = nullptr)
        : inputLogic(logic), view(view), manager(nullptr), mainDisplayState(nullptr), isRelativeMode(false), 
          errorMessage(""), showError(false), errorStartTime(0) {
        for (int i = 0; i < 4; ++i) { lastDigits[i] = -1; lastEntered[i] = false; }
    }
    void onEnter() override {
        if (inputLogic) inputLogic->reset();
        if (view) {
            view->clear();
            // 相対値入力モードの場合はタイトルを変更
            const char* title = isRelativeMode ? "REL+" : "INPUT";
            view->showTitle(title, 42, false);
            view->showHints("INC", "NEXT", "SET");
            for (int i = 0; i < 4; ++i) { lastDigits[i] = -1; lastEntered[i] = false; }
        }
        // エラー状態をリセット
        showError = false;
        errorMessage = "";
        errorStartTime = 0;
    }
    void onExit() override {}
    // 相対値計算結果のプレビュー文字列を生成
    void generateRelativePreview(char* preview, size_t previewSize) {
        time_t relativeTime = inputLogic->getAbsoluteValue();
        if (relativeTime != -1) {
            struct tm result_tm;
            struct tm now_tm;
            struct tm* result_tm_ptr = localtime(&relativeTime);
            result_tm = *result_tm_ptr;
            time_t now = time(nullptr);
            struct tm* now_tm_ptr = localtime(&now);
            now_tm = *now_tm_ptr;
            
            // 日付跨ぎ判定
            bool nextDay = (result_tm.tm_mday != now_tm.tm_mday);
            
            // プレビュー文字列生成
            if (nextDay) {
                snprintf(preview, previewSize, "+1d %02d:%02d", result_tm.tm_hour, result_tm.tm_min);
            } else {
                snprintf(preview, previewSize, "%02d:%02d", result_tm.tm_hour, result_tm.tm_min);
            }
        }
    }

    void onDraw() override {
        if (inputLogic && view) {
            const int* digits = inputLogic->getDigits();
            const bool* entered = inputLogic->getEntered();
            bool needsUpdate = false;
            
            // 入力値の変化をチェック
            for (int i = 0; i < 4; ++i) {
                if (digits[i] != lastDigits[i] || entered[i] != lastEntered[i]) {
                    needsUpdate = true;
                    break;
                }
            }
            
            // 入力値が変化した場合のみ更新
            if (needsUpdate) {
                for (int i = 0; i < 4; ++i) {
                    view->showDigit(i, digits[i], entered[i]);
                    lastDigits[i] = digits[i];
                    lastEntered[i] = entered[i];
                }
            }
        }
        
        // プレビュー表示 - 入力値が変更されたら即座に更新
        int value = inputLogic ? inputLogic->getValue() : InputLogic::EMPTY_VALUE;
        if (view) {
            char preview[32] = "";
            
            // エラーメッセージ表示の管理
            if (showError) {
                // エラーメッセージを3秒間表示
                time_t currentTime = time(nullptr);
                if (currentTime - errorStartTime >= 3) {
                    showError = false;
                    errorMessage = "";
                } else {
                    strncpy(preview, errorMessage.c_str(), sizeof(preview) - 1);
                    preview[sizeof(preview) - 1] = '\0';
                }
            } else if (value != InputLogic::EMPTY_VALUE) {
                // 完全入力時のプレビュー表示（絶対値・相対値共通）
                if (isRelativeMode) {
                    generateRelativePreview(preview, sizeof(preview));
                } else {
                    // 絶対時刻入力モードの場合は従来通り
                    snprintf(preview, sizeof(preview), "%02d:%02d", value/100, value%100);
                }
            } else {
                // 部分入力時のプレビュー表示（絶対値・相対値共通）
                const int* digits = inputLogic ? inputLogic->getDigits() : nullptr;
                const bool* entered = inputLogic ? inputLogic->getEntered() : nullptr;
                if (digits && entered) {
                    // PartialInputLogicを使用して確定処理と同じロジックでプレビュー生成
                    auto parsedTime = PartialInputLogic::parsePartialInput(digits, entered);
                    bool hasInput = false;
                    for (int i = 0; i < 4; ++i) {
                        if (entered[i]) {
                            hasInput = true;
                            break;
                        }
                    }
                    if (hasInput && parsedTime.isValid) {
                        if (isRelativeMode) {
                            generateRelativePreview(preview, sizeof(preview));
                        } else {
                            // 絶対時刻入力モードの場合：確定処理と同じロジックで表示
                            std::string timeStr = PartialInputLogic::formatTime(parsedTime.hour, parsedTime.minute);
                            strncpy(preview, timeStr.c_str(), sizeof(preview) - 1);
                            preview[sizeof(preview) - 1] = '\0';
                        }
                    }
                }
            }
            
            // プレビュー内容の変化チェック
            std::string currentPreview(preview);
            if (currentPreview != lastPreview) {
                view->showPreview(preview);
                lastPreview = currentPreview;
            }
            view->showColon();
        }
#ifndef ARDUINO
        printf("[InputDisplay] value=%d, relativeMode=%s\n", value, isRelativeMode ? "true" : "false");
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
        if (!inputLogic) return;
        
        bool success = false;
        bool error = false;
        
        if (isRelativeMode) {
            // 相対値入力モード: InputLogicから相対値を取得してアラーム追加
            time_t relativeTime = inputLogic->getAbsoluteValue();
            if (relativeTime != -1) {
                extern std::vector<time_t> alarm_times;
                AlarmLogic::AddAlarmResult result;
                std::string msg;
                
                // 絶対時刻としてアラーム追加
                success = AlarmLogic::addAlarmAtTime(alarm_times, relativeTime, result, msg);
                if (!success) {
                    error = true;
                    showError = true;
                    errorMessage = msg;
                    errorStartTime = time(nullptr);
                }
            } else {
                error = true;
                showError = true;
                errorMessage = "Input is empty.";
                errorStartTime = time(nullptr);
            }
        } else {
            // 絶対時刻入力モード: 既存のロジックを使用
            extern std::vector<time_t> alarm_times;
            const int* digits = inputLogic->getDigits();
            const bool* entered = inputLogic->getEntered();
            if (digits && entered) {
                time_t now = time(nullptr);
                AlarmLogic::AddAlarmResult result;
                std::string msg;
                bool ok = AlarmLogic::addAlarmFromPartialInput(alarm_times, now, digits, entered, result, msg);
                if (ok) {
                    success = true;
                } else {
                    error = true;
                    showError = true;
                    errorMessage = msg;
                    errorStartTime = time(nullptr);
                }
            }
        }
        
        if (success && !error) {
            if (manager && mainDisplayState) {
                manager->setState(mainDisplayState);
            }
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
    
    // 相対値入力モードの設定
    void setRelativeMode(bool relative) { isRelativeMode = relative; }
    bool getRelativeMode() const { return isRelativeMode; }
    
private:
    InputLogic* inputLogic;
    IInputDisplayView* view;
    int lastDigits[4] = {-1,-1,-1,-1};
    bool lastEntered[4] = {false,false,false,false};
    StateManager* manager;
    IState* mainDisplayState;
    bool isRelativeMode;
    
    // エラーメッセージ表示管理
    std::string errorMessage;
    bool showError;
    time_t errorStartTime;
    
    // プレビュー内容の変化チェック用
    std::string lastPreview;

}; 