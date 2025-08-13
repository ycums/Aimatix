#include <unity.h>
#include "FrameClockPlanner.h"

void setUp(void) {}
void tearDown(void) {}

static void test_fcp_emits_approximate_intervals_62_63_ms(void) {
	FrameClockPlanner fcp(62500, 1000);
	const uint32_t s1 = fcp.nextDelayMs();
	const uint32_t s2 = fcp.nextDelayMs();
	TEST_ASSERT_EQUAL_UINT32(62, s1);
	TEST_ASSERT_EQUAL_UINT32(63, s2);
}

static void test_fcp_cumulative_ms_matches_expected_over_10_frames(void) {
	FrameClockPlanner fcp(62500, 1000);
	uint32_t sum = 0;
	for (int i = 0; i < 10; ++i) sum += fcp.nextDelayMs();
	TEST_ASSERT_EQUAL_UINT32(625, sum); // 10 * 62.5ms
}

static void test_fcp_reset_restores_first_step_to_62_ms(void) {
	FrameClockPlanner fcp(62500, 1000);
	(void)fcp.nextDelayMs();
	(void)fcp.nextDelayMs();
	fcp.reset();
	TEST_ASSERT_EQUAL_UINT32(62, fcp.nextDelayMs());
}

int main(int, char**) {
	UNITY_BEGIN();
	RUN_TEST(test_fcp_emits_approximate_intervals_62_63_ms);
	RUN_TEST(test_fcp_cumulative_ms_matches_expected_over_10_frames);
	RUN_TEST(test_fcp_reset_restores_first_step_to_62_ms);
	return UNITY_END();
}


