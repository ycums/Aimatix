#pragma once
#include "StateManager.h"
#include "InputDisplayState.h"
#include "IMainDisplayView.h"
#include "TimeLogic.h"
#include "AlarmLogic.h"
#include <vector>
#include <string>
#include <ctime>

class MainDisplayState : public IState {
public:
    MainDisplayState(StateManager* mgr, InputDisplayState* inputState, IMainDisplayView* view = nullptr, TimeLogic* timeLogic = nullptr, AlarmLogic* alarmLogic = nullptr)
        : manager(mgr), inputDisplayState(inputState), view(view), timeLogic(timeLogic), alarmLogic(alarmLogic) {}
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
        extern std::vector<time_t> alarmTimes;
        AlarmLogic::removePastAlarms(alarmTimes, now);
        // --- 残り時間・進捗計算 ---
        int remainSec = AlarmLogic::getRemainSec(alarmTimes, now);
        static time_t lastAlarmStart = 0;
        static int lastAlarmTotalSec = 0;
        static time_t prevNextAlarm = 0;
        time_t nextAlarm = (!alarmTimes.empty()) ? alarmTimes.front() : 0;
        if (nextAlarm != prevNextAlarm) {
            lastAlarmStart = now;
            lastAlarmTotalSec = remainSec;
            prevNextAlarm = nextAlarm;
        }
        int totalSec = lastAlarmTotalSec > 0 ? lastAlarmTotalSec : 1;
        int progressPercent = AlarmLogic::getRemainPercent(remainSec, totalSec);
        char remainTime[16];
        snprintf(remainTime, sizeof(remainTime), "%02d:%02d:%02d", remainSec/3600, (remainSec/60)%60, remainSec%60);
        if (alarmTimes.empty()) {
            snprintf(remainTime, sizeof(remainTime), "00:00:00");
            progressPercent = 0;
        }
        view->showRemain(remainTime);
        view->showProgress(progressPercent);
        // --- アラームリスト ---
        std::vector<std::string> alarmStrs;
        AlarmLogic::getAlarmTimeStrings(alarmTimes, alarmStrs);
        view->showAlarmList(alarmStrs);
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
    void setTimeLogic(TimeLogic* t) { timeLogic = t; }
    void setAlarmLogic(AlarmLogic* a) { alarmLogic = a; }
private:
    StateManager* manager;
    InputDisplayState* inputDisplayState;
    IMainDisplayView* view;
    TimeLogic* timeLogic;
    AlarmLogic* alarmLogic;
}; 