#pragma once

#include <cstdint>
#include <vector>
#include "IBacklight.h"

/**
 * BacklightSequencer updates brightness only on frame boundaries.
 * It is pure logic and assumes caller invokes tick() at a fixed frame cadence
 * (e.g., 16 fps). When non-repeating sequence finishes, it keeps the last
 * brightness (do not change output further).
 */
class BacklightSequencer {
public:
	struct Step {
		uint8_t brightness;       // 0-255
		uint16_t durationFrames;  // number of frames to hold this brightness
	};

	BacklightSequencer()
		: repeat_(false), active_(false), currentIndex_(0), framesLeftInStep_(0), lastBrightness_(0) {}

	void clear() {
		pattern_.clear();
		active_ = false;
		currentIndex_ = 0;
		framesLeftInStep_ = 0;
	}

	void enqueueStep(uint8_t brightness, uint16_t durationFrames) {
		pattern_.push_back(Step{brightness, durationFrames});
	}

	void enqueuePattern(const std::vector<Step>& steps, bool repeat) {
		for (const auto& s : steps) pattern_.push_back(s);
		repeat_ = repeat;
	}

	void setRepeat(bool repeat) { repeat_ = repeat; }
	bool isActive() const { return active_; }
	int getCurrentStepIndex() const { return active_ ? static_cast<int>(currentIndex_) : -1; }
	uint16_t getRemainingFramesInStep() const { return framesLeftInStep_; }
	uint8_t getLastBrightness() const { return lastBrightness_; }

	// Start sequence from the beginning. Keeps lastBrightness_ as "pre-sequence" fallback.
	void start() {
		if (pattern_.empty()) { active_ = false; return; }
		active_ = true;
		currentIndex_ = 0;
		framesLeftInStep_ = pattern_[0].durationFrames;
	}

	void stop(IBacklight* /*output*/) {
		active_ = false;
		// Do not change brightness on stop (keep lastBrightness_)
	}

	// Called exactly once per frame boundary.
	void tick(IBacklight* output) {
		if (!output) return;
		if (!active_) return; // keep last brightness
		if (pattern_.empty()) { active_ = false; return; }

		// On first frame of a step (framesLeftInStep_ just set or >0), ensure output is set.
		const Step& step = pattern_[currentIndex_];
		if (framesLeftInStep_ == step.durationFrames) {
			output->setBrightness(step.brightness);
			lastBrightness_ = step.brightness;
		}

		// Consume one frame
		if (framesLeftInStep_ > 0) {
			--framesLeftInStep_;
		}

		// If finished this step, advance
		if (framesLeftInStep_ == 0) {
			// Move to next step
			++currentIndex_;
			if (currentIndex_ >= pattern_.size()) {
				if (repeat_ && !pattern_.empty()) {
					currentIndex_ = 0;
					framesLeftInStep_ = pattern_[0].durationFrames;
					// Will set brightness at next tick's entry of the step
				} else {
					active_ = false; // keep lastBrightness_ as decided
					return;
				}
			} else {
				framesLeftInStep_ = pattern_[currentIndex_].durationFrames;
			}
		}
	}

private:
	std::vector<Step> pattern_;
	bool repeat_;
	bool active_;
	size_t currentIndex_;
	uint16_t framesLeftInStep_;
	uint8_t lastBrightness_;
};


