#pragma once
#include "IDateTimeInputView.h"
#include "DisplayCommon.h"
#include "IDisplay.h"

// 日時入力画面のView実装（M5Stack依存）
class DateTimeInputViewImpl : public IDateTimeInputView {
public:
    explicit DateTimeInputViewImpl(IDisplay* disp) : disp(disp) {}
    
    void clear() override;
    void showTitle(const char* title, int batteryLevel, bool isWarning) override;
    void showHints(const char* hintA, const char* hintB, const char* hintC) override;
    void showDateTimeString(const std::string& dateTimeStr, int cursorPosition) override;
    void showErrorMessage(const std::string& message) override;
    
private:
    IDisplay* disp;
    
    // 内部ヘルパーメソッド
    void drawCursorHighlight(const std::string& dateTimeStr, int cursorPosition);
    int getCursorPixelPosition(int cursorPosition) const;
}; 