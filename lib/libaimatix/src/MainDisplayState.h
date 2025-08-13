#pragma once
#include "StateManager.h"
#include "InputDisplayState.h"
#include "IMainDisplayView.h"
#include "TimeLogic.h"
#include "AlarmLogic.h"
#include <vector>
#include <string>
#include <ctime>
#include "AlarmRolloverDetector.h"

class MainDisplayState : public IState {
public:
    MainDisplayState(StateManager* mgr, InputDisplayState* inputState, IMainDisplayView* view = nullptr, TimeLogic* timeLogic = nullptr, AlarmLogic* alarmLogic = nullptr)
        : manager(mgr), inputDisplayState(inputState), view(view), timeLogic(timeLogic), alarmLogic(alarmLogic) {}
    
    void setAlarmDisplayState(IState* alarmState) { alarmDisplayState = alarmState; }
    void setSettingsDisplayState(IState* settingsState) { settingsDisplayState = settingsState; }
    void setAlarmActiveState(IState* ringingState) { alarmActiveState = ringingState; }
    void onEnter() override {
        if (view) {
            view->clear();
            view->showTitle("MAIN", 42, false);
            view->showHints("ABS", "REL+", "MGMT");
        }
    }
    void onExit() override {}
    void onDraw() override {
        if (!view || !timeLogic || !alarmLogic) return;
        // --- 現在時刻取得 ---
        time_t now = time(nullptr);
        struct tm* tm_now = localtime(&now);
        char currentTime[16];
        snprintf(currentTime, sizeof(currentTime), "%02d:%02d", tm_now->tm_hour, tm_now->tm_min);
        view->showTime(currentTime);
        // --- アラームリストの消化 ---
        extern std::vector<time_t> alarm_times;
        AlarmLogic::removePastAlarms(alarm_times, now);
        // 先頭アラーム消化の検出（remove後のfront差し替わりで判定）
        if (manager && alarmActiveState) {
            if (rolloverDetector.onFrame(alarm_times, now)) {
                manager->setState(alarmActiveState);
                return; // 次フレームで描画は鳴動状態に委譲
            }
        }
        // --- 残り時間・進捗計算 ---
        int remainSec = AlarmLogic::getRemainSec(alarm_times, now);
        static time_t lastAlarmStart = 0;
        static int lastAlarmTotalSec = 0;
        static time_t prevNextAlarm = 0;
        time_t nextAlarm = (!alarm_times.empty()) ? alarm_times.front() : 0;
        if (nextAlarm != prevNextAlarm) {
            lastAlarmStart = now;
            lastAlarmTotalSec = remainSec;
            prevNextAlarm = nextAlarm;
        }
        int totalSec = lastAlarmTotalSec > 0 ? lastAlarmTotalSec : 1;
        int progressPercent = AlarmLogic::getRemainPercent(remainSec, totalSec);
        char remainTime[16];
        snprintf(remainTime, sizeof(remainTime), "%02d:%02d:%02d", remainSec/3600, (remainSec/60)%60, remainSec%60);
        if (alarm_times.empty()) {
            snprintf(remainTime, sizeof(remainTime), "00:00:00");
            progressPercent = 0;
        }
        view->showRemain(remainTime);
        view->showProgress(progressPercent);
        // --- アラームリスト ---
        std::vector<std::string> alarmStrs;
        AlarmLogic::getAlarmTimeStrings(alarm_times, alarmStrs);
        view->showAlarmList(alarmStrs);
    }
    void onButtonA() override {
        if (manager && inputDisplayState) {
            // 絶対時刻入力モードで遷移
            inputDisplayState->setRelativeMode(false);
            manager->setState(inputDisplayState);
        }
    }
    void onButtonB() override {
        if (manager && inputDisplayState) {
            // 相対値入力モードで遷移
            inputDisplayState->setRelativeMode(true);
            manager->setState(inputDisplayState);
        }
    }
    void onButtonC() override {
        if (manager && alarmDisplayState) {
            // アラーム管理画面に遷移
            manager->setState(alarmDisplayState);
        }
    }
    void onButtonALongPress() override {}
    void onButtonCLongPress() override {
        if (manager && settingsDisplayState) {
            // 設定画面に遷移
            manager->setState(settingsDisplayState);
        }
    }
    void onButtonBLongPress() override {}
    void setView(IMainDisplayView* v) { view = v; }
    void setTimeLogic(TimeLogic* t) { timeLogic = t; }
    void setAlarmLogic(AlarmLogic* a) { alarmLogic = a; }
private:
    StateManager* manager;
    InputDisplayState* inputDisplayState;
    IState* alarmDisplayState;
    IState* settingsDisplayState;
        IState* alarmActiveState = nullptr;
    IMainDisplayView* view;
    TimeLogic* timeLogic;
    AlarmLogic* alarmLogic;
        AlarmRolloverDetector rolloverDetector;
}; 