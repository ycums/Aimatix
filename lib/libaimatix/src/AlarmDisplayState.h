#pragma once
#include "StateManager.h"
#include "AlarmLogic.h"
#include "IAlarmDisplayView.h"
#include "ITimeProvider.h"
#include "ITimeManager.h"
#include <vector>
#include <string>
#include <ctime>
#include <memory>

class AlarmDisplayState : public IState {
public:
    AlarmDisplayState(StateManager* mgr, IAlarmDisplayView* view = nullptr, 
                     std::shared_ptr<ITimeProvider> timeProvider = nullptr,
                     std::shared_ptr<ITimeManager> timeManager = nullptr)
        : manager(mgr), view(view), timeProvider(timeProvider), 
          timeManager(timeManager), selectedIndex(0), mainDisplayState(nullptr), 
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
    void setTimeProvider(std::shared_ptr<ITimeProvider> tp) { timeProvider = tp; }
    void setTimeManager(std::shared_ptr<ITimeManager> tm) { timeManager = tm; }
    
    // テスト用のアクセサ
    size_t getSelectedIndex() const { return selectedIndex; }
    void setSelectedIndex(size_t index) { selectedIndex = index; }
    
private:
    StateManager* manager;
    IAlarmDisplayView* view;
    std::shared_ptr<ITimeProvider> timeProvider;
    std::shared_ptr<ITimeManager> timeManager;
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
    unsigned long getCurrentMillis() const;
    
    // 強制描画（初期表示とリアルタイム更新用）
    void forceDraw();
}; 