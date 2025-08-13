#pragma once

// Pure UI abstraction for Time Sync display.
// Implementations must live in src/ (e.g., TimeSyncViewImpl) to keep lib pure.
class ITimeSyncView {
public:
    virtual ~ITimeSyncView() {}

    // Title bar, e.g., "TIME SYNC > JOIN AP"
    virtual void showTitle(const char* text) = 0;

    // Three-slot button hints (A/B/C). Empty string allowed for unused slot.
    virtual void showHints(const char* hintA, const char* hintB, const char* hintC) = 0;

    // Draw Wi‑Fi QR payload (e.g., WIFI:T:WPA;S:...;P:...;H:false;;)
    virtual void showWifiQr(const char* payload) = 0;

    // Draw URL QR payload (e.g., http://192.168.4.1/sync?t=...)
    virtual void showUrlQr(const char* payload) = 0;

    // Show short error message in the content area
    virtual void showError(const char* message) = 0;
};


