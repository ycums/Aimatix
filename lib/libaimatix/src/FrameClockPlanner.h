#pragma once

#include <cstdint>

// Pure logic frame clock planner.
// Computes the next integer millisecond delay per frame to approximate a
// given frame interval in microseconds under a tick rate (e.g., 1000 Hz).
// FreeRTOS/Arduino independent.
class FrameClockPlanner {
public:
	FrameClockPlanner(uint32_t frameIntervalUs, uint32_t tickRateHz)
		: frameIntervalUs_(frameIntervalUs), tickRateHz_(tickRateHz), accumulatedUs_(0), emittedMsTotal_(0) {}

	void reset() {
		accumulatedUs_ = 0;
		emittedMsTotal_ = 0;
	}

	// Returns the next delay in integer milliseconds per frame.
	uint32_t nextDelayMs() {
		accumulatedUs_ += static_cast<uint64_t>(frameIntervalUs_);
		const uint64_t totalMs = accumulatedUs_ / 1000ULL; // floor
		const uint32_t stepMs = static_cast<uint32_t>(totalMs - emittedMsTotal_);
		emittedMsTotal_ = totalMs;
		return stepMs;
	}

private:
	uint32_t frameIntervalUs_;
	uint32_t tickRateHz_;
	uint64_t accumulatedUs_;
	uint64_t emittedMsTotal_;
};


