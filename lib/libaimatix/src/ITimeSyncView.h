#pragma once

// Pure UI abstraction for Time Sync display.
// Implementations must live in src/ (e.g., TimeSyncViewImpl) to keep lib pure.
class ITimeSyncView {
public:
    virtual ~ITimeSyncView() {}

    // Title bar, e.g., "TIME SYNC | JOIN AP"
    virtual void showTitle(const char* text) = 0;

    // Three-slot button hints (A/B/C). Empty string allowed for unused slot.
    virtual void showHints(const char* hintA, const char* hintB, const char* hintC) = 0;
};


