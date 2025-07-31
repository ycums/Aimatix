#include "PartialInputLogic.h"
#include <cstdio>

PartialInputLogic::ParsedTime PartialInputLogic::parsePartialInput(const int* digits, const bool* entered) {
    if (digits == nullptr || entered == nullptr) {
        return ParsedTime(0, 0, false);
    }
    
    // 完全未入力チェック（確定拒絶のため）
    bool hasAnyInput = false;
    for (int i = 0; i < 4; ++i) {
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
    
    // 時の解釈（digits[0], digits[1]）
    if (entered[0] && entered[1]) {
        // 両方入力済み
        hour = digits[0] * HOURS_10 + digits[1];
    } else if (entered[0] && !entered[1]) {
        // 時十桁のみ入力済み → 時一桁を0として補完
        hour = digits[0] * HOURS_10 + 0;
    } else if (!entered[0] && entered[1]) {
        // 時一桁のみ入力済み → その値を時十桁として解釈、時一桁は0
        hour = digits[1] * HOURS_10 + 0;
    } else {
        // 時が未入力
        hour = 0;
    }
    
    // 分の解釈（digits[2], digits[3]）
    if (entered[2] && entered[3]) {
        // 両方入力済み
        minute = digits[2] * HOURS_10 + digits[3];
    } else if (entered[2] && !entered[3]) {
        // 分十桁のみ入力済み → 分一桁を0として補完
        minute = digits[2] * HOURS_10 + 0;
    } else if (!entered[2] && entered[3]) {
        // 分一桁のみ入力済み → その値を分十桁として解釈、分一桁は0
        minute = digits[3] * HOURS_10 + 0;
    } else {
        // 分が未入力
        minute = 0;
    }
    
    bool valid = isValidTime(hour, minute);
    return ParsedTime(hour, minute, valid);
}

std::string PartialInputLogic::formatTime(int hour, int minute) {
    char buffer[16];
    std::snprintf(buffer, sizeof(buffer), "%02d:%02d", hour, minute);
    return std::string(buffer);
}

bool PartialInputLogic::isValidTime(int hour, int minute) {
    return hour >= 0 && hour < HOURS_24 && minute >= 0 && minute < MINUTES_60;
}