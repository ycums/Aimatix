#include "AlarmDisplayState.h"
#include "DisplayCommon.h"
#include "ui_constants.h"
#include <cstdio>
#include <cstring>
#include <algorithm>

// 外部変数（既存のアラームリスト）
extern std::vector<time_t> alarm_times;

void AlarmDisplayState::onEnter() {
    selectedIndex = 0;
    updateLastUserAction();
    if (display) {
        display->clear();
        // 共通のタイトルバーとボタンヒントを使用
        drawTitleBar(display, "ALARMS", 100, false); // 仮のバッテリー値
        drawButtonHintsGrid(display, "UP", "DOWN", "DEL");
    }
}

void AlarmDisplayState::onExit() {
    // クリーンアップ処理（必要に応じて）
}

void AlarmDisplayState::onDraw() {
    if (!display) return;
    
    // ハイブリッドアプローチ: リアルタイム更新の判定
    if (shouldUpdateRealTime()) {
        // アラームリストを取得（毎回最新の状態を取得）
        std::vector<time_t> alarms = getAlarmList();
        
        // 選択位置の調整（アラーム消化後も適切に調整）
        adjustSelectionIndex();
        
        // ちらつき防止：画面全体をクリア
        display->fillRect(0, TITLE_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - TITLE_HEIGHT - HINT_HEIGHT, TFT_BLACK);
        
        if (alarms.empty()) {
            // 空リスト表示（中央配置）
            display->setTextDatum(MC_DATUM); // 中央基準
            display->setTextColor(AMBER_COLOR, TFT_BLACK);
            display->setTextFont(FONT_AUXILIARY);
            display->drawText(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, "NO ALARMS", FONT_AUXILIARY);
            return;
        }
        
        // アラームリスト表示
        constexpr int START_Y = 40;
        constexpr int LINE_HEIGHT = 30;
        constexpr int MAX_DISPLAY = 5;
        
        for (size_t i = 0; i < alarms.size() && i < MAX_DISPLAY; ++i) {
            // 時刻文字列に変換
            struct tm* tm_alarm = localtime(&alarms[i]);
            char timeStr[16];
            snprintf(timeStr, sizeof(timeStr), "%02d:%02d", tm_alarm->tm_hour, tm_alarm->tm_min);
            
            int y = START_Y + i * LINE_HEIGHT;
            
            // 背景をクリア（選択状態に関係なく）
            display->fillRect(0, y - 5, SCREEN_WIDTH, LINE_HEIGHT, TFT_BLACK);
            
            if (i == selectedIndex) {
                // 選択中のアラーム（背景色反転）
                display->fillRect(0, y - 5, SCREEN_WIDTH, LINE_HEIGHT, AMBER_COLOR); // アンバー背景
                display->setTextColor(TFT_BLACK, AMBER_COLOR); // 黒文字、アンバー背景
            } else {
                // 通常表示
                display->setTextColor(AMBER_COLOR, TFT_BLACK); // アンバー文字、黒背景
            }
            
            display->setTextFont(FONT_AUXILIARY);
            display->setTextDatum(TL_DATUM);
            display->drawText(10, y, timeStr, FONT_AUXILIARY);
        }
        
        // 色をリセット
        display->setTextColor(AMBER_COLOR, TFT_BLACK);
    }
    // 更新抑制中は前回の表示を維持（何もしない）
}

void AlarmDisplayState::onButtonA() {
    updateLastUserAction();
    moveUp();
}

void AlarmDisplayState::onButtonB() {
    updateLastUserAction();
    moveDown();
}

void AlarmDisplayState::onButtonC() {
    updateLastUserAction();
    deleteSelectedAlarm();
}

void AlarmDisplayState::onButtonALongPress() {
    updateLastUserAction();
    moveToTop();
}

void AlarmDisplayState::onButtonBLongPress() {
    updateLastUserAction();
    moveToBottom();
}

void AlarmDisplayState::onButtonCLongPress() {
    updateLastUserAction();
    // メイン画面に戻る
    if (manager && mainDisplayState) {
        manager->setState(mainDisplayState);
    }
}

std::vector<time_t> AlarmDisplayState::getAlarmList() const {
    // 外部変数からアラームリストを取得
    return AlarmLogic::getAlarms(alarm_times);
}

void AlarmDisplayState::adjustSelectionIndex() {
    std::vector<time_t> alarms = getAlarmList();
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
    std::vector<time_t> displayedAlarms = getAlarmList();
    if (selectedIndex >= displayedAlarms.size()) {
        return; // 選択位置が無効
    }
    
    time_t selectedTime = displayedAlarms[selectedIndex];
    
    // 実体リストから一致するものを削除（valueベース削除）
    auto it = std::find(alarm_times.begin(), alarm_times.end(), selectedTime);
    if (it != alarm_times.end()) {
        alarm_times.erase(it);
        // 削除成功 - 選択位置の調整は次回のonDrawで行われる
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
    std::vector<time_t> alarms = getAlarmList();
    if (selectedIndex < alarms.size() - 1) {
        selectedIndex++;
    }
    // 端で停止（循環なし）
}

void AlarmDisplayState::moveToTop() {
    selectedIndex = 0;
}

void AlarmDisplayState::moveToBottom() {
    std::vector<time_t> alarms = getAlarmList();
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

unsigned long AlarmDisplayState::getCurrentMillis() const {
    // ITimeManager経由で時刻を取得
    if (timeManager) {
        return timeManager->getCurrentMillis();
    }
    // フォールバック: 簡易実装として0を返す
    return 0;
} 