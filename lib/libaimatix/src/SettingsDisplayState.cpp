#include "SettingsDisplayState.h"
#include <cassert>

void SettingsDisplayState::onEnter() {
    // ちらつき防止用の状態をリセット
    lastDisplayedItems.clear();
    lastSelectedIndex = -1;
    
    if (view) {
        view->clear();
        view->showTitle("SETTINGS", 42, false);
        view->showHints("UP", "DOWN", "SELECT");
    }
    onDraw();
}

void SettingsDisplayState::onExit() {
    // 必要に応じてクリーンアップ処理
}

void SettingsDisplayState::onDraw() {
    if (!view || !settingsLogic) {
        return;
    }
    
    std::vector<std::string> settingsList = generateSettingsList();
    int selectedIndex = settingsLogic->getIndexByItem(settingsLogic->getSelectedItem());
    
    // ちらつき防止：変更があった場合のみ更新
    bool needsUpdate = false;
    
    // 初期表示時は強制的に更新
    if (lastDisplayedItems.empty()) {
        needsUpdate = true;
    } else {
        // 選択位置の変更をチェック
        if (selectedIndex != lastSelectedIndex) {
            needsUpdate = true;
        }
    }
    
    // 変更がない場合は何もしない（ちらつき防止）
    if (!needsUpdate) {
        return;
    }
    
    // 変更があった場合のみ画面を更新
    view->showSettingsList(settingsList, selectedIndex);
    
    // 現在の状態を記憶
    lastDisplayedItems = settingsList;
    lastSelectedIndex = selectedIndex;
}

void SettingsDisplayState::onButtonA() {
    if (!settingsLogic) return;
    
    // 前項目（上に移動）- 端で停止
    int currentIndex = settingsLogic->getIndexByItem(settingsLogic->getSelectedItem());
    if (currentIndex > 0) {
        int newIndex = currentIndex - 1;
        settingsLogic->setSelectedItem(settingsLogic->getItemByIndex(newIndex));
        onDraw();
    }
}

void SettingsDisplayState::onButtonB() {
    if (!settingsLogic) return;
    
    // 次項目（下に移動）- 端で停止
    int currentIndex = settingsLogic->getIndexByItem(settingsLogic->getSelectedItem());
    if (currentIndex < settingsLogic->getItemCount() - 1) {
        int newIndex = currentIndex + 1;
        settingsLogic->setSelectedItem(settingsLogic->getItemByIndex(newIndex));
        onDraw();
    }
}

void SettingsDisplayState::onButtonC() {
    if (!settingsLogic) return;
    
    // 選択確定（値変更モードに入る）
    // 3-0-12フェーズでは値変更モードは実装しない
    // 後続フェーズで実装予定
    onDraw();
}

void SettingsDisplayState::onButtonALongPress() {
    // 一番上に移動
    if (settingsLogic) {
        settingsLogic->setSelectedItem(SettingsItem::SOUND);
        onDraw();
    }
}

void SettingsDisplayState::onButtonBLongPress() {
    // 一番下に移動
    if (settingsLogic) {
        settingsLogic->setSelectedItem(SettingsItem::INFO);
        onDraw();
    }
}

void SettingsDisplayState::onButtonCLongPress() {
    // メイン画面に戻る
    if (manager && mainDisplayState) {
        manager->setState(mainDisplayState);
    }
}

int SettingsDisplayState::getSelectedIndex() const {
    if (!settingsLogic) return 0;
    return settingsLogic->getIndexByItem(settingsLogic->getSelectedItem());
}

void SettingsDisplayState::setSelectedIndex(int index) {
    if (!settingsLogic) return;
    if (index >= 0 && index < settingsLogic->getItemCount()) {
        settingsLogic->setSelectedItem(settingsLogic->getItemByIndex(index));
    }
}

std::vector<std::string> SettingsDisplayState::generateSettingsList() const {
    std::vector<std::string> list;
    if (!settingsLogic) {
        return list;
    }
    
    for (int i = 0; i < settingsLogic->getItemCount(); ++i) {
        SettingsItem item = settingsLogic->getItemByIndex(i);
        std::string displayName = settingsLogic->getItemDisplayName(item);
        std::string valueString = settingsLogic->getItemValueString(item);
        
        if (!valueString.empty()) {
            list.push_back(displayName + ": " + valueString);
        } else {
            list.push_back(displayName);
        }
    }
    
    return list;
} 