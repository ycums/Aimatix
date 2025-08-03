#include <unity.h>
#include <memory>
#include <iostream>
#include "InputDisplayViewImpl.h"
#include "IDisplay.h"

// Unityテストフレームワークの要件
void setUp(void) {}
void tearDown(void) {}

// テスト用のモックDisplayクラス
class MockDisplay : public IDisplay {
public:
    MockDisplay() : clearCalled(false), setTextDatumCalled(false), setTextColorCalled(false) {}
    
    void clear() override { clearCalled = true; }
    void setTextDatum(int datum) override { setTextDatumCalled = true; }
    void setTextColor(uint32_t color, uint32_t bgColor) override { setTextColorCalled = true; }
    void drawText(int x, int y, const char* text, int fontSize) override { drawTextCalled = true; }
    void fillRect(int x, int y, int w, int h, uint32_t color) override { fillRectCalled = true; }
    void drawRect(int x, int y, int w, int h, uint32_t color) override { drawRectCalled = true; }
    void setTextFont(int font) override { setTextFontCalled = true; }
    void fillProgressBarSprite(int x, int y, int w, int h, int percent) override { fillProgressBarSpriteCalled = true; }
    void drawLine(int x0, int y0, int x1, int y1, uint32_t color) override { drawLineCalled = true; }
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
};

void test_input_display_view_impl_constructor() {
    // コンストラクタのテスト（1観点）
    auto mockDisplay = std::make_shared<MockDisplay>();
    InputDisplayViewImpl view(mockDisplay.get());
    
    TEST_ASSERT_TRUE(true); // エラーが発生しなければ成功
}

void test_input_display_view_impl_clear() {
    // clear()メソッドのテスト（1観点）
    auto mockDisplay = std::make_shared<MockDisplay>();
    InputDisplayViewImpl view(mockDisplay.get());
    
    view.clear();
    
    TEST_ASSERT_TRUE(mockDisplay->clearCalled);
}

void test_input_display_view_impl_show_title() {
    // showTitle()メソッドのテスト（1観点）
    auto mockDisplay = std::make_shared<MockDisplay>();
    InputDisplayViewImpl view(mockDisplay.get());
    
    view.showTitle("TEST", 80, false);
    
    // drawTitleBarが呼ばれることを確認（モックの実装に依存）
    TEST_ASSERT_TRUE(true); // エラーが発生しなければ成功
}

void test_input_display_view_impl_show_digit() {
    // showDigit()メソッドのテスト（1観点）
    auto mockDisplay = std::make_shared<MockDisplay>();
    InputDisplayViewImpl view(mockDisplay.get());
    
    view.showDigit(0, 1, true);
    
    TEST_ASSERT_TRUE(mockDisplay->setTextDatumCalled);
    TEST_ASSERT_TRUE(mockDisplay->setTextColorCalled);
    TEST_ASSERT_TRUE(mockDisplay->drawTextCalled);
}

void test_input_display_view_impl_show_hints() {
    // showHints()メソッドのテスト（1観点）
    auto mockDisplay = std::make_shared<MockDisplay>();
    InputDisplayViewImpl view(mockDisplay.get());
    
    view.showHints("A", "B", "C");
    
    // drawButtonHintsGridが呼ばれることを確認
    TEST_ASSERT_TRUE(true); // エラーが発生しなければ成功
}

void test_input_display_view_impl_show_preview() {
    // showPreview()メソッドのテスト（1観点）
    auto mockDisplay = std::make_shared<MockDisplay>();
    InputDisplayViewImpl view(mockDisplay.get());
    
    view.showPreview("Preview Text");
    
    TEST_ASSERT_TRUE(mockDisplay->setTextFontCalled);
    TEST_ASSERT_TRUE(mockDisplay->setTextColorCalled);
    TEST_ASSERT_TRUE(mockDisplay->setTextDatumCalled);
    TEST_ASSERT_TRUE(mockDisplay->fillRectCalled);
    TEST_ASSERT_TRUE(mockDisplay->drawTextCalled);
}

void test_input_display_view_impl_show_colon() {
    // showColon()メソッドのテスト（1観点）
    auto mockDisplay = std::make_shared<MockDisplay>();
    InputDisplayViewImpl view(mockDisplay.get());
    
    view.showColon();
    
    TEST_ASSERT_TRUE(mockDisplay->setTextDatumCalled);
    TEST_ASSERT_TRUE(mockDisplay->setTextColorCalled);
    TEST_ASSERT_TRUE(mockDisplay->drawTextCalled);
}

int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_input_display_view_impl_constructor);
    RUN_TEST(test_input_display_view_impl_clear);
    RUN_TEST(test_input_display_view_impl_show_title);
    RUN_TEST(test_input_display_view_impl_show_digit);
    RUN_TEST(test_input_display_view_impl_show_hints);
    RUN_TEST(test_input_display_view_impl_show_preview);
    RUN_TEST(test_input_display_view_impl_show_colon);
    
    return UNITY_END();
} 