#include "DateTimeInputViewImpl.h"
#include <cstring>
#ifdef ARDUINO
#include <Arduino.h>
#endif

void DateTimeInputViewImpl::clear() {
    if (disp) {
        disp->clear();
    }
}

void DateTimeInputViewImpl::showTitle(const char* title, int batteryLevel, bool isWarning) {
    if (!disp) return;
    
    // タイトルバー描画
    drawTitleBar(disp, title, batteryLevel, isWarning);
}

void DateTimeInputViewImpl::showHints(const char* hintA, const char* hintB, const char* hintC) {
    if (!disp) return;
    
    drawButtonHintsGrid(disp, hintA, hintB, hintC);
}

void DateTimeInputViewImpl::showDateTimeString(const std::string& dateTimeStr, int cursorPosition) {
    if (!disp) return;
    
    disp->setTextFont(4);
    disp->setTextDatum(MC_DATUM);
    disp->setTextColor(AMBER_COLOR, TFT_BLACK);
    
    // 日時文字列を中央に表示
    disp->drawText(160, 120, dateTimeStr.c_str(), 4);
    
    // カーソルハイライト描画
    drawCursorHighlight(dateTimeStr, cursorPosition);
}

void DateTimeInputViewImpl::showErrorMessage(const std::string& message) {
    if (!disp) return;
    
    disp->setTextFont(2);
    disp->setTextDatum(MC_DATUM);
    disp->setTextColor(AMBER_COLOR, TFT_BLACK);
    
    // エラーメッセージを中央に表示
    disp->drawText(160, 160, message.c_str(), 2);
}

void DateTimeInputViewImpl::drawCursorHighlight(const std::string& dateTimeStr, int cursorPosition) {
    if (!disp || cursorPosition < 0 || cursorPosition >= static_cast<int>(dateTimeStr.length())) {
        return;
    }
    
    // カーソル位置の文字をネガポジ反転表示
    int cursorX = getCursorPixelPosition(cursorPosition);
    int charWidth = 16; // フォント4の文字幅（概算）
    
    // カーソル位置の背景を反転
    disp->fillRect(cursorX - 2, 110, charWidth + 4, 20, AMBER_COLOR);
    
    // カーソル位置の文字を黒で再描画
    disp->setTextColor(TFT_BLACK, AMBER_COLOR);
    disp->setTextDatum(TL_DATUM);
    char cursorChar[2] = {dateTimeStr[cursorPosition], '\0'};
    disp->drawText(cursorX, 110, cursorChar, 4);
    
    // 色を元に戻す
    disp->setTextColor(AMBER_COLOR, TFT_BLACK);
    disp->setTextDatum(MC_DATUM);
}

int DateTimeInputViewImpl::getCursorPixelPosition(int cursorPosition) const {
    // 日時文字列の各文字位置を計算
    // "2025/01/01 00:00" 形式での位置計算
    
    const int baseX = 80; // 開始X座標
    const int charWidth = 16; // フォント4の文字幅（概算）
    
    // 各文字の位置を計算
    int positions[] = {
        0,   // 年千
        1,   // 年百
        2,   // 年十
        3,   // 年一
        4,   // /
        5,   // 月十
        6,   // 月一
        7,   // /
        8,   // 日十
        9,   // 日一
        10,  // スペース
        11,  // 時十
        12,  // 時一
        13,  // :
        14,  // 分十
        15   // 分一
    };
    
    if (cursorPosition >= 0 && cursorPosition < 16) {
        return baseX + positions[cursorPosition] * charWidth;
    }
    
    return baseX;
} 