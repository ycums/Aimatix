#include "DateTimeInputState.h"
#include "ui_constants.h"
#include <cassert>
#include <cstring>
#include <ctime>
#include <sstream>
#include <iomanip>

// 定数定義
constexpr int YEAR_BASE_OFFSET = 1000;
constexpr int MONTH_BASE_OFFSET = 100;
constexpr int DAY_BASE_OFFSET = 10;
constexpr int HOUR_BASE_OFFSET = 10;
constexpr int MINUTE_BASE_OFFSET = 10;
constexpr int YEAR_MIN = 2000;
constexpr int YEAR_MAX = 2099;
constexpr int MONTH_MIN = 1;
constexpr int MONTH_MAX = 12;
constexpr int DAY_MIN = 1;
constexpr int DAY_MAX = 31;
constexpr int HOUR_MIN = 0;
constexpr int HOUR_MAX = 23;
constexpr int MINUTE_MIN = 0;
constexpr int MINUTE_MAX = 59;
constexpr int FEBRUARY = 2;
constexpr int LEAP_YEAR_DIVISOR_4 = 4;
constexpr int LEAP_YEAR_DIVISOR_100 = 100;
constexpr int LEAP_YEAR_DIVISOR_400 = 400;
constexpr int DAYS_IN_FEBRUARY_NORMAL = 28;
constexpr int YEAR_1900 = 1900;
constexpr int YEAR_2024 = 2024;
constexpr int MONTH_TEN_MAX = 2;

void DateTimeInputState::onEnter() {
    resetDateTime();
    // 初期カーソル位置を年十の位に設定（年千の位、年百の位は入力不可のため）
    cursorPosition = DIGIT_YEAR_TEN;
    isEditMode = false;
    
    if (view != nullptr) {
        view->clear();
        view->showTitle("SET DATE/TIME", BATTERY_WARNING_THRESHOLD, false);
        view->showHints("INC", "NEXT", "SET");
        onDraw();
    }
}

void DateTimeInputState::onExit() {
    // 必要に応じてクリーンアップ処理
}

