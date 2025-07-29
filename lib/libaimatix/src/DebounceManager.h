#pragma once
#include <cstdint>

class DebounceManager {
public:
    DebounceManager(uint32_t debounce_ms = 50);
    // 入力値を更新し、デバウンス後の安定値を返す
    bool update(bool raw, uint32_t now_ms);
    // 現在の安定値を取得
    bool getStable() const;
    // 最後に安定値が変化した時刻
    uint32_t getLastChange() const;
private:
    bool stable = false;
    bool lastRaw = false;
    uint32_t lastChange = 0;
    uint32_t debounceMs = 50;
}; 