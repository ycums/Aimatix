#pragma once
#include "StateManager.h"
#include "SettingsLogic.h"
#include "ISettingsDisplayView.h"
#include <vector>
#include <string>
#include <memory>

class SettingsDisplayState : public IState {
public:
    SettingsDisplayState(ISettingsLogic* logic = nullptr, ISettingsDisplayView* view = nullptr)
        : settingsLogic(logic), view(view), manager(nullptr), mainDisplayState(nullptr) {}
    
    void onEnter() override;
    void onExit() override;
    void onDraw() override;
    void onButtonA() override;
    void onButtonB() override;
    void onButtonC() override;
    void onButtonALongPress() override;
    void onButtonBLongPress() override;
    void onButtonCLongPress() override;
    
    void setView(ISettingsDisplayView* v) { view = v; }
    void setManager(StateManager* m) { manager = m; }
    void setMainDisplayState(IState* mainState) { mainDisplayState = mainState; }
    void setSettingsLogic(ISettingsLogic* logic) { settingsLogic = logic; }
    void setDateTimeInputState(IState* datetimeState) { datetimeInputState = datetimeState; }
    void setTimeSyncDisplayState(IState* timeSyncState) { timeSyncDisplayState = timeSyncState; }
    
private:
    ISettingsLogic* settingsLogic;
    ISettingsDisplayView* view;
    StateManager* manager;
    IState* mainDisplayState;
    IState* datetimeInputState;
    IState* timeSyncDisplayState{nullptr};
    
    // ちらつき防止用の状態記憶
    std::vector<std::string> lastDisplayedItems;
    int lastSelectedIndex = -1;
    
    // 表示用の設定項目リストを生成
    std::vector<std::string> generateSettingsList() const;
}; 