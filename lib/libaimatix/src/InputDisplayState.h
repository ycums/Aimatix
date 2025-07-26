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
        : inputLogic(logic), view(view), manager(nullptr), mainDisplayState(nullptr), isRelativeMode(false) {
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
        // プレビュー表示
        int value = inputLogic ? inputLogic->getValue() : InputLogic::EMPTY_VALUE;
        if (view) {
            char preview[32] = "";
            if (value != InputLogic::EMPTY_VALUE) {
                if (isRelativeMode) {
                    // 相対値入力モードの場合は相対値計算結果を表示
                    calculateRelativeTimePreview(value, preview, sizeof(preview));
                } else {
                    // 絶対時刻入力モードの場合は従来通り
                    snprintf(preview, sizeof(preview), "プレビュー: %02d:%02d", value/100, value%100);
                }
            }
            view->showPreview(preview);
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
        if (!inputLogic || !view) return;
        
        // 部分的な入力状態を直接取得
        const int* digits = inputLogic->getDigits();
        const bool* entered = inputLogic->getEntered();
        
        extern std::vector<time_t> alarm_times;
        time_t now = time(nullptr);
        AlarmLogic::AddAlarmResult result;
        std::string msg;
        
        // 相対値入力モードの場合は相対値計算結果をアラームとして追加
        bool ok;
        if (isRelativeMode) {
            extern std::vector<time_t> alarm_times;
            ok = addRelativeAlarm(digits, entered, alarm_times, result, msg);
        } else {
            // 絶対時刻入力モードの場合は従来通り
            extern std::vector<time_t> alarm_times;
            ok = AlarmLogic::addAlarmFromPartialInput(alarm_times, now, digits, entered, result, msg);
        }
        
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
    
    // 相対値計算のプレビュー表示
    void calculateRelativeTimePreview(int inputValue, char* preview, size_t previewSize) {
        // 現在時刻取得
        time_t now = time(nullptr);
        struct tm* tm_now = localtime(&now);
        
        // 入力値を時分に分解
        int inputHour = inputValue / 100;
        int inputMinute = inputValue % 100;
        
        // 現在時刻 + 入力値
        int resultHour = tm_now->tm_hour + inputHour;
        int resultMinute = tm_now->tm_min + inputMinute;
        
        // 繰り上げ処理
        if (resultMinute >= 60) {
            resultHour += resultMinute / 60;
            resultMinute %= 60;
        }
        
        // 日付跨ぎ処理
        bool nextDay = false;
        if (resultHour >= 24) {
            resultHour -= 24;
            nextDay = true;
        }
        
        // プレビュー文字列生成
        if (nextDay) {
            snprintf(preview, previewSize, "+1d %02d:%02d", resultHour, resultMinute);
        } else {
            snprintf(preview, previewSize, "%02d:%02d", resultHour, resultMinute);
        }
    }
    
    // 相対値アラーム追加
    bool addRelativeAlarm(const int* digits, const bool* entered, std::vector<time_t>& alarm_times, AlarmLogic::AddAlarmResult& result, std::string& msg) {
        // 入力値チェック
        if (!digits || !entered) {
            result = AlarmLogic::AddAlarmResult::ErrorInvalid;
            msg = "Invalid input data";
            return false;
        }
        
        // 部分的な入力状態を完全な時分に変換
        int hour = 0, minute = 0;
        
        // 時の解釈（digits[0], digits[1]）
        if (entered[0] && entered[1]) {
            // 両方入力済み
            hour = digits[0] * 10 + digits[1];
        } else if (entered[0] && !entered[1]) {
            // 時十桁のみ入力済み → 時一桁を0として補完
            hour = digits[0] * 10 + 0;
        } else if (!entered[0] && entered[1]) {
            // 時一桁のみ入力済み → 時十桁を0として補完
            hour = 0 * 10 + digits[1];
        } else {
            // 時が未入力
            hour = 0;
        }
        
        // 分の解釈（digits[2], digits[3]）
        if (entered[2] && entered[3]) {
            // 両方入力済み
            minute = digits[2] * 10 + digits[3];
        } else if (entered[2] && !entered[3]) {
            // 分十桁のみ入力済み → 分一桁を0として補完
            minute = digits[2] * 10 + 0;
        } else if (!entered[2] && entered[3]) {
            // 分一桁のみ入力済み → 分十桁を0として補完
            minute = 0 * 10 + digits[3];
        } else {
            // 分が未入力
            minute = 0;
        }
        
        // 未入力チェック
        if (hour == 0 && minute == 0) {
            result = AlarmLogic::AddAlarmResult::ErrorEmptyInput;
            msg = "Input is empty.";
            return false;
        }
        
        // 相対値計算（現在時刻 + 入力値）
        time_t now = time(nullptr);
        struct tm* tm_now = localtime(&now);
        struct tm alarm_tm = *tm_now;
        alarm_tm.tm_sec = 0;
        alarm_tm.tm_isdst = -1;
        
        // 分繰り上げ
        if (minute >= 60) { 
            hour += minute / 60; 
            minute = minute % 60; 
        }
        // 時繰り上げ
        int add_day = hour / 24;
        hour = hour % 24;
        
        // 現在時刻に加算
        alarm_tm.tm_hour += hour;
        alarm_tm.tm_min += minute;
        alarm_tm.tm_mday += add_day;
        
        // 時刻正規化
        time_t alarm_time = mktime(&alarm_tm);
        
        // アラーム追加
        return AlarmLogic::addAlarm(alarm_times, now, alarm_time, result, msg);
    }
}; 