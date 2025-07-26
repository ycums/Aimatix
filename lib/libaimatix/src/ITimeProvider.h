#pragma once

// 物理時間（現在時刻）取得インターフェース
class ITimeProvider {
public:
    virtual ~ITimeProvider() {}
    virtual time_t now() const = 0;
    virtual struct tm* localtime(time_t* time) const = 0;
}; 