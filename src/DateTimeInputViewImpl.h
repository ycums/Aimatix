#pragma once
#include "IDateTimeInputView.h"
#include "DisplayCommon.h"
#include "IDisplay.h"
#include <string>

// 日時入力画面のView実装（M5Stack依存）
class DateTimeInputViewImpl : public IDateTimeInputView {
public:
    explicit DateTimeInputViewImpl(IDisplay* disp) : disp(disp), lastCursorPosition(-1), lastDateTimeStr("") {}
    
    void clear() override;
    void showTitle(const char* title, int batteryLevel, bool isWarning) override;
    void showHints(const char* hintA, const char* hintB, const char* hintC) override;
    void showDateTimeString(const std::string& dateTimeStr, int cursorPosition) override;
    void showErrorMessage(const std::string& message) override;
    
private:
    IDisplay* disp;
    
    // ちらつき防止用の状態管理
    int lastCursorPosition;
    std::string lastDateTimeStr;
    
    // 内部ヘルパーメソッド
    void drawDateTimeGrid(const std::string& dateTimeStr);
    void drawCursorHighlight(const std::string& dateTimeStr, int cursorPosition);
    void clearCursorHighlight(int cursorPosition);
    static int getCursorPixelPosition(int cursorPosition);
    static int getCharWidth(char c);
    void drawCharAtPosition(char c, int x, int y, bool isHighlighted = false);
}; 