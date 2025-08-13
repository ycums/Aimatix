#include "DebounceManager.h"

DebounceManager::DebounceManager(uint32_t debounce_ms)
    : debounceMs(debounce_ms) {}

auto DebounceManager::update(bool raw, uint32_t now_ms) -> bool {
    if (raw != lastRaw) {
        lastRaw = raw;
        lastChange = now_ms;
    }
    if ((now_ms - lastChange) >= debounceMs && stable != raw) {
        stable = raw;
    }
    return stable;
}

auto DebounceManager::getStable() const -> bool {
    return stable;
}

auto DebounceManager::getLastChange() const -> uint32_t {
    return lastChange;
} 