#include "mock_speaker.h"

MockSpeaker::MockSpeaker() {}
MockSpeaker::~MockSpeaker() {}

void MockSpeaker::beep(int freq, int durationMs) {
    lastFreq = freq;
    lastDuration = durationMs;
    stopped = false;
}

void MockSpeaker::stop() {
    stopped = true;
} 