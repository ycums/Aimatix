#pragma once

#include "StateManager.h"
#include <cstdint>
#include <vector>
#include "BacklightSequencer.h"
#include "IBacklight.h"
#include "ISettingsLogic.h"
#include "ui_constants.h"

// AlarmActiveState delegates visual alert to BacklightSequencer.
// It enqueues a fixed non-repeating pattern on enter, restores the
// pre-alarm brightness on finish or immediate stop, and returns to main state.
class AlarmActiveState : public IState {
public:
    AlarmActiveState(StateManager* manager,
                     IState* mainState,
                     BacklightSequencer* backlightSeq,
                     IBacklight* backlightOut,
                     ISettingsLogic* settings = nullptr)
        : manager_(manager),
          mainState_(mainState),
          backlightSeq_(backlightSeq),
          backlightOut_(backlightOut),
          settings_(settings),
          baselineBrightness_(DEFAULT_LCD_BRIGHTNESS),
          started_(false) {}

    void onEnter() override {
        // Capture baseline brightness to restore later (settings -> seq -> default)
        if (settings_) {
            const int s = settings_->getLcdBrightness();
            if (s >= 0 && s <= 255) {
                baselineBrightness_ = static_cast<uint8_t>(s);
            }
        } else if (backlightSeq_) {
            baselineBrightness_ = backlightSeq_->getLastBrightness();
        } else {
            baselineBrightness_ = DEFAULT_LCD_BRIGHTNESS;
        }

        // Build 1-second pattern @16fps:
        // [255x2f, 0x2f] x3 + [0x4f] = 16 frames
        if (backlightSeq_) {
            backlightSeq_->clear();
            for (int i = 0; i < 3; ++i) {
                backlightSeq_->enqueueStep(255, 2);
                backlightSeq_->enqueueStep(0, 2);
            }
            backlightSeq_->enqueueStep(0, 4);
            // Repeat 4 times total (4 seconds) by enqueuing the 1s pattern 3 more times
            for (int r = 0; r < 3; ++r) {
                for (int i = 0; i < 3; ++i) {
                    backlightSeq_->enqueueStep(255, 2);
                    backlightSeq_->enqueueStep(0, 2);
                }
                backlightSeq_->enqueueStep(0, 4);
            }
            backlightSeq_->setRepeat(false);
            backlightSeq_->start();
            started_ = true;
        }
    }

    void onExit() override {
        // Ensure baseline restoration when leaving the state for any reason
        restoreBaseline_();
        started_ = false;
    }

    void onDraw() override {
        // Auto-exit when sequence finished (tick is driven by main loop)
        if (started_ && backlightSeq_ && !backlightSeq_->isActive()) {
            restoreBaseline_();
            if (manager_ && mainState_) { manager_->setState(mainState_); }
            started_ = false;
        }
    }

    void onButtonA() override { immediateExit_(); }
    void onButtonB() override { immediateExit_(); }
    void onButtonC() override { immediateExit_(); }
    void onButtonALongPress() override { immediateExit_(); }
    void onButtonBLongPress() override { immediateExit_(); }
    void onButtonCLongPress() override { immediateExit_(); }

private:
    void restoreBaseline_() {
        if (backlightSeq_) {
            backlightSeq_->stop(backlightOut_);
        }
        if (backlightOut_) {
            backlightOut_->setBrightness(baselineBrightness_);
        }
    }

    void immediateExit_() {
        restoreBaseline_();
        if (manager_ && mainState_) { manager_->setState(mainState_); }
        started_ = false;
    }

    StateManager* manager_;
    IState* mainState_;
    BacklightSequencer* backlightSeq_;
    IBacklight* backlightOut_;
    ISettingsLogic* settings_;
    uint8_t baselineBrightness_;
    bool started_;
};