void DateTimeInputState::onDraw() {
    if (view != nullptr) {
        const std::string dateTimeStr = formatDateTimeString();
        const int stringPosition = dataPositionToStringPosition(cursorPosition);
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
        if (manager != nullptr && settingsDisplayState != nullptr) {
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
    if (manager != nullptr && settingsDisplayState != nullptr) {
        manager->setState(settingsDisplayState);
    }
}

void DateTimeInputState::resetDateTime() {
    if (timeProvider != nullptr) {
        // システム時刻から初期値を設定
        time_t currentTime = timeProvider->now();
        struct tm* timeInfo = timeProvider->localtime(&currentTime);
        
        if (timeInfo != nullptr) {
            const int year = timeInfo->tm_year + YEAR_1900;  // tm_yearは1900からのオフセット
            const int month = timeInfo->tm_mon + MONTH_MIN;     // tm_monは0ベース
            const int day = timeInfo->tm_mday;
            const int hour = timeInfo->tm_hour;
            const int minute = timeInfo->tm_min;
            
            // 各桁に分解して設定
            dateTimeDigits = {
                year / 1000,        // 年千の位
                (year / 100) % 10,  // 年百の位
                (year / 10) % 10,   // 年十の位
                year % 10,          // 年一の位
                month / 10,         // 月十の位
                month % 10,         // 月一の位
                day / 10,           // 日十の位
                day % 10,           // 日一の位
                hour / 10,          // 時十の位
                hour % 10,          // 時一の位
                minute / 10,        // 分十の位
                minute % 10         // 分一の位
            };
        } else {
            // timeInfoがnullの場合はデフォルト値を使用
            dateTimeDigits = {2, 0, 2, 4, 0, 1, 0, 1, 0, 0, 0, 0}; // 2024年1月1日00:00
        }
    } else {
        // timeProviderがnullの場合はデフォルト値を使用
        dateTimeDigits = {2, 0, 2, 4, 0, 1, 0, 1, 0, 0, 0, 0}; // 2024年1月1日00:00
    }
}

void DateTimeInputState::incrementCurrentDigit() {
    if (cursorPosition < 0 || cursorPosition >= static_cast<int>(dateTimeDigits.size())) {
        return;
    }
    
    // 年千の位、年百の位は入力不可
    if (cursorPosition == DIGIT_YEAR_THOUSAND || cursorPosition == DIGIT_YEAR_HUNDRED) {
        return;
    }
    
    int& currentDigit = dateTimeDigits[cursorPosition];
    
    // 現在の値を参照して動的に最大値を設定
    int maxValue = MAX_DIGIT_9; // デフォルト最大値
    
    if (cursorPosition == DIGIT_YEAR_TEN || cursorPosition == DIGIT_YEAR_ONE) { // 年十の位・年一の位: 2000-2099の範囲で0-9
        // maxValue = MAX_DIGIT_9; // デフォルト値と同じなので設定不要
    } else if (cursorPosition == DIGIT_MONTH_TEN) { // 月十の位: 01-12なので0-1
        maxValue = MAX_MONTH_TEN_DIGIT;
    } else if (cursorPosition == DIGIT_MONTH_ONE) { // 月一の位: 十の位に応じて決定
        if (dateTimeDigits[DIGIT_MONTH_TEN] == 0) {
            // maxValue = MAX_DIGIT_9; // デフォルト値と同じなので設定不要
        } else if (dateTimeDigits[DIGIT_MONTH_TEN] == 1) {
            maxValue = MONTH_TEN_MAX; // 10-12月
        }
        // else: maxValue = MAX_DIGIT_9; // デフォルト値と同じなので設定不要
    } else if (cursorPosition == DIGIT_DAY_TEN) { // 日十の位: 01-31なので0-3
        maxValue = MAX_DAY_TEN_DIGIT_OTHER;
    } else if (cursorPosition == DIGIT_DAY_ONE) { // 日一の位: 十の位と月に応じて決定
        // 現在の月を取得
        const int currentMonth = dateTimeDigits[DIGIT_MONTH_TEN] * 10 + dateTimeDigits[DIGIT_MONTH_ONE];
        int maxDaysInMonth = DAYS_IN_MONTH_31; // デフォルト
        
        // 月に応じた最大日数を設定
        if (currentMonth == 2) {
            // 2月: うるう年チェックは省略し、28日固定とする
            maxDaysInMonth = DAYS_IN_FEBRUARY;
        } else if (currentMonth == 4 || currentMonth == 6 || currentMonth == 9 || currentMonth == 11) {
            // 4月、6月、9月、11月は30日まで
            maxDaysInMonth = DAYS_IN_MONTH_30;
        }
        
        if (dateTimeDigits[DIGIT_DAY_TEN] == 0) {
            // maxValue = MAX_DIGIT_9; // デフォルト値と同じなので設定不要
        } else if (dateTimeDigits[DIGIT_DAY_TEN] == 1) {
            // maxValue = MAX_DIGIT_9; // デフォルト値と同じなので設定不要
        } else if (dateTimeDigits[DIGIT_DAY_TEN] == 2) {
            if (maxDaysInMonth >= DAYS_IN_FEBRUARY_LEAP) {
                // maxValue = MAX_DIGIT_9; // デフォルト値と同じなので設定不要
            } else {
                maxValue = 8; // 20-28日（2月の場合）
            }
        } else if (dateTimeDigits[DIGIT_DAY_TEN] == 3) {
            if (maxDaysInMonth == DAYS_IN_MONTH_31) {
                maxValue = 1; // 30-31日
            } else if (maxDaysInMonth == DAYS_IN_MONTH_30) {
                maxValue = 0; // 30日のみ
            } else {
                maxValue = 0; // 制限なし（実際には28/29日まで）
            }
        }
        // else: maxValue = MAX_DIGIT_9; // デフォルト値と同じなので設定不要
    } else if (cursorPosition == DIGIT_HOUR_TEN) { // 時十の位: 00-23なので0-2
        maxValue = MAX_HOUR_TEN_DIGIT;
    } else if (cursorPosition == DIGIT_HOUR_ONE) { // 時一の位: 十の位に応じて決定
        if (dateTimeDigits[DIGIT_HOUR_TEN] == 0 || dateTimeDigits[DIGIT_HOUR_TEN] == 1) {
            // maxValue = MAX_DIGIT_9; // デフォルト値と同じなので設定不要
        } else if (dateTimeDigits[DIGIT_HOUR_TEN] == 2) {
            maxValue = 3; // 20-23時
        }
        // else: maxValue = MAX_DIGIT_9; // デフォルト値と同じなので設定不要
    } else if (cursorPosition == DIGIT_MINUTE_TEN) { // 分十の位: 00-59なので0-5
        maxValue = MAX_MINUTE_TEN_DIGIT;
    }
    // else if (cursorPosition == DIGIT_MINUTE_ONE): maxValue = MAX_DIGIT_9; // デフォルト値と同じなので設定不要
    
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
        const int nextIndex = (currentIndex + 1) % numValidPositions;
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
        const int prevIndex = (currentIndex - 1 + numValidPositions) % numValidPositions;
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
    const int year = getDigitValue(0) * 1000 + getDigitValue(1) * 100 + getDigitValue(2) * 10 + getDigitValue(3);
    return year >= 2000 && year <= 2099;
}

bool DateTimeInputState::validateMonth() const {
    const int month = getDigitValue(4) * 10 + getDigitValue(5);
    return month >= 1 && month <= 12;
}

bool DateTimeInputState::validateDay() const {
    const int day = getDigitValue(6) * 10 + getDigitValue(7);
    if (day < 1 || day > 31) {
        return false;
    }
    
    // 月に応じた日数の制限をチェック
    const int year = getDigitValue(0) * 1000 + getDigitValue(1) * 100 + getDigitValue(2) * 10 + getDigitValue(3);
    const int month = getDigitValue(4) * 10 + getDigitValue(5);
    const int maxDays = getDaysInMonth(year, month);
    
    return day <= maxDays;
}

bool DateTimeInputState::validateHour() const {
    const int hour = getDigitValue(8) * 10 + getDigitValue(9);
    return hour >= 0 && hour <= 23;
}

bool DateTimeInputState::validateMinute() const {
    const int minute = getDigitValue(10) * 10 + getDigitValue(11);
    return minute >= 0 && minute <= 59;
}

void DateTimeInputState::commitDateTime() {
    if (timeProvider == nullptr || !validateDateTime()) {
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
    
    // システム時刻を更新
    bool success = timeProvider->setSystemTime(newTime);
    
    // 更新結果をログ出力（デバッグ用）
    if (success) {
        // 正常に更新された場合の処理
        // 実際のプロダクションでは必要に応じてログ出力
    }
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

int DateTimeInputState::dataPositionToStringPosition(int dataPos) {
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

bool DateTimeInputState::isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int DateTimeInputState::getDaysInMonth(int year, int month) {
    const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    if (month < 1 || month > 12) {
        return 31; // デフォルト値
    }
    
    if (month == 2 && isLeapYear(year)) {
        return 29;
    }
    
    return daysInMonth[month - 1];
} 