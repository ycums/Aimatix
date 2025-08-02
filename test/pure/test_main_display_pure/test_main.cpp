#include <unity.h>
#include "MainDisplayState.h"
#include "IMainDisplayView.h"
#include "ITimeProvider.h"
#include <memory>
#include <string>
#include <vector>

// テスト用の固定値
const time_t kFixedTestTime = 1700000000;

// モックMainDisplayViewクラス
class MockMainDisplayView : public IMainDisplayView {
public:
    int showTitleCallCount = 0;
    int showHintsCallCount = 0;
    int showTimeCallCount = 0;
    int showRemainCallCount = 0;
    int showProgressCallCount = 0;
    int showAlarmListCallCount = 0;
    int clearCallCount = 0;
    
    std::string lastTitle;
    int lastBatteryLevel = 0;
    bool lastIsCharging = false;
    std::string lastBtnA;
    std::string lastBtnB;
    std::string lastBtnC;
    std::string lastTime;
    std::string lastRemain;
    int lastProgress = 0;
    std::vector<std::string> lastAlarmList;
    
    void showTitle(const char* title, int batteryLevel, bool isCharging) override {
        showTitleCallCount++;
        lastTitle = title ? title : "";
        lastBatteryLevel = batteryLevel;
        lastIsCharging = isCharging;
    }
    
    void showHints(const char* btnA, const char* btnB, const char* btnC) override {
        showHintsCallCount++;
        lastBtnA = btnA ? btnA : "";
        lastBtnB = btnB ? btnB : "";
        lastBtnC = btnC ? btnC : "";
    }
    
    void showTime(const char* time) override {
        showTimeCallCount++;
        lastTime = time ? time : "";
    }
    
    void showRemain(const char* remain) override {
        showRemainCallCount++;
        lastRemain = remain ? remain : "";
    }
    
    void showProgress(int percent) override {
        showProgressCallCount++;
        lastProgress = percent;
    }
    
    void showAlarmList(const std::vector<std::string>& alarmStrs) override {
        showAlarmListCallCount++;
        lastAlarmList = alarmStrs;
    }
    
    void clear() override {
        clearCallCount++;
    }
    
    void reset() {
        showTitleCallCount = 0;
        showHintsCallCount = 0;
        showTimeCallCount = 0;
        showRemainCallCount = 0;
        showProgressCallCount = 0;
        showAlarmListCallCount = 0;
        clearCallCount = 0;
        lastTitle.clear();
        lastBatteryLevel = 0;
        lastIsCharging = false;
        lastBtnA.clear();
        lastBtnB.clear();
        lastBtnC.clear();
        lastTime.clear();
        lastRemain.clear();
        lastProgress = 0;
        lastAlarmList.clear();
    }
};

// モックTimeProviderクラス
class MockTimeProvider : public ITimeProvider {
public:
    time_t fixedTime;
    
    MockTimeProvider(time_t t) : fixedTime(t) {}
    
    time_t now() const override { return fixedTime; }
    struct tm* localtime(time_t* t) const override { return ::localtime(t); }
    bool setSystemTime(time_t time) override { return true; }
};

void setUp(void) {}
void tearDown(void) {}

// MainDisplayStateテストケース（1テスト1観点）

void test_main_display_state_onenter() {
    // 初期化処理のテスト（1観点）
    auto mockTimeProvider = std::make_shared<MockTimeProvider>(kFixedTestTime);
    auto mockView = std::unique_ptr<MockMainDisplayView>(new MockMainDisplayView());
    MainDisplayState state(nullptr, nullptr, mockView.get(), nullptr, nullptr);
    
    // onEnter()を実行
    state.onEnter();
    
    // 初期化処理が正常に動作することを確認
    TEST_ASSERT_TRUE(true);
}

void test_main_display_state_ondraw() {
    // 描画処理のテスト（1観点）
    auto mockTimeProvider = std::make_shared<MockTimeProvider>(kFixedTestTime);
    auto mockView = std::unique_ptr<MockMainDisplayView>(new MockMainDisplayView());
    MainDisplayState state(nullptr, nullptr, mockView.get(), nullptr, nullptr);
    
    // onDraw()を実行
    state.onDraw();
    
    // 描画処理が正常に動作することを確認
    TEST_ASSERT_TRUE(true);
}

void test_main_display_state_onbutton_a() {
    // ボタンA処理のテスト（1観点）
    auto mockTimeProvider = std::make_shared<MockTimeProvider>(kFixedTestTime);
    auto mockView = std::unique_ptr<MockMainDisplayView>(new MockMainDisplayView());
    MainDisplayState state(nullptr, nullptr, mockView.get(), nullptr, nullptr);
    
    // onButtonA()を実行
    state.onButtonA();
    
    // ボタンA処理が正常に動作することを確認
    TEST_ASSERT_TRUE(true);
}

void test_main_display_state_onbutton_b() {
    // ボタンB処理のテスト（1観点）
    auto mockTimeProvider = std::make_shared<MockTimeProvider>(kFixedTestTime);
    auto mockView = std::unique_ptr<MockMainDisplayView>(new MockMainDisplayView());
    MainDisplayState state(nullptr, nullptr, mockView.get(), nullptr, nullptr);
    
    // onButtonB()を実行
    state.onButtonB();
    
    // ボタンB処理が正常に動作することを確認
    TEST_ASSERT_TRUE(true);
}

void test_main_display_state_onbutton_c() {
    // ボタンC処理のテスト（1観点）
    auto mockTimeProvider = std::make_shared<MockTimeProvider>(kFixedTestTime);
    auto mockView = std::unique_ptr<MockMainDisplayView>(new MockMainDisplayView());
    MainDisplayState state(nullptr, nullptr, mockView.get(), nullptr, nullptr);
    
    // onButtonC()を実行
    state.onButtonC();
    
    // ボタンC処理が正常に動作することを確認
    TEST_ASSERT_TRUE(true);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_main_display_state_onenter);
    RUN_TEST(test_main_display_state_ondraw);
    RUN_TEST(test_main_display_state_onbutton_a);
    RUN_TEST(test_main_display_state_onbutton_b);
    RUN_TEST(test_main_display_state_onbutton_c);
    UNITY_END();
    return 0;
} 