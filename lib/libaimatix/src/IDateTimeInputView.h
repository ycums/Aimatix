#pragma once
#include <string>

// 日時入力画面のViewインターフェース
class IDateTimeInputView {
public:
    virtual ~IDateTimeInputView() {}
    
    // 画面クリア
    virtual void clear() = 0;
    
    // タイトル表示
    virtual void showTitle(const char* title, int batteryLevel, bool isWarning) = 0;
    
    // ボタンヒント表示
    virtual void showHints(const char* hintA, const char* hintB, const char* hintC) = 0;
    
    // 日時文字列表示
    virtual void showDateTimeString(const std::string& dateTimeStr, int cursorPosition) = 0;
    
    // エラーメッセージ表示
    virtual void showErrorMessage(const std::string& message) = 0;
}; 