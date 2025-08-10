#include "DateTimeInputViewImpl.h"
#include "ui_constants.h"
#include <cstring>
#include <string>
#ifdef ARDUINO
#include <Arduino.h>
#endif

void DateTimeInputViewImpl::clear() {
    if (disp != nullptr) {
        disp->clear();
        // 状態をリセット
        lastCursorPosition = -1;
        lastDateTimeStr = "";
    }
}

void DateTimeInputViewImpl::showTitle(const char* title, int batteryLevel, bool isWarning) {
    if (disp == nullptr) return;
    
    // タイトルバー描画
    drawTitleBar(disp, title, batteryLevel, isWarning);
}

void DateTimeInputViewImpl::showHints(const char* hintA, const char* hintB, const char* hintC) {
    if (disp == nullptr) return;
    
    drawButtonHintsGrid(disp, hintA, hintB, hintC);
}

void DateTimeInputViewImpl::showDateTimeString(const std::string& dateTimeStr, int cursorPosition) {
    if (disp == nullptr) return;
    
    // 状態変更の検出
    bool needsRedraw = (dateTimeStr != lastDateTimeStr) || (lastDateTimeStr.empty());
    const bool cursorChanged = (cursorPosition != lastCursorPosition);
    
    // 日時文字列が変更された場合または初回描画時、全体を再描画
    if (needsRedraw) {
        drawDateTimeGrid(dateTimeStr);
        // 全体再描画後、カーソル位置をハイライト
        if (cursorPosition >= 0 && cursorPosition < static_cast<int>(dateTimeStr.length())) {
            drawCursorHighlight(dateTimeStr, cursorPosition);
        }
    } else if (cursorChanged) {
        // カーソルのみ変更された場合
        // 前のカーソル位置をクリア
        if (lastCursorPosition >= 0 && lastCursorPosition < static_cast<int>(dateTimeStr.length())) {
            clearCursorHighlight(lastCursorPosition);
        }
        // 新しいカーソル位置をハイライト
        if (cursorPosition >= 0 && cursorPosition < static_cast<int>(dateTimeStr.length())) {
            drawCursorHighlight(dateTimeStr, cursorPosition);
        }
    }
    
    // 状態を更新
    lastDateTimeStr = dateTimeStr;
    lastCursorPosition = cursorPosition;
}

void DateTimeInputViewImpl::showErrorMessage(const std::string& message) {
    if (disp == nullptr) return;
    
    disp->setTextFont(2);
    disp->setTextDatum(MC_DATUM);
    disp->setTextColor(AMBER_COLOR, TFT_BLACK);
    
    // エラーメッセージを中央に表示
    disp->drawText(SCREEN_CENTER_X, SCREEN_CENTER_X, message.c_str(), 2);
}

void DateTimeInputViewImpl::drawDateTimeGrid(const std::string& dateTimeStr) {
    if (disp == nullptr) return;
    
    disp->setTextFont(FONT_MAIN);
    disp->setTextDatum(MC_DATUM);
    disp->setTextColor(AMBER_COLOR, TFT_BLACK);
    
    // 文字列全体の幅を計算
    const int totalWidth = std::accumulate(
        dateTimeStr.begin(),
        dateTimeStr.end(),
        0,
        [](int acc, char c) { return acc + getCharWidth(c); }
    );
    
    // 中央寄せのための開始X座標を計算
    const int startX = (SCREEN_WIDTH - totalWidth) / 2;
    const int baseY = GRID_Y(5); // グリッドレイアウトに準拠
    
    // 日時文字列をグリッド様に1文字ずつ描画
    int currentX = startX;
    for (size_t i = 0; i < dateTimeStr.length(); ++i) {
        char c = dateTimeStr[i];
        // MC_DATUMなので文字の中央座標を計算
        const int charCenterX = currentX + getCharWidth(c) / 2;
        drawCharAtPosition(c, charCenterX, baseY, false);
        currentX += getCharWidth(c);
    }
}

void DateTimeInputViewImpl::drawCursorHighlight(const std::string& dateTimeStr, int cursorPosition) {
    if (disp == nullptr || cursorPosition < 0 || cursorPosition >= static_cast<int>(dateTimeStr.length())) {
        return;
    }
    
    // カーソル位置の文字をハイライト表示
    const int cursorX = getCursorPixelPosition(cursorPosition);
    const int baseY = GRID_Y(5); // グリッドレイアウトに準拠
    const char cursorChar = dateTimeStr[cursorPosition];
    
    drawCharAtPosition(cursorChar, cursorX, baseY, true);
}

void DateTimeInputViewImpl::clearCursorHighlight(int cursorPosition) {
    if (disp == nullptr || cursorPosition < 0 || cursorPosition >= static_cast<int>(lastDateTimeStr.length())) {
        return;
    }
    
    // 前のカーソル位置の文字を通常表示に戻す
    const int cursorX = getCursorPixelPosition(cursorPosition);
    const int baseY = GRID_Y(5); // グリッドレイアウトに準拠
    const char cursorChar = lastDateTimeStr[cursorPosition];
    
    drawCharAtPosition(cursorChar, cursorX, baseY, false);
}

void DateTimeInputViewImpl::drawCharAtPosition(char c, int x, int y, bool isHighlighted) {
    if (disp == nullptr) return;
    
    // フォントとDATUMの設定
    disp->setTextFont(FONT_MAIN);
    disp->setTextDatum(MC_DATUM);
    
    if (isHighlighted) {
        // ハイライト表示：setTextColorで背景色も指定
        disp->setTextColor(TFT_BLACK, AMBER_COLOR);
    } else {
        // 通常表示
        disp->setTextColor(AMBER_COLOR, TFT_BLACK);
    }
    
    // 文字を描画
    char charStr[2] = {c, '\0'};
    disp->drawText(x, y, charStr, FONT_MAIN);
}

int DateTimeInputViewImpl::getCursorPixelPosition(int cursorPosition) {
    // 日時文字列の各文字位置を正確に計算
    // "2025/01/01 00:00" 形式での位置計算
    
    // 実際の日時文字列から位置を計算
    std::string dateTimeStr = "2025/01/01 00:00";
    
    // 文字列全体の幅を計算
    int totalWidth = 0;
    for (size_t i = 0; i < dateTimeStr.length(); ++i) {
        totalWidth += getCharWidth(dateTimeStr[i]);
    }
    
    // 中央寄せのための開始X座標を計算
    const int startX = (SCREEN_WIDTH - totalWidth) / 2;
    int currentX = startX;
    
    // 各文字の位置を正確に計算（MC_DATUMなので文字の中央座標）
    for (int i = 0; i < cursorPosition && i < static_cast<int>(dateTimeStr.length()); ++i) {
        const char c = dateTimeStr[i];
        currentX += getCharWidth(c);
    }
    
    // MC_DATUMなので文字の中央座標を返す
    if (cursorPosition >= 0 && cursorPosition < static_cast<int>(dateTimeStr.length())) {
        return currentX + getCharWidth(dateTimeStr[cursorPosition]) / 2;
    }
    return currentX;
}

int DateTimeInputViewImpl::getCharWidth(char c) {
    // フォント4での文字幅を正確に計算
    switch (c) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            return CHAR_WIDTH_FONT7; // 数字は16ピクセル
        case '/': case ':': case ' ':
            return CHAR_WIDTH_FONT4;  // 記号・スペースは8ピクセル
        default:
            return CHAR_WIDTH_FONT7; // デフォルト
    }
} 