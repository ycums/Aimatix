#pragma once
#include "StateManager.h"
#include "ITimeService.h"
#include "IDateTimeInputView.h"
#include <array>
#include <string>
#include <vector>

// 日時入力画面の状態クラス
class DateTimeInputState : public IState {
public:
    DateTimeInputState(ITimeService* timeService = nullptr, IDateTimeInputView* view = nullptr)
        : timeService(timeService), view(view), manager(nullptr), settingsDisplayState(nullptr),
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
    void setTimeService(ITimeService* service) { timeService = service; }
    void setView(IDateTimeInputView* v) { view = v; }
    
    // 公開バリデーションメソッド
    bool validateDateTime() const;
    std::string formatDateTimeString() const;
    
private:
    ITimeService* timeService;
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
    
    // 最小システム時刻設定（2025/01/01 00:00）
    static constexpr int MIN_SYSTEM_YEAR = 2025;
    static constexpr int MIN_SYSTEM_MONTH = 1;
    static constexpr int MIN_SYSTEM_DAY = 1;
    static constexpr int MIN_SYSTEM_HOUR = 0;
    static constexpr int MIN_SYSTEM_MINUTE = 0;
    static constexpr int MIN_SYSTEM_SECOND = 0;
    
    // デフォルト日時の桁配列：{2,0,2,5,0,1,0,1,0,0,0,0} (2025/01/01 00:00)
    static std::array<int, 12> getDefaultDateTimeDigits() {
        return {2, 0, 2, 5, 0, 1, 0, 1, 0, 0, 0, 0};
    }
    
    // 内部メソッド
    void resetDateTime();
    void incrementCurrentDigit();
    void moveCursorRight();
    void moveCursorLeft();
    static int dataPositionToStringPosition(int dataPos);
    bool validateYear() const;
    bool validateMonth() const;
    bool validateDay() const;
    bool validateHour() const;
    bool validateMinute() const;
    void commitDateTime();
    int getDigitValue(int position) const;
    void setDigitValue(int position, int value);
    static bool isLeapYear(int year);
    static int getDaysInMonth(int year, int month);
}; 