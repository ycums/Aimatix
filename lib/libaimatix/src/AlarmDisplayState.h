#pragma once
#include "StateManager.h"
#include "AlarmLogic.h"
#include "IAlarmDisplayView.h"
#include "ITimeService.h"
#include <vector>
#include <string>
#include <ctime>
#include <memory>

class AlarmDisplayState : public IState {
public:
    AlarmDisplayState(StateManager* mgr, IAlarmDisplayView* view = nullptr, 
                     std::shared_ptr<ITimeService> timeService = nullptr)
        : manager(mgr), view(view), timeService(timeService), selectedIndex(0), mainDisplayState(nullptr), 
          lastUserAction(0), lastSelectedIndex(0) {}
    
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
    
    void setView(IAlarmDisplayView* v) { view = v; }
    void setTimeService(std::shared_ptr<ITimeService> s) { timeService = s; }
    
    // テスト用のアクセサ
    size_t getSelectedIndex() const { return selectedIndex; }
    void setSelectedIndex(size_t index) { selectedIndex = index; }
    
private:
    StateManager* manager;
    IAlarmDisplayView* view;
    std::shared_ptr<ITimeService> timeService;
    IState* mainDisplayState;
    size_t selectedIndex;
    unsigned long lastUserAction;
    
    // ちらつき防止用：前回の表示状態を記憶
    std::vector<time_t> lastDisplayedAlarms;
    size_t lastSelectedIndex;
    
    // ハイブリッドアプローチ用の定数
    static constexpr unsigned long UPDATE_PAUSE_DURATION = 3000; // 3秒
    
    // アラームリストを取得（外部変数から）
    std::vector<time_t> getAlarmList() const;
    
    // 選択位置の調整
    void adjustSelectionIndex();
    
    // アラーム削除処理（valueベース）
    void deleteSelectedAlarm();
    
    // ナビゲーション処理
    void moveUp();
    void moveDown();
    void moveToTop();
    void moveToBottom();
    
    // ハイブリッドアプローチ用のヘルパー
    bool shouldUpdateRealTime() const;
    void updateLastUserAction();
    
    // 静的メソッド
    static unsigned long getCurrentMillis(const std::shared_ptr<ITimeService>& timeService);
    
    // 強制描画（初期表示とリアルタイム更新用）
    void forceDraw();
}; 