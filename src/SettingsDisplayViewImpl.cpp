#include "SettingsDisplayViewImpl.h"
#include <cstring>

void SettingsDisplayViewImpl::showSettingsList(const std::vector<std::string>& items, size_t selectedIndex) {
    if (!disp) return;
    
    // グリッドセル(0,2)-(15,6)の範囲内に表示
    const int startY = GRID_Y(2);
    const int endY = GRID_Y(6);
    const int itemHeight = 20; // Font4の高さ
    const int maxItems = (endY - startY) / itemHeight;
    
    // 表示可能な項目数を制限
    size_t displayCount = (items.size() < maxItems) ? items.size() : maxItems;
    
    for (size_t i = 0; i < displayCount; ++i) {
        int y = startY + i * itemHeight;
        
        // 選択中の項目は背景色反転
        if (i == selectedIndex) {
            disp->fillRect(0, y, SCREEN_WIDTH, itemHeight, AMBER_COLOR);
            disp->setTextColor(TFT_BLACK, AMBER_COLOR);
        } else {
            disp->setTextColor(AMBER_COLOR, TFT_BLACK);
        }
        
        // テキスト描画
        disp->setTextFont(4);
        disp->setTextDatum(ML_DATUM);
        disp->drawText(10, y + itemHeight/2, items[i].c_str(), 4);
    }
} 