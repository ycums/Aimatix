#include <unity.h>
#include "BacklightSequencer.h"

class MockBacklight : public IBacklight {
public:
	MockBacklight() : lastBrightness(0), calls(0) {}
	void setBrightness(uint8_t b) override { lastBrightness = b; ++calls; }
	uint8_t lastBrightness;
	int calls;
};

void setUp(void) {}
void tearDown(void) {}

int main(int, char**) {
	UNITY_BEGIN();

	auto test_single_step_three_frames = [](){
		BacklightSequencer seq; MockBacklight out;
		seq.enqueueStep(128, 3);
		seq.start();
		seq.tick(&out); // frame 1
		TEST_ASSERT_EQUAL_UINT8(128, out.lastBrightness);
		seq.tick(&out); // frame 2
		TEST_ASSERT_EQUAL_UINT8(128, out.lastBrightness);
		seq.tick(&out); // frame 3
		TEST_ASSERT_EQUAL_UINT8(128, out.lastBrightness);
		// finished; next tick should keep last (no new call required by spec, but our impl sets only on step entry)
		seq.tick(&out); // after completion: inactive; last stays 128
		TEST_ASSERT_FALSE(seq.isActive());
		TEST_ASSERT_EQUAL_UINT8(128, seq.getLastBrightness());
	};

	auto test_blink_oneshot = [](){
		BacklightSequencer seq; MockBacklight out;
		seq.enqueueStep(255, 2);
		seq.enqueueStep(0, 2);
		seq.start();
		seq.tick(&out); // 255
		seq.tick(&out); // hold 255
		seq.tick(&out); // 0
		TEST_ASSERT_EQUAL_UINT8(0, out.lastBrightness);
		seq.tick(&out); // hold 0, then end
		TEST_ASSERT_FALSE(seq.isActive());
		TEST_ASSERT_EQUAL_UINT8(0, seq.getLastBrightness());
	};

	auto test_repeat_pattern = [](){
		BacklightSequencer seq; MockBacklight out;
		seq.enqueuePattern({ {10,1}, {20,1} }, true);
		seq.start();
		seq.tick(&out); // 10
		TEST_ASSERT_EQUAL_UINT8(10, out.lastBrightness);
		seq.tick(&out); // 20
		TEST_ASSERT_EQUAL_UINT8(20, out.lastBrightness);
		seq.tick(&out); // loop to 10
		TEST_ASSERT_TRUE(seq.isActive());
		TEST_ASSERT_EQUAL_UINT8(10, out.lastBrightness);
	};

	RUN_TEST(test_single_step_three_frames);
	RUN_TEST(test_blink_oneshot);
	RUN_TEST(test_repeat_pattern);

	return UNITY_END();
}


