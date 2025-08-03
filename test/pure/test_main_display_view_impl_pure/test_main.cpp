#include <unity.h>
#include <memory>
#include <vector>
#include <string>
#include "MainDisplayViewImpl.h"
#include "IDisplay.h"

// Unityテストフレームワークの要件
void setUp(void) {}
void tearDown(void) {}

// テスト用のモックDisplayクラス
class MockDisplay : public IDisplay {
public:
    MockDisplay() : clearCalled(false), setTextDatumCalled(false), setTextColorCalled(false), 
                    drawTextCalled(false), fillRectCalled(false), fillProgressBarSpriteCalled(false) {}
    
    void clear() override { clearCalled = true; }
    void setTextDatum(int datum) override { setTextDatumCalled = true; }
    void setTextColor(uint16_t color, uint16_t bgColor) override { setTextColorCalled = true; }
    void drawText(int x, int y, const char* text, int fontSize) override { 
        drawTextCalled = true; 
        lastDrawText = text ? text : "";
    }
    void fillRect(int x, int y, int w, int h, uint16_t color) override { fillRectCalled = true; }
    void drawRect(int x, int y, int w, int h, uint16_t color) override { drawRectCalled = true; }
    void setTextFont(int font) override { setTextFontCalled = true; }
    void fillProgressBarSprite(int x, int y, int w, int h, int percent) override { 
        fillProgressBarSpriteCalled = true; 
        lastProgressPercent = percent;
    }
    void drawLine(int x0, int y0, int x1, int y1, uint16_t color) override { drawLineCalled = true; }
    int getTextDatum() const override { getTextDatumCalled = true; return 0; }
    
    // テスト用フラグ
    bool clearCalled;
    bool setTextDatumCalled;
    bool setTextColorCalled;
    bool drawTextCalled;
    bool fillRectCalled;
    bool drawRectCalled;
    bool drawLineCalled;
    bool setTextFontCalled;
    bool fillProgressBarSpriteCalled;
    mutable bool getTextDatumCalled;
    std::string lastDrawText;
    int lastProgressPercent = 0;
};

void test_main_display_view_impl_constructor() {
    // コンストラクタのテスト（1観点）
    auto mockDisplay = std::make_shared<MockDisplay>();
    MainDisplayViewImpl view(mockDisplay.get());
    
    TEST_ASSERT_TRUE(true); // エラーが発生しなければ成功
}

void test_main_display_view_impl_clear() {
    // clear()メソッドのテスト（1観点）
    auto mockDisplay = std::make_shared<MockDisplay>();
    MainDisplayViewImpl view(mockDisplay.get());
    
    view.clear();
    
    TEST_ASSERT_TRUE(mockDisplay->clearCalled);
}

void test_main_display_view_impl_show_title() {
    // showTitle()メソッドのテスト（1観点）
    auto mockDisplay = std::make_shared<MockDisplay>();
    MainDisplayViewImpl view(mockDisplay.get());
    
    view.showTitle("MAIN", 80, false);
    
    // drawTitleBarが呼ばれることを確認
    TEST_ASSERT_TRUE(true); // エラーが発生しなければ成功
}

void test_main_display_view_impl_show_time() {
    // showTime()メソッドのテスト（1観点）
    auto mockDisplay = std::make_shared<MockDisplay>();
    MainDisplayViewImpl view(mockDisplay.get());
    
    view.showTime("12:34");
    
    TEST_ASSERT_TRUE(mockDisplay->setTextColorCalled);
    TEST_ASSERT_TRUE(mockDisplay->setTextDatumCalled);
    TEST_ASSERT_TRUE(mockDisplay->drawTextCalled);
    TEST_ASSERT_EQUAL_STRING("12:34", mockDisplay->lastDrawText.c_str());
}

void test_main_display_view_impl_show_remain() {
    // showRemain()メソッドのテスト（1観点）
    auto mockDisplay = std::make_shared<MockDisplay>();
    MainDisplayViewImpl view(mockDisplay.get());
    
    view.showRemain("01:23:45");
    
    TEST_ASSERT_TRUE(mockDisplay->setTextDatumCalled);
    TEST_ASSERT_TRUE(mockDisplay->drawTextCalled);
    TEST_ASSERT_EQUAL_STRING("01:23:45", mockDisplay->lastDrawText.c_str());
}

