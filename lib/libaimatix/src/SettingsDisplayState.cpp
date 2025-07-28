#include "SettingsDisplayState.h"
#include <cassert>

void SettingsDisplayState::onEnter() {
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
    if (!view || !settingsLogic) return;
    
    std::vector<std::string> settingsList = generateSettingsList();
    int selectedIndex = settingsLogic->getIndexByItem(settingsLogic->getSelectedItem());
    view->showSettingsList(settingsList, selectedIndex);
}

void SettingsDisplayState::onButtonA() {
    if (!settingsLogic) return;
    
    // 前項目（上に移動）
    int currentIndex = settingsLogic->getIndexByItem(settingsLogic->getSelectedItem());
    int newIndex = (currentIndex - 1 + settingsLogic->getItemCount()) % settingsLogic->getItemCount();
    settingsLogic->setSelectedItem(settingsLogic->getItemByIndex(newIndex));
    onDraw();
}

void SettingsDisplayState::onButtonB() {
    if (!settingsLogic) return;
    
    // 次項目（下に移動）
    int currentIndex = settingsLogic->getIndexByItem(settingsLogic->getSelectedItem());
    int newIndex = (currentIndex + 1) % settingsLogic->getItemCount();
    settingsLogic->setSelectedItem(settingsLogic->getItemByIndex(newIndex));
    onDraw();
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
    if (!settingsLogic) return list;
    
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