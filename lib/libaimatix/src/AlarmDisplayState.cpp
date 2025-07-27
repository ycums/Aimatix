#include "AlarmDisplayState.h"
#include "DisplayCommon.h"
#include "ui_constants.h"
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <array>

// 外部変数（既存のアラームリスト）
extern std::vector<time_t> alarm_times;

void AlarmDisplayState::onEnter() {
    selectedIndex = 0;
    lastSelectedIndex = 0; // 初期状態をリセット
    lastDisplayedAlarms.clear(); // 初期状態をリセット
    updateLastUserAction();
    if (display) {
        display->clear();
        // 共通のタイトルバーとボタンヒントを使用
        drawTitleBar(display, "ALARMS", BATTERY_WARNING_THRESHOLD, false); // 仮のバッテリー値
        drawButtonHintsGrid(display, "UP", "DOWN", "DEL");
    }
    // 初期表示を即座に実行（鈍重さを解消）
    forceDraw();
}

void AlarmDisplayState::onExit() {
    // クリーンアップ処理（必要に応じて）
}

void AlarmDisplayState::onDraw() {
    if (display == nullptr) {
        return;
    }
    
    // ハイブリッドアプローチ: リアルタイム更新の判定
    if (shouldUpdateRealTime()) {
        forceDraw();
    }
    // 更新抑制中は前回の表示を維持（何もしない）
}

void AlarmDisplayState::forceDraw() {
    if (display == nullptr) {
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
        // 空リスト表示（中央配置）
        // 前回アラームがあった場合は、その領域をクリア
        if (!lastDisplayedAlarms.empty()) {
            const int clearHeight = std::min(static_cast<int>(lastDisplayedAlarms.size()), ALARM_MAX_DISPLAY) * ALARM_LINE_HEIGHT;
            display->fillRect(0, ALARM_DISPLAY_START_Y - ALARM_BACKGROUND_OFFSET, SCREEN_WIDTH, clearHeight, TFT_BLACK);
        }
        
        display->setTextDatum(MC_DATUM); // 中央基準
        display->setTextColor(AMBER_COLOR, TFT_BLACK);
        display->setTextFont(FONT_AUXILIARY);
        display->drawText(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, "NO ALARMS", FONT_AUXILIARY);
    } else {
        // アラームリスト表示
        
        // 前回より項目が減った場合、減った分の領域をクリア
        if (alarms.size() < lastDisplayedAlarms.size()) {
            const int clearStartY = ALARM_DISPLAY_START_Y + alarms.size() * ALARM_LINE_HEIGHT - ALARM_BACKGROUND_OFFSET;
            const int clearHeight = (std::min(static_cast<int>(lastDisplayedAlarms.size()), ALARM_MAX_DISPLAY) - static_cast<int>(alarms.size())) * ALARM_LINE_HEIGHT;
            if (clearHeight > 0) {
                display->fillRect(0, clearStartY, SCREEN_WIDTH, clearHeight, TFT_BLACK);
            }
        }
        
        for (size_t i = 0; i < alarms.size() && i < ALARM_MAX_DISPLAY; ++i) {
            // 時刻文字列に変換
            struct tm* tm_alarm = localtime(&alarms[i]);
            std::array<char, ALARM_TIME_STR_SIZE> timeStr{};
            snprintf(timeStr.data(), timeStr.size(), "%02d:%02d", tm_alarm->tm_hour, tm_alarm->tm_min);
            
            const int y_pos = ALARM_DISPLAY_START_Y + i * ALARM_LINE_HEIGHT;
            
            // 背景をクリア（選択状態に関係なく）
            display->fillRect(0, y_pos - ALARM_BACKGROUND_OFFSET, SCREEN_WIDTH, ALARM_LINE_HEIGHT, TFT_BLACK);
            
            if (i == selectedIndex) {
                // 選択中のアラーム（背景色反転）
                display->fillRect(0, y_pos - ALARM_BACKGROUND_OFFSET, SCREEN_WIDTH, ALARM_LINE_HEIGHT, AMBER_COLOR); // アンバー背景
                display->setTextColor(TFT_BLACK, AMBER_COLOR); // 黒文字、アンバー背景
            } else {
                // 通常表示
                display->setTextColor(AMBER_COLOR, TFT_BLACK); // アンバー文字、黒背景
            }
            
            display->setTextFont(FONT_AUXILIARY);
            display->setTextDatum(TL_DATUM);
            display->drawText(ALARM_TEXT_OFFSET, y_pos, timeStr.data(), FONT_AUXILIARY);
        }
        
        // 色をリセット
        display->setTextColor(AMBER_COLOR, TFT_BLACK);
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
    if (manager && mainDisplayState) {
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

void AlarmDisplayState::deleteSelectedAlarm() {
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

void AlarmDisplayState::moveUp() {
    if (selectedIndex > 0) {
        selectedIndex--;
    }
    // 端で停止（循環なし）
}

void AlarmDisplayState::moveDown() {
    const std::vector<time_t> alarms = getAlarmList();
    if (selectedIndex < alarms.size() - 1) {
        selectedIndex++;
    }
    // 端で停止（循環なし）
}

void AlarmDisplayState::moveToTop() {
    selectedIndex = 0;
}

void AlarmDisplayState::moveToBottom() {
    const std::vector<time_t> alarms = getAlarmList();
    if (!alarms.empty()) {
        selectedIndex = alarms.size() - 1;
    }
}

bool AlarmDisplayState::shouldUpdateRealTime() const {
    // ユーザー操作から一定時間経過していればリアルタイム更新
    return (getCurrentMillis() - lastUserAction) > UPDATE_PAUSE_DURATION;
}

void AlarmDisplayState::updateLastUserAction() {
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