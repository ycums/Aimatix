#pragma once
#include "../../lib/aimatix_lib/include/ISpeaker.h"

class MockSpeaker : public ISpeaker {
public:
    MockSpeaker();
    ~MockSpeaker() override;
    void beep(int freq, int durationMs) override;
    void stop() override;
    // テスト用: 最後に鳴らした音情報
    int lastFreq = 0;
    int lastDuration = 0;
    bool stopped = false;
}; 