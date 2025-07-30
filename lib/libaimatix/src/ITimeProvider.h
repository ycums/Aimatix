#pragma once
#include <ctime>

// 物理時間（現在時刻）取得・設定インターフェース
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
class ITimeProvider {
public:
    virtual ~ITimeProvider() {}
    virtual time_t now() const = 0;
    virtual struct tm* localtime(time_t* time) const = 0;
    virtual bool setSystemTime(time_t time) = 0;
}; 