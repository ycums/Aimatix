#pragma once
#include <stdint.h>
#include <vector>
#include "IVibration.h"

/**
 * VibrationSequencer drives a vibration output according to a sequence
 * of time segments (durationMs, dutyPercent). This module is pure logic
 * and has no dependency on Arduino/M5.
 */
class VibrationSequencer {
public:
	struct Segment {
		uint32_t durationMs;
		uint8_t dutyPercent; // 0-100
	};

	VibrationSequencer() : repeat_(false), active_(false), startMs_(0) {}

	void loadPattern(const std::vector<Segment>& pattern, bool repeat) {
		pattern_ = pattern;
		repeat_ = repeat;
	}

	void start(uint32_t nowMs) {
		if (pattern_.empty()) {
			active_ = false;
			return;
		}
		startMs_ = nowMs;
		active_ = true;
	}

	void stop(IVibrationOutput* output) {
		active_ = false;
		if (output) {
			output->setDutyPercent(0);
		}
	}

	void update(uint32_t nowMs, IVibrationOutput* output) {
		if (!active_ || pattern_.empty() || output == nullptr) return;

		uint32_t elapsed = nowMs - startMs_;
		uint32_t total = totalDurationMs();
		if (!repeat_ && elapsed >= total) {
			active_ = false;
			output->setDutyPercent(0);
			return;
		}

		if (repeat_ && total > 0) {
			elapsed %= total;
		}

		uint32_t acc = 0;
		for (const auto& seg : pattern_) {
			if (elapsed < acc + seg.durationMs) {
				output->setDutyPercent(seg.dutyPercent);
				return;
			}
			acc += seg.durationMs;
		}

		output->setDutyPercent(0);
	}

	bool isActive() const { return active_; }

	uint32_t totalDurationMs() const {
		uint32_t sum = 0;
		for (const auto& s : pattern_) sum += s.durationMs;
		return sum;
	}

private:
	std::vector<Segment> pattern_;
	bool repeat_;
	bool active_;
	uint32_t startMs_;
};


