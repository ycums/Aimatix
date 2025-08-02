#include "PartialInputLogic.h"
#include "ui_constants.h"
#include <cstdio>

auto PartialInputLogic::parsePartialInput(const int* digits, const bool* entered) -> ParsedTime {
    if (digits == nullptr || entered == nullptr) {
        return ParsedTime(0, 0, false);
    }
    
    // 完全未入力チェック（確定拒絶のため）
    bool hasAnyInput = false;
    for (int i = 0; i < DIGITS_COUNT; ++i) {
        if (entered[i]) {
            hasAnyInput = true;
            break;
        }
    }
    if (!hasAnyInput) {
        // 完全未入力の場合は無効とする（確定拒絶）
        return ParsedTime(0, 0, false);
    }
    
    int hour = 0;
    int minute = 0;
    bool hourSpecified = false;
    bool minuteSpecified = false;
    
    // 時の解釈（digits[0], digits[1]）
    if (entered[0] && entered[1]) {
        // 両方入力済み
        hour = digits[0] * HOURS_10 + digits[1];
        hourSpecified = true;
    } else if (!entered[0] && entered[1]) {
        // 時一桁のみ入力済み → その値を時一桁として解釈（修正）
        hour = digits[1];
        hourSpecified = true;
    } else {
        // 時が未入力
        hour = 0;
        hourSpecified = false;
    }
    
    // 分の解釈（digits[2], digits[3]）
    if (entered[2] && entered[3]) {
        // 両方入力済み
        minute = digits[2] * HOURS_10 + digits[3];
        minuteSpecified = true;
    } else if (!entered[2] && entered[3]) {
        // 分一桁のみ入力済み → 直感的に分一桁として解釈
        minute = digits[3];
        minuteSpecified = true;
    } else {
        // 分が未入力
        minute = 0;
        minuteSpecified = false;
    }
    
    bool valid = isValidTime(hour, minute);
    return ParsedTime(hour, minute, valid, hourSpecified, minuteSpecified);
}

auto PartialInputLogic::formatTime(int hour, int minute) -> std::string {
    char buffer[STRING_BUFFER_SIZE] = {};
    const int result = std::snprintf(buffer, sizeof(buffer), "%02d:%02d", hour, minute);
    if (result < 0 || static_cast<size_t>(result) >= sizeof(buffer)) {
        return std::string(); // エラー時は空文字列を返す
    }
    return std::string(buffer);
}

auto PartialInputLogic::isValidTime(int hour, int minute) -> bool {
    return hour >= 0 && hour < HOURS_24 && minute >= 0 && minute < MINUTES_60;
}