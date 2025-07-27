#include "AlarmDisplayState.h"
#include <cstdio>
#include <cstring>

// 外部変数（既存のアラームリスト）
extern std::vector<time_t> alarm_times;

void AlarmDisplayState::onEnter() {
    selectedIndex = 0;
    if (display) {
        display->clear();
        display->drawText(0, 0, "ALARMS", 4);
        display->drawText(0, 200, "A:UP B:DOWN C:DEL", 2);
    }
}

void AlarmDisplayState::onExit() {
    // クリーンアップ処理（必要に応じて）
}

void AlarmDisplayState::onDraw() {
    if (!display) return;
    
    // アラームリストを取得
    std::vector<time_t> alarms = getAlarmList();
    
    if (alarms.empty()) {
        // 空リスト表示
        display->drawText(120, 100, "NO ALARMS", 4);
        return;
    }
    
    // 選択位置の調整
    adjustSelectionIndex();
    
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
        
        if (i == selectedIndex) {
            // 選択中のアラーム（背景色反転）
            display->fillRect(0, y - 5, 320, LINE_HEIGHT, 0xFB20); // アンバー背景
            display->setTextColor(0x000000, 0xFB20); // 黒文字、アンバー背景
        } else {
            // 通常表示
            display->setTextColor(0xFB20, 0x000000); // アンバー文字、黒背景
        }
        
        display->drawText(10, y, timeStr, 4);
    }
    
    // 色をリセット
    display->setTextColor(0xFB20, 0x000000);
}

void AlarmDisplayState::onButtonA() {
    moveUp();
}

void AlarmDisplayState::onButtonB() {
    moveDown();
}

void AlarmDisplayState::onButtonC() {
    deleteSelectedAlarm();
}

void AlarmDisplayState::onButtonALongPress() {
    moveToTop();
}

void AlarmDisplayState::onButtonBLongPress() {
    moveToBottom();
}

void AlarmDisplayState::onButtonCLongPress() {
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
    std::vector<time_t> alarms = getAlarmList();
    if (alarms.empty() || selectedIndex >= alarms.size()) {
        return;
    }
    
    // アラーム削除
    if (AlarmLogic::deleteAlarm(alarm_times, selectedIndex)) {
        // 削除後の選択位置調整
        if (selectedIndex >= alarm_times.size() && !alarm_times.empty()) {
            selectedIndex = alarm_times.size() - 1;
        }
    }
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