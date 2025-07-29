#include "DateTimeInputState.h"
#include <cassert>
#include <cstring>
#include <ctime>
#include <sstream>
#include <iomanip>

void DateTimeInputState::onEnter() {
    resetDateTime();
    // 初期カーソル位置を年千の位に設定
    cursorPosition = 0;
    isEditMode = false;
}

void DateTimeInputState::onExit() {
    // 必要に応じてクリーンアップ処理
}

void DateTimeInputState::onDraw() {
    // この実装では、描画は外部のViewクラスに委譲される
    // 実際の描画処理は、Viewクラスがこの状態クラスのデータを参照して行う
}

void DateTimeInputState::onButtonA() {
    // +1ボタン（短押し）
    incrementCurrentDigit();
}

void DateTimeInputState::onButtonB() {
    // カーソル移動ボタン（短押し）
    moveCursorRight();
}

void DateTimeInputState::onButtonC() {
    // 確定ボタン（短押し）
    if (validateDateTime()) {
        commitDateTime();
        // 設定画面に戻る
        if (manager && settingsDisplayState) {
            manager->setState(settingsDisplayState);
        }
    }
}

void DateTimeInputState::onButtonALongPress() {
    // 長押しは未実装
}

void DateTimeInputState::onButtonBLongPress() {
    // 長押しは未実装
}

void DateTimeInputState::onButtonCLongPress() {
    // 戻るボタン（長押し）
    if (manager && settingsDisplayState) {
        manager->setState(settingsDisplayState);
    }
}

void DateTimeInputState::resetDateTime() {
    // 初期値: 2025/01/01 00:00
    dateTimeDigits = {2, 0, 2, 5, 0, 1, 0, 1, 0, 0, 0, 0};
}

void DateTimeInputState::incrementCurrentDigit() {
    if (cursorPosition < 0 || cursorPosition >= static_cast<int>(dateTimeDigits.size())) {
        return;
    }
    
    int& currentDigit = dateTimeDigits[cursorPosition];
    
    // 位置に応じて最大値を設定
    int maxValue = 9;
    if (cursorPosition == 0) { // 年千の位
        maxValue = 2;
    } else if (cursorPosition == 1) { // 年百の位
        maxValue = 0;
    } else if (cursorPosition == 2) { // 年十の位
        maxValue = 3;
    } else if (cursorPosition == 3) { // 年一の位
        maxValue = 0;
    } else if (cursorPosition == 4) { // 月十の位
        maxValue = 1;
    } else if (cursorPosition == 5) { // 月一の位
        maxValue = 2;
    } else if (cursorPosition == 6) { // 日十の位
        maxValue = 3;
    } else if (cursorPosition == 7) { // 日一の位
        maxValue = 1;
    } else if (cursorPosition == 8) { // 時十の位
        maxValue = 2;
    } else if (cursorPosition == 9) { // 時一の位
        maxValue = 3;
    } else if (cursorPosition == 10) { // 分十の位
        maxValue = 5;
    } else if (cursorPosition == 11) { // 分一の位
        maxValue = 9;
    }
    
    // 値をインクリメント（最大値を超えたら0に戻る）
    currentDigit = (currentDigit + 1) % (maxValue + 1);
}

void DateTimeInputState::moveCursorRight() {
    if (cursorPosition < static_cast<int>(dateTimeDigits.size()) - 1) {
        cursorPosition++;
    }
}

void DateTimeInputState::moveCursorLeft() {
    if (cursorPosition > 0) {
        cursorPosition--;
    }
}

bool DateTimeInputState::validateDateTime() const {
    return validateYear() && validateMonth() && validateDay() && validateHour() && validateMinute();
}

bool DateTimeInputState::validateYear() const {
    int year = getDigitValue(0) * 1000 + getDigitValue(1) * 100 + getDigitValue(2) * 10 + getDigitValue(3);
    return year >= YEAR_MIN && year <= YEAR_MAX;
}

bool DateTimeInputState::validateMonth() const {
    int month = getDigitValue(4) * 10 + getDigitValue(5);
    return month >= MONTH_MIN && month <= MONTH_MAX;
}

bool DateTimeInputState::validateDay() const {
    int day = getDigitValue(6) * 10 + getDigitValue(7);
    if (day < DAY_MIN || day > DAY_MAX) {
        return false;
    }
    
    // 月に応じた日数の制限をチェック
    int year = getDigitValue(0) * 1000 + getDigitValue(1) * 100 + getDigitValue(2) * 10 + getDigitValue(3);
    int month = getDigitValue(4) * 10 + getDigitValue(5);
    int maxDays = getDaysInMonth(year, month);
    
    return day <= maxDays;
}

bool DateTimeInputState::validateHour() const {
    int hour = getDigitValue(8) * 10 + getDigitValue(9);
    return hour >= HOUR_MIN && hour <= HOUR_MAX;
}

bool DateTimeInputState::validateMinute() const {
    int minute = getDigitValue(10) * 10 + getDigitValue(11);
    return minute >= MINUTE_MIN && minute <= MINUTE_MAX;
}

void DateTimeInputState::commitDateTime() {
    if (!timeProvider || !validateDateTime()) {
        return;
    }
    
    // 入力された日時をtime_tに変換
    struct tm timeInfo = {};
    
    timeInfo.tm_year = (getDigitValue(0) * 1000 + getDigitValue(1) * 100 + 
                       getDigitValue(2) * 10 + getDigitValue(3)) - 1900;
    timeInfo.tm_mon = (getDigitValue(4) * 10 + getDigitValue(5)) - 1;
    timeInfo.tm_mday = getDigitValue(6) * 10 + getDigitValue(7);
    timeInfo.tm_hour = getDigitValue(8) * 10 + getDigitValue(9);
    timeInfo.tm_min = getDigitValue(10) * 10 + getDigitValue(11);
    timeInfo.tm_sec = 0;
    timeInfo.tm_isdst = -1; // 自動判定
    
    // time_tに変換
    time_t newTime = mktime(&timeInfo);
    
    // システム時刻を更新（この実装ではITimeProviderに更新機能がないため、
    // 実際の更新は外部で行う必要がある）
    // ここでは、更新可能なITimeProviderの実装を想定
}

std::string DateTimeInputState::formatDateTimeString() const {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(4) 
        << (getDigitValue(0) * 1000 + getDigitValue(1) * 100 + getDigitValue(2) * 10 + getDigitValue(3));
    oss << "/";
    oss << std::setfill('0') << std::setw(2) 
        << (getDigitValue(4) * 10 + getDigitValue(5));
    oss << "/";
    oss << std::setfill('0') << std::setw(2) 
        << (getDigitValue(6) * 10 + getDigitValue(7));
    oss << " ";
    oss << std::setfill('0') << std::setw(2) 
        << (getDigitValue(8) * 10 + getDigitValue(9));
    oss << ":";
    oss << std::setfill('0') << std::setw(2) 
        << (getDigitValue(10) * 10 + getDigitValue(11));
    
    return oss.str();
}

int DateTimeInputState::getDigitValue(int position) const {
    if (position >= 0 && position < static_cast<int>(dateTimeDigits.size())) {
        return dateTimeDigits[position];
    }
    return 0;
}

void DateTimeInputState::setDigitValue(int position, int value) {
    if (position >= 0 && position < static_cast<int>(dateTimeDigits.size())) {
        dateTimeDigits[position] = value;
    }
}

bool DateTimeInputState::isLeapYear(int year) const {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int DateTimeInputState::getDaysInMonth(int year, int month) const {
    const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    if (month < 1 || month > 12) {
        return 31; // デフォルト値
    }
    
    if (month == 2 && isLeapYear(year)) {
        return 29;
    }
    
    return daysInMonth[month - 1];
} 