#include <unity.h>
#include "StateManager.h"
#include "MainDisplayState.h"
#include "InputDisplayState.h"
#include "InputLogic.h"
#include <cstdio>
#include <cstring>
#include <vector>
#include <ctime>
#include "ITimeService.h"
#include <memory>

extern std::vector<time_t> alarm_times;

const time_t kFixedTestTime = 1700000000;
struct MockTimeService : public ITimeService {
    time_t n; uint32_t ms{0};
    explicit MockTimeService(time_t now): n(now) {}
    time_t now() const override { return n; }
    struct tm* localtime(time_t* t) const override { return ::localtime(t); }
    bool setSystemTime(time_t t) override { n = t; return true; }
    uint32_t monotonicMillis() const override { return ms; }
};

void setUp(void) {}
void tearDown(void) {}

// 基本的なテストのみ
void test_basic_state_manager() {
    StateManager sm;
    TEST_ASSERT_NOT_NULL(&sm);
}

void test_basic_input_logic() {
    auto timeService = std::make_shared<MockTimeService>(kFixedTestTime);
    InputLogic logic(timeService);
    logic.reset();
    
    // 初期状態確認
    TEST_ASSERT_EQUAL(-1, logic.getValue());
    
    // 基本的な入力テスト
    logic.incrementInput(5);
    TEST_ASSERT_EQUAL(5, logic.getDigit(3));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_basic_state_manager);
    RUN_TEST(test_basic_input_logic);
    UNITY_END();
    return 0;
} 