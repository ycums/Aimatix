#include "TimeSyncViewImpl.h"
#include "DisplayCommon.h"

void TimeSyncViewImpl::showTitle(const char* text) {
    if (!adapter_) return;
    // 画面初期化は入場時の1回のみ呼ばれる想定
    adapter_->clear();
    constexpr int BATTERY_LEVEL_PLACEHOLDER = 42;
    const bool IS_CHARGING_PLACEHOLDER = false;
    drawTitleBar(adapter_, text, BATTERY_LEVEL_PLACEHOLDER, IS_CHARGING_PLACEHOLDER);
}

void TimeSyncViewImpl::showHints(const char* hintA, const char* hintB, const char* hintC) {
    if (!adapter_) return;
    drawButtonHintsGrid(adapter_, hintA, hintB, hintC);
}


