#pragma once
#include "IAlarmDisplayView.h"
#include "DisplayCommon.h"
#include "IDisplay.h"
#include <vector>
#include <ctime>
#include <cstdio>
#include <cstring>
#include <array>

class AlarmDisplayViewImpl : public IAlarmDisplayView {
public:
    AlarmDisplayViewImpl(IDisplay* disp) : disp(disp) {}
    
    void showTitle(const char* title, int batteryLevel, bool isCharging) override {
        drawTitleBar(disp, title, batteryLevel, isCharging);
    }
    
    void showHints(const char* btnA, const char* btnB, const char* btnC) override {
        drawButtonHintsGrid(disp, btnA, btnB, btnC);
    }
    
    void showAlarmList(const std::vector<time_t>& alarms, size_t selectedIndex) override {
        if (disp == nullptr) {
            return;
        }
        
        // 前回より項目が減った場合、減った分の領域をクリア
        if (alarms.size() < lastDisplayedAlarms.size()) {
            const int clearStartY = ALARM_DISPLAY_START_Y + alarms.size() * ALARM_LINE_HEIGHT - ALARM_BACKGROUND_OFFSET;
            const int clearHeight = (std::min(static_cast<int>(lastDisplayedAlarms.size()), ALARM_MAX_DISPLAY) - static_cast<int>(alarms.size())) * ALARM_LINE_HEIGHT;
            if (clearHeight > 0) {
                disp->fillRect(0, clearStartY, SCREEN_WIDTH, clearHeight, TFT_BLACK);
            }
        }
        
        for (size_t i = 0; i < alarms.size() && i < ALARM_MAX_DISPLAY; ++i) {
            // 時刻文字列に変換
            struct tm* tm_alarm = localtime(&alarms[i]);
            std::array<char, ALARM_TIME_STR_SIZE> timeStr{};
            snprintf(timeStr.data(), timeStr.size(), "%02d:%02d", tm_alarm->tm_hour, tm_alarm->tm_min);
            
            const int y_pos = ALARM_DISPLAY_START_Y + i * ALARM_LINE_HEIGHT;
            
            // 背景をクリア（選択状態に関係なく）
            disp->fillRect(0, y_pos - ALARM_BACKGROUND_OFFSET, SCREEN_WIDTH, ALARM_LINE_HEIGHT, TFT_BLACK);
            
            if (i == selectedIndex) {
                // 選択中のアラーム（背景色反転）
                disp->fillRect(0, y_pos - ALARM_BACKGROUND_OFFSET, SCREEN_WIDTH, ALARM_LINE_HEIGHT, AMBER_COLOR); // アンバー背景
                disp->setTextColor(TFT_BLACK, AMBER_COLOR); // 黒文字、アンバー背景
            } else {
                // 通常表示
                disp->setTextColor(AMBER_COLOR, TFT_BLACK); // アンバー文字、黒背景
            }
            
            disp->setTextFont(FONT_AUXILIARY);
            disp->setTextDatum(TL_DATUM);
            disp->drawText(ALARM_TEXT_OFFSET, y_pos, timeStr.data(), FONT_AUXILIARY);
        }
        
        // 色をリセット
        disp->setTextColor(AMBER_COLOR, TFT_BLACK);
        
        // 現在の状態を記憶
        lastDisplayedAlarms = alarms;
    }
    
    void showNoAlarms() override {
        if (disp == nullptr) {
            return;
        }
        
        // 前回アラームがあった場合は、その領域をクリア
        if (!lastDisplayedAlarms.empty()) {
            const int clearHeight = std::min(static_cast<int>(lastDisplayedAlarms.size()), ALARM_MAX_DISPLAY) * ALARM_LINE_HEIGHT;
            disp->fillRect(0, ALARM_DISPLAY_START_Y - ALARM_BACKGROUND_OFFSET, SCREEN_WIDTH, clearHeight, TFT_BLACK);
        }
        
        disp->setTextDatum(MC_DATUM); // 中央基準
        disp->setTextColor(AMBER_COLOR, TFT_BLACK);
        disp->setTextFont(FONT_AUXILIARY);
        disp->drawText(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, "NO ALARMS", FONT_AUXILIARY);
        
        // 空リスト状態を記憶
        lastDisplayedAlarms.clear();
    }
    
    void clear() override {
        if (disp) {
            disp->clear();
        }
    }
    
private:
    IDisplay* disp;
    std::vector<time_t> lastDisplayedAlarms;
}; 