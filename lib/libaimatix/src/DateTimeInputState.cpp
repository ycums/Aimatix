#include "DateTimeInputState.h"
#include <cassert>
#include <cstring>
#include <ctime>
#include <sstream>
#include <iomanip>

void DateTimeInputState::onEnter() {
    resetDateTime();
    // 初期カーソル位置を年十の位に設定（年千の位、年百の位は入力不可のため）
    cursorPosition = 2;
    isEditMode = false;
    
    if (view) {
        view->clear();
        constexpr int BATTERY_WARNING_THRESHOLD = 42;
        view->showTitle("SET DATE/TIME", BATTERY_WARNING_THRESHOLD, false);
        view->showHints("INC", "NEXT", "SET");
        onDraw();
    }
}

void DateTimeInputState::onExit() {
    // 必要に応じてクリーンアップ処理
}

void DateTimeInputState::onDraw() {
    if (view) {
        std::string dateTimeStr = formatDateTimeString();
        int stringPosition = dataPositionToStringPosition(cursorPosition);
        view->showDateTimeString(dateTimeStr, stringPosition);
    }
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
    
    // 年千の位、年百の位は入力不可
    if (cursorPosition == 0 || cursorPosition == 1) {
        return;
    }
    
    int& currentDigit = dateTimeDigits[cursorPosition];
    
    // 現在の値を参照して動的に最大値を設定
    int maxValue = 9; // デフォルト最大値
    
    if (cursorPosition == 2) { // 年十の位: 2000-2099の範囲で0-9
        maxValue = 9;
    } else if (cursorPosition == 3) { // 年一の位: 2000-2099の範囲で0-9
        maxValue = 9;
    } else if (cursorPosition == 4) { // 月十の位: 01-12なので0-1
        maxValue = 1;
    } else if (cursorPosition == 5) { // 月一の位: 十の位に応じて決定
        if (dateTimeDigits[4] == 0) {
            maxValue = 9; // 01-09月
        } else if (dateTimeDigits[4] == 1) {
            maxValue = 2; // 10-12月
        } else {
            maxValue = 9;
        }
    } else if (cursorPosition == 6) { // 日十の位: 01-31なので0-3
        maxValue = 3;
    } else if (cursorPosition == 7) { // 日一の位: 十の位と月に応じて決定
        // 現在の月を取得
        int currentMonth = dateTimeDigits[4] * 10 + dateTimeDigits[5];
        int maxDaysInMonth = 31; // デフォルト
        
        // 月に応じた最大日数を設定
        if (currentMonth == 2) {
            // 2月: うるう年チェックは省略し、28日固定とする
            maxDaysInMonth = 28;
        } else if (currentMonth == 4 || currentMonth == 6 || currentMonth == 9 || currentMonth == 11) {
            // 4月、6月、9月、11月は30日まで
            maxDaysInMonth = 30;
        }
        
        if (dateTimeDigits[6] == 0) {
            maxValue = 9; // 01-09日
        } else if (dateTimeDigits[6] == 1) {
            maxValue = 9; // 10-19日
        } else if (dateTimeDigits[6] == 2) {
            if (maxDaysInMonth >= 29) {
                maxValue = 9; // 20-29日
            } else {
                maxValue = 8; // 20-28日（2月の場合）
            }
        } else if (dateTimeDigits[6] == 3) {
            if (maxDaysInMonth == 31) {
                maxValue = 1; // 30-31日
            } else if (maxDaysInMonth == 30) {
                maxValue = 0; // 30日のみ
            } else {
                maxValue = 0; // 制限なし（実際には28/29日まで）
            }
        } else {
            maxValue = 9;
        }
    } else if (cursorPosition == 8) { // 時十の位: 00-23なので0-2
        maxValue = 2;
    } else if (cursorPosition == 9) { // 時一の位: 十の位に応じて決定
        if (dateTimeDigits[8] == 0 || dateTimeDigits[8] == 1) {
            maxValue = 9; // 00-19時
        } else if (dateTimeDigits[8] == 2) {
            maxValue = 3; // 20-23時
        } else {
            maxValue = 9;
        }
    } else if (cursorPosition == 10) { // 分十の位: 00-59なので0-5
        maxValue = 5;
    } else if (cursorPosition == 11) { // 分一の位: 00-59なので0-9
        maxValue = 9;
    }
    
    // 値をインクリメント（最大値を超えたら0に戻る）
    currentDigit = (currentDigit + 1) % (maxValue + 1);
}

void DateTimeInputState::moveCursorRight() {
    // 入力可能な位置のリスト（年千の位、年百の位、記号は除く）
    const int validPositions[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11}; // 年十の位から分一の位まで
    const int numValidPositions = sizeof(validPositions) / sizeof(validPositions[0]);
    
    // 現在位置のインデックスを取得
    int currentIndex = -1;
    for (int i = 0; i < numValidPositions; ++i) {
        if (validPositions[i] == cursorPosition) {
            currentIndex = i;
            break;
        }
    }
    
    // 次の有効位置に移動（循環）
    if (currentIndex >= 0) {
        int nextIndex = (currentIndex + 1) % numValidPositions;
        cursorPosition = validPositions[nextIndex];
    } else {
        // 現在位置が無効の場合、最初の有効位置に移動
        cursorPosition = validPositions[0];
    }
}

void DateTimeInputState::moveCursorLeft() {
    // 入力可能な位置のリスト（年千の位、年百の位、記号は除く）
    const int validPositions[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11}; // 年十の位から分一の位まで
    const int numValidPositions = sizeof(validPositions) / sizeof(validPositions[0]);
    
    // 現在位置のインデックスを取得
    int currentIndex = -1;
    for (int i = 0; i < numValidPositions; ++i) {
        if (validPositions[i] == cursorPosition) {
            currentIndex = i;
            break;
        }
    }
    
    // 前の有効位置に移動（循環）
    if (currentIndex >= 0) {
        int prevIndex = (currentIndex - 1 + numValidPositions) % numValidPositions;
        cursorPosition = validPositions[prevIndex];
    } else {
        // 現在位置が無効の場合、最初の有効位置に移動
        cursorPosition = validPositions[0];
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

int DateTimeInputState::dataPositionToStringPosition(int dataPos) const {
    // データ位置から表示文字列位置への変換
    // データ: [年千,年百,年十,年一,月十,月一,日十,日一,時十,時一,分十,分一]
    // 文字列: "2025/01/01 00:00"
    //         0123456789012345
    const int stringPositions[] = {
        0,  // 年千の位 -> 0
        1,  // 年百の位 -> 1
        2,  // 年十の位 -> 2
        3,  // 年一の位 -> 3
        5,  // 月十の位 -> 5 ("/の後")
        6,  // 月一の位 -> 6
        8,  // 日十の位 -> 8 ("/の後")
        9,  // 日一の位 -> 9
        11, // 時十の位 -> 11 (" の後")
        12, // 時一の位 -> 12
        14, // 分十の位 -> 14 (":の後")
        15  // 分一の位 -> 15
    };
    
    if (dataPos >= 0 && dataPos < 12) {
        return stringPositions[dataPos];
    }
    return 0;
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