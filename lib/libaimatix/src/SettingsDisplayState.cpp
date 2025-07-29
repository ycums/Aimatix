#include "SettingsDisplayState.h"
#include <cassert>
#include <vector>
#include <string>

void SettingsDisplayState::onEnter() {
    // ちらつき防止用の状態をリセット
    lastDisplayedItems.clear();
    lastSelectedIndex = -1;
    
    if (view != nullptr) {
        view->clear();
        constexpr int BATTERY_WARNING_THRESHOLD = 42;
        view->showTitle("SETTINGS", BATTERY_WARNING_THRESHOLD, false);
        view->showHints("UP", "DOWN", "SELECT");
    }
    onDraw();
}

void SettingsDisplayState::onExit() {
    // 必要に応じてクリーンアップ処理
}

void SettingsDisplayState::onDraw() {
    if (view == nullptr || settingsLogic == nullptr) {
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
    if (settingsLogic == nullptr) {
        return;
    }
    
    // 前項目（上に移動）- 端で停止
    const int currentIndex = settingsLogic->getIndexByItem(settingsLogic->getSelectedItem());
    if (currentIndex > 0) {
        const int newIndex = currentIndex - 1;
        settingsLogic->setSelectedItem(settingsLogic->getItemByIndex(newIndex));
        onDraw();
    }
}

void SettingsDisplayState::onButtonB() {
    if (settingsLogic == nullptr) {
        return;
    }
    
    // 次項目（下に移動）- 端で停止
    const int currentIndex = settingsLogic->getIndexByItem(settingsLogic->getSelectedItem());
    if (currentIndex < settingsLogic->getItemCount() - 1) {
        const int newIndex = currentIndex + 1;
        settingsLogic->setSelectedItem(settingsLogic->getItemByIndex(newIndex));
        onDraw();
    }
}

void SettingsDisplayState::onButtonC() {
    if (settingsLogic == nullptr) {
        return;
    }
    
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

auto SettingsDisplayState::getSelectedIndex() const -> int {
    if (settingsLogic == nullptr) {
        return 0;
    }
    return settingsLogic->getIndexByItem(settingsLogic->getSelectedItem());
}

void SettingsDisplayState::setSelectedIndex(int index) {
    if (settingsLogic == nullptr) {
        return;
    }
    if (index >= 0 && index < settingsLogic->getItemCount()) {
        settingsLogic->setSelectedItem(settingsLogic->getItemByIndex(index));
    }
}

auto SettingsDisplayState::generateSettingsList() const -> std::vector<std::string> {
    std::vector<std::string> list;
    if (settingsLogic == nullptr) {
        return list;
    }
    
    for (int i = 0; i < settingsLogic->getItemCount(); ++i) {
        const SettingsItem item = settingsLogic->getItemByIndex(i);
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