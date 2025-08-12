#pragma once

#include <vector>
#include <ctime>

// 先頭アラーム到達（消化）をフレーム間で検出する純粋ロジック
class AlarmRolloverDetector {
public:
    AlarmRolloverDetector() : prevNext_(0) {}

    // removePastAlarms 実行直後の alarms と now を与える
    // 直前の先頭 prevNext_ が now に到達し、先頭が差し替わっていれば true
    bool onFrame(const std::vector<time_t>& alarms, time_t now) {
        const time_t currNext = alarms.empty() ? 0 : alarms.front();
        const bool justElapsed = (prevNext_ > 0) && (now >= prevNext_) && (currNext != prevNext_);
        prevNext_ = currNext;
        return justElapsed;
    }

    // 明示的リセット
    void reset() { prevNext_ = 0; }

private:
    time_t prevNext_;
};


