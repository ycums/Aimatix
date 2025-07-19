#pragma once
#include "../../lib/libaimatix/include/ISpeaker.h"

// 汎用的なモックSpeakerクラス
class MockSpeaker : public ISpeaker {
public:
    bool beep_called = false;
    bool stop_called = false;
    int last_freq = 0;
    int last_duration = 0;
    
    void beep(int freq, int durationMs) override {
        beep_called = true;
        last_freq = freq;
        last_duration = durationMs;
    }
    
    void stop() override {
        stop_called = true;
    }
    
    void reset() {
        beep_called = false;
        stop_called = false;
        last_freq = 0;
        last_duration = 0;
    }
}; 