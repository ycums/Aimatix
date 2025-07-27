#pragma once
#include "StateManager.h"
#include "AlarmLogic.h"
#include "IDisplay.h"
#include <vector>
#include <string>
#include <ctime>

class AlarmDisplayState : public IState {
public:
    AlarmDisplayState(StateManager* mgr, IDisplay* display = nullptr, AlarmLogic* alarmLogic = nullptr)
        : manager(mgr), display(display), alarmLogic(alarmLogic), selectedIndex(0), mainDisplayState(nullptr) {}
    
    void setMainDisplayState(IState* mainState) { mainDisplayState = mainState; }
    
    void onEnter() override;
    void onExit() override;
    void onDraw() override;
    void onButtonA() override;
    void onButtonB() override;
    void onButtonC() override;
    void onButtonALongPress() override;
    void onButtonBLongPress() override;
    void onButtonCLongPress() override;
    
    void setDisplay(IDisplay* d) { display = d; }
    void setAlarmLogic(AlarmLogic* a) { alarmLogic = a; }
    
private:
    StateManager* manager;
    IDisplay* display;
    AlarmLogic* alarmLogic;
    IState* mainDisplayState;
    size_t selectedIndex;
    
    // アラームリストを取得（外部変数から）
    std::vector<time_t> getAlarmList() const;
    
    // 選択位置の調整
    void adjustSelectionIndex();
    
    // アラーム削除処理
    void deleteSelectedAlarm();
    
    // ナビゲーション処理
    void moveUp();
    void moveDown();
    void moveToTop();
    void moveToBottom();
}; 