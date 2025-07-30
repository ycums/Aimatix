#pragma once
#include "StateManager.h"
#include "ITimeProvider.h"
#include "IDateTimeInputView.h"
#include <string>
#include <vector>

// 日時入力画面の状態クラス
class DateTimeInputState : public IState {
public:
    DateTimeInputState(ITimeProvider* timeProvider = nullptr, IDateTimeInputView* view = nullptr)
        : timeProvider(timeProvider), view(view), manager(nullptr), settingsDisplayState(nullptr),
          cursorPosition(0), isEditMode(false) {
        resetDateTime();
    }
    
    void onEnter() override;
    void onExit() override;
    void onDraw() override;
    void onButtonA() override;
    void onButtonB() override;
    void onButtonC() override;
    void onButtonALongPress() override;
    void onButtonBLongPress() override;
    void onButtonCLongPress() override;
    
    void setManager(StateManager* m) { manager = m; }
    void setSettingsDisplayState(IState* settingsState) { settingsDisplayState = settingsState; }
    void setTimeProvider(ITimeProvider* provider) { timeProvider = provider; }
    void setView(IDateTimeInputView* v) { view = v; }
    
    // テスト用のアクセサ
    int getCursorPosition() const { return cursorPosition; }
    void setCursorPosition(int pos) { cursorPosition = pos; }
    const std::vector<int>& getDateTimeDigits() const { return dateTimeDigits; }
    void setDateTimeDigits(const std::vector<int>& digits) { dateTimeDigits = digits; }
    bool getIsEditMode() const { return isEditMode; }
    void setIsEditMode(bool mode) { isEditMode = mode; }
    
    // テスト用のバリデーションメソッド
    bool validateDateTime() const;
    std::string formatDateTimeString() const;
    
private:
    ITimeProvider* timeProvider;
    IDateTimeInputView* view;
    StateManager* manager;
    IState* settingsDisplayState;
    
    // 日時入力データ: [年千,年百,年十,年一,月十,月一,日十,日一,時十,時一,分十,分一]
    std::vector<int> dateTimeDigits;
    int cursorPosition;  // 0-11の範囲
    bool isEditMode;
    
    // 定数
    static constexpr int YEAR_MIN = 2020;
    static constexpr int YEAR_MAX = 2030;
    static constexpr int MONTH_MIN = 1;
    static constexpr int MONTH_MAX = 12;
    static constexpr int DAY_MIN = 1;
    static constexpr int DAY_MAX = 31;
    static constexpr int HOUR_MIN = 0;
    static constexpr int HOUR_MAX = 23;
    static constexpr int MINUTE_MIN = 0;
    static constexpr int MINUTE_MAX = 59;
    
    // 内部メソッド
    void resetDateTime();
    void incrementCurrentDigit();
    void moveCursorRight();
    void moveCursorLeft();
    int dataPositionToStringPosition(int dataPos) const;
    bool validateYear() const;
    bool validateMonth() const;
    bool validateDay() const;
    bool validateHour() const;
    bool validateMinute() const;
    void commitDateTime();
    int getDigitValue(int position) const;
    void setDigitValue(int position, int value);
    bool isLeapYear(int year) const;
    int getDaysInMonth(int year, int month) const;
}; 