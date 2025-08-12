#pragma once

#include <cstdint>

// AlarmFlashScheduler
// - calls=4（合計4000ms）
// - 1拍=250ms
//   - 拍1〜3: 125ms ON → 125ms OFF
//   - 拍4: 250ms OFF
// - 時間ベースで進行し、境界誤差に強い
class AlarmFlashScheduler {
public:
    AlarmFlashScheduler() : startMs_(0), started_(false), finished_(false) {}

    // 鳴動開始時刻を設定
    void begin(uint32_t startMs) {
        startMs_ = startMs;
        started_ = true;
        finished_ = false;
    }

    // 経過ミリ秒を与えてON/OFFを返す
    // 終了後は常にOFF
    bool update(uint32_t elapsed) {
        if (!started_) {
            return false;
        }
        if (elapsed >= kTotalDurationMs) {
            finished_ = true;
            return false;
        }
        const uint32_t withinSecond = elapsed % 1000U;
        if (withinSecond < 750U) {
            // 拍1〜3の範囲
            return (withinSecond % 250U) < 125U;
        }
        // 拍4（休止）
        return false;
    }

    bool isFinished() const {
        return started_ && finished_;
    }

    void reset() {
        startMs_ = 0;
        started_ = false;
        finished_ = false;
    }

    static constexpr uint32_t kCalls = 4U;
    static constexpr uint32_t kBeatMs = 250U;
    static constexpr uint32_t kTotalDurationMs = kCalls * 1000U; // 4000ms

private:
    uint32_t startMs_;
    bool started_;
    bool finished_;
};