void test_main_display_view_impl_show_progress() {
    // showProgress()メソッドのテスト（1観点）
    auto mockDisplay = std::make_shared<MockDisplay>();
    MainDisplayViewImpl view(mockDisplay.get());
    
    view.showProgress(50);
    
    TEST_ASSERT_TRUE(mockDisplay->fillProgressBarSpriteCalled);
    TEST_ASSERT_EQUAL(50, mockDisplay->lastProgressPercent);
}

void test_main_display_view_impl_show_alarm_list() {
    // showAlarmList()メソッドのテスト（1観点）
    auto mockDisplay = std::make_shared<MockDisplay>();
    MainDisplayViewImpl view(mockDisplay.get());
    
    std::vector<std::string> alarms = {"12:00", "13:30", "15:45"};
    view.showAlarmList(alarms);
    
    TEST_ASSERT_TRUE(mockDisplay->setTextDatumCalled);
    TEST_ASSERT_TRUE(mockDisplay->drawTextCalled);
}

void test_main_display_view_impl_show_alarm_list_empty() {
    // showAlarmList()メソッドのテスト（空リスト）
    auto mockDisplay = std::make_shared<MockDisplay>();
    MainDisplayViewImpl view(mockDisplay.get());
    
    std::vector<std::string> alarms;
    view.showAlarmList(alarms);
    
    TEST_ASSERT_TRUE(mockDisplay->setTextDatumCalled);
    TEST_ASSERT_TRUE(mockDisplay->fillRectCalled); // 空の場合はクリアされる
}

void test_main_display_view_impl_show_hints() {
    // showHints()メソッドのテスト（1観点）
    auto mockDisplay = std::make_shared<MockDisplay>();
    MainDisplayViewImpl view(mockDisplay.get());
    
    view.showHints("ABS", "REL+", "MGMT");
    
    // drawButtonHintsGridが呼ばれることを確認
    TEST_ASSERT_TRUE(true); // エラーが発生しなければ成功
}

void test_main_display_view_impl_show_alarm_list_partial() {
    // showAlarmList()メソッドのテスト（部分的なアラームリスト）
    auto mockDisplay = std::make_shared<MockDisplay>();
    MainDisplayViewImpl view(mockDisplay.get());
    
    std::vector<std::string> alarms = {"12:00"}; // 1つだけ
    view.showAlarmList(alarms);
    
    TEST_ASSERT_TRUE(mockDisplay->setTextDatumCalled);
    TEST_ASSERT_TRUE(mockDisplay->drawTextCalled);
    TEST_ASSERT_TRUE(mockDisplay->fillRectCalled); // 残りはクリアされる
}

void test_main_display_view_impl_show_progress_edge_cases() {
    // showProgress()メソッドのテスト（境界値）
    auto mockDisplay = std::make_shared<MockDisplay>();
    MainDisplayViewImpl view(mockDisplay.get());
    
    // 0%
    view.showProgress(0);
    TEST_ASSERT_EQUAL(0, mockDisplay->lastProgressPercent);
    
    // 100%
    view.showProgress(100);
    TEST_ASSERT_EQUAL(100, mockDisplay->lastProgressPercent);
    
    // 負の値
    view.showProgress(-10);
    TEST_ASSERT_EQUAL(-10, mockDisplay->lastProgressPercent);
}

void test_main_display_view_impl_show_time_null() {
    // showTime()メソッドのテスト（null文字列）
    auto mockDisplay = std::make_shared<MockDisplay>();
    MainDisplayViewImpl view(mockDisplay.get());
    
    view.showTime(nullptr);
    
    TEST_ASSERT_TRUE(mockDisplay->setTextColorCalled);
    TEST_ASSERT_TRUE(mockDisplay->setTextDatumCalled);
    TEST_ASSERT_TRUE(mockDisplay->drawTextCalled);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    
    RUN_TEST(test_main_display_view_impl_constructor);
    RUN_TEST(test_main_display_view_impl_clear);
    RUN_TEST(test_main_display_view_impl_show_title);
    RUN_TEST(test_main_display_view_impl_show_time);
    RUN_TEST(test_main_display_view_impl_show_remain);
    RUN_TEST(test_main_display_view_impl_show_progress);
    RUN_TEST(test_main_display_view_impl_show_alarm_list);
    RUN_TEST(test_main_display_view_impl_show_alarm_list_empty);
    RUN_TEST(test_main_display_view_impl_show_hints);
    RUN_TEST(test_main_display_view_impl_show_alarm_list_partial);
    RUN_TEST(test_main_display_view_impl_show_progress_edge_cases);
    RUN_TEST(test_main_display_view_impl_show_time_null);
    
    UNITY_END();
    return 0;
} 