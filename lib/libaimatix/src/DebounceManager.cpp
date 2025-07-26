#include "DebounceManager.h"

DebounceManager::DebounceManager(uint32_t debounce_ms)
    : debounceMs(debounce_ms) {}

bool DebounceManager::update(bool raw, uint32_t now_ms) {
    if (raw != lastRaw) {
        lastRaw = raw;
        lastChange = now_ms;
    }
    if ((now_ms - lastChange) >= debounceMs && stable != raw) {
        stable = raw;
    }
    return stable;
}

bool DebounceManager::getStable() const {
    return stable;
}

uint32_t DebounceManager::getLastChange() const {
    return lastChange;
} 