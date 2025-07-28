#include "SettingsDisplayViewImpl.h"
#include <cstring>
#ifdef ARDUINO
#include <Arduino.h>
#endif

void SettingsDisplayViewImpl::showSettingsList(const std::vector<std::string>& items, size_t selectedIndex) {
    if (disp == nullptr) {
        return;
    }
    
    // アラーム管理画面と同様のレイアウト
    const int startY = ALARM_DISPLAY_START_Y;
    const int lineHeight = ALARM_LINE_HEIGHT;
    const int maxItems = ALARM_MAX_DISPLAY;
    
    // 表示可能な項目数を制限
    size_t displayCount = (items.size() < maxItems) ? items.size() : maxItems;
    
    for (size_t i = 0; i < displayCount; ++i) {
        const int y_pos = startY + i * lineHeight;
        
        // 背景をクリア（選択状態に関係なく）
        disp->fillRect(0, y_pos - ALARM_BACKGROUND_OFFSET, SCREEN_WIDTH, lineHeight, TFT_BLACK);
        
        if (i == selectedIndex) {
            // 選択中の項目（背景色反転）
            disp->fillRect(0, y_pos - ALARM_BACKGROUND_OFFSET, SCREEN_WIDTH, lineHeight, AMBER_COLOR);
            disp->setTextColor(TFT_BLACK, AMBER_COLOR);
        } else {
            // 通常表示
            disp->setTextColor(AMBER_COLOR, TFT_BLACK);
        }
        
        disp->setTextFont(FONT_AUXILIARY);
        disp->setTextDatum(ML_DATUM);
        disp->drawText(ALARM_TEXT_OFFSET, y_pos, items[i].c_str(), FONT_AUXILIARY);
    }
    
    // 色をリセット
    disp->setTextColor(AMBER_COLOR, TFT_BLACK);
} 