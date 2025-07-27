#include "AlarmDisplayState.h"
#include "ui_constants.h"
#include <cstdio>
#include <cstring>
#include <algorithm>

// 外部変数（既存のアラームリスト）
extern std::vector<time_t> alarm_times;

void AlarmDisplayState::onEnter() {
    if (view == nullptr) {
        return;
    }
    
    view->clear();
    view->showTitle("ALARMS", BATTERY_WARNING_THRESHOLD, false);
    view->showHints("UP", "DOWN", "DEL");
    forceDraw();
}

void AlarmDisplayState::onExit() {
    // クリーンアップ処理（必要に応じて）
}

void AlarmDisplayState::onDraw() {
    if (view == nullptr) {
        return;
    }
    
    // ハイブリッドアプローチ: リアルタイム更新とユーザー操作の両立
    if (shouldUpdateRealTime()) {
        forceDraw();
    }
}

void AlarmDisplayState::forceDraw() {
    if (view == nullptr) {
        return;
    }
    
    // リアルタイム削除: 過去のアラームを削除
    time_t now = 0;
    if (timeProvider != nullptr) {
        now = timeProvider->now();
        AlarmLogic::removePastAlarms(alarm_times, now);
    }
    
    // アラームリストを取得（毎回最新の状態を取得）
    const std::vector<time_t> alarms = getAlarmList();
    
    // 選択位置の調整（アラーム消化後も適切に調整）
    adjustSelectionIndex();
    
    // ちらつき防止：変更があった場合のみ更新
    bool needsUpdate = false;
    
    // 初期表示時は強制的に更新
    if (lastDisplayedAlarms.empty()) {
        needsUpdate = true;
    } else {
        // アラームリストの変更をチェック
        if (alarms != lastDisplayedAlarms) {
            needsUpdate = true;
        }
        
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
    if (alarms.empty()) {
        // 空リスト表示
        view->showNoAlarms();
    } else {
        // アラームリスト表示
        view->showAlarmList(alarms, selectedIndex);
    }
    
    // 現在の状態を記憶
    lastDisplayedAlarms = alarms;
    lastSelectedIndex = selectedIndex;
}

void AlarmDisplayState::onButtonA() {
    updateLastUserAction();
    moveUp();
    // 選択位置変更後に画面を再描画
    forceDraw();
}

void AlarmDisplayState::onButtonB() {
    updateLastUserAction();
    moveDown();
    // 選択位置変更後に画面を再描画
    forceDraw();
}

void AlarmDisplayState::onButtonC() {
    updateLastUserAction();
    deleteSelectedAlarm();
}

void AlarmDisplayState::onButtonALongPress() {
    updateLastUserAction();
    moveToTop();
    // 選択位置変更後に画面を再描画
    forceDraw();
}

void AlarmDisplayState::onButtonBLongPress() {
    updateLastUserAction();
    moveToBottom();
    // 選択位置変更後に画面を再描画
    forceDraw();
}

void AlarmDisplayState::onButtonCLongPress() {
    updateLastUserAction();
    // メイン画面に戻る
    if (manager != nullptr && mainDisplayState != nullptr) {
        manager->setState(mainDisplayState);
    }
}

auto AlarmDisplayState::getAlarmList() const -> std::vector<time_t> {
    // 外部変数からアラームリストを取得
    return AlarmLogic::getAlarms(alarm_times);
}

auto AlarmDisplayState::adjustSelectionIndex() -> void {
    const std::vector<time_t> alarms = getAlarmList();
    if (alarms.empty()) {
        selectedIndex = 0;
        return;
    }
    
    // 選択位置が範囲外の場合は調整
    if (selectedIndex >= alarms.size()) {
        selectedIndex = alarms.size() - 1;
    }
}

auto AlarmDisplayState::deleteSelectedAlarm() -> void {
    // 画面上で選択されている時刻を取得
    const std::vector<time_t> displayedAlarms = getAlarmList();
    if (selectedIndex >= displayedAlarms.size()) {
        return; // 選択位置が無効
    }
    
    const time_t selectedTime = displayedAlarms[selectedIndex];
    
    // 実体リストから一致するものを削除（valueベース削除）
    const auto iterator = std::find(alarm_times.begin(), alarm_times.end(), selectedTime);
    if (iterator != alarm_times.end()) {
        alarm_times.erase(iterator);
        // 削除成功 - 即座に画面を再描画
        forceDraw();
    }
    // 既に消化済みの場合は何もしない（正常終了）
}

auto AlarmDisplayState::moveUp() -> void {
    if (selectedIndex > 0) {
        selectedIndex--;
    }
    // 端で停止（循環なし）
}

auto AlarmDisplayState::moveDown() -> void {
    const std::vector<time_t> alarms = getAlarmList();
    if (selectedIndex < alarms.size() - 1) {
        selectedIndex++;
    }
    // 端で停止（循環なし）
}

auto AlarmDisplayState::moveToTop() -> void {
    selectedIndex = 0;
}

auto AlarmDisplayState::moveToBottom() -> void {
    const std::vector<time_t> alarms = getAlarmList();
    if (!alarms.empty()) {
        selectedIndex = alarms.size() - 1;
    }
}

auto AlarmDisplayState::shouldUpdateRealTime() const -> bool {
    // ユーザー操作から一定時間経過していればリアルタイム更新
    return (getCurrentMillis() - lastUserAction) > UPDATE_PAUSE_DURATION;
}

auto AlarmDisplayState::updateLastUserAction() -> void {
    lastUserAction = getCurrentMillis();
}

auto AlarmDisplayState::getCurrentMillis() const -> unsigned long {
    // ITimeManager経由で時刻を取得
    if (timeManager) {
        return timeManager->getCurrentMillis();
    }
    // フォールバック: 簡易実装として0を返す
    return 0;
} 