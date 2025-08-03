#include <unity.h>
#include "InputDisplayViewImpl.h"
#include "IDisplay.h"
#include "IInputDisplayView.h"
#include <memory>
#include <string>

// テスト用の固定値
const int kTestBatteryLevel = 80;
const bool kTestIsCharging = false;

// モックDisplayクラス
class MockDisplay : public IDisplay, public IInputDisplayView {
public:
    int drawTextCallCount = 0;
    int setTextColorCallCount = 0;
    int fillRectCallCount = 0;
    int drawLineCallCount = 0;
    int setTextDatumCallCount = 0;
    int setTextFontCallCount = 0;
    int clearCallCount = 0;
    
    std::string lastTitle;
    int lastBatteryLevel = 0;
    bool lastIsCharging = false;
    std::string lastBtnA;
    std::string lastBtnB;
    std::string lastBtnC;
    std::string lastPreview;
    int lastDigitIndex = 0;
    int lastDigitValue = 0;
    bool lastDigitEntered = false;
    
    // IDisplayの純粋仮想関数の実装
    void drawText(int x, int y, const char* text, int fontSize) override {
        drawTextCallCount++;
    }
    void setTextColor(uint32_t color, uint32_t bgColor) override {
        setTextColorCallCount++;
    }
    void fillRect(int x, int y, int w, int h, uint32_t color) override {
        fillRectCallCount++;
    }
    void drawRect(int x, int y, int w, int h, uint32_t color) override {}
    void setTextDatum(int datum) override {
        setTextDatumCallCount++;
    }
    void setTextFont(int font) override {
        setTextFontCallCount++;
    }
    void fillProgressBarSprite(int x, int y, int w, int h, int percent) override {}
    void drawLine(int x0, int y0, int x1, int y1, uint32_t color) override {
        drawLineCallCount++;
    }
    int getTextDatum() const override { return 0; }
    
    void showTitle(const char* title, int batteryLevel, bool isCharging) override {
        lastTitle = title ? title : "";
        lastBatteryLevel = batteryLevel;
        lastIsCharging = isCharging;
    }
    
    void showHints(const char* btnA, const char* btnB, const char* btnC) override {
        lastBtnA = btnA ? btnA : "";
        lastBtnB = btnB ? btnB : "";
        lastBtnC = btnC ? btnC : "";
    }
    
    void showPreview(const char* preview) override {
        lastPreview = preview ? preview : "";
    }
    
    void clear() override {
        clearCallCount++;
    }
    
    void showDigit(int index, int value, bool entered) override {
        lastDigitIndex = index;
        lastDigitValue = value;
        lastDigitEntered = entered;
    }
    
    void showColon() override {
        // showColonはdrawTextを呼び出す
    }
    
    void reset() {
        drawTextCallCount = 0;
        setTextColorCallCount = 0;
        fillRectCallCount = 0;
        drawLineCallCount = 0;
        setTextDatumCallCount = 0;
        setTextFontCallCount = 0;
        clearCallCount = 0;
        lastTitle.clear();
        lastBatteryLevel = 0;
        lastIsCharging = false;
        lastBtnA.clear();
        lastBtnB.clear();
        lastBtnC.clear();
        lastPreview.clear();
        lastDigitIndex = 0;
        lastDigitValue = 0;
        lastDigitEntered = false;
    }
};

void setUp(void) {}
void tearDown(void) {}

// InputDisplayViewImplテストケース（1テスト1観点）

void test_input_display_view_impl_show_title() {
    // タイトル表示処理のテスト（1観点）
    auto mockDisplay = std::unique_ptr<MockDisplay>(new MockDisplay());
    InputDisplayViewImpl view(mockDisplay.get());
    
    // タイトル表示を実行
    view.showTitle("Test Title", kTestBatteryLevel, kTestIsCharging);
    
    // 表示処理が呼ばれることを確認（drawTextが呼ばれる）
    TEST_ASSERT_TRUE(mockDisplay->drawTextCallCount > 0);
}

void test_input_display_view_impl_show_hints() {
    // ヒント表示処理のテスト（1観点）
    auto mockDisplay = std::unique_ptr<MockDisplay>(new MockDisplay());
    InputDisplayViewImpl view(mockDisplay.get());
    
    // ヒント表示を実行
    view.showHints("A", "B", "C");
    
    // 表示処理が呼ばれることを確認（drawTextが呼ばれる）
    TEST_ASSERT_TRUE(mockDisplay->drawTextCallCount > 0);
}

void test_input_display_view_impl_show_preview() {
    // プレビュー表示処理のテスト（1観点）
    auto mockDisplay = std::unique_ptr<MockDisplay>(new MockDisplay());
    InputDisplayViewImpl view(mockDisplay.get());
    
    // プレビュー表示を実行
    view.showPreview("Test Preview");
    
    // 表示処理が呼ばれることを確認（drawTextが呼ばれる）
    TEST_ASSERT_TRUE(mockDisplay->drawTextCallCount > 0);
}

void test_input_display_view_impl_show_digit() {
    // 数字表示処理のテスト（1観点）
    auto mockDisplay = std::unique_ptr<MockDisplay>(new MockDisplay());
    InputDisplayViewImpl view(mockDisplay.get());
    
    // 数字表示を実行
    view.showDigit(2, 5, true);
    
    // 表示処理が呼ばれることを確認（drawTextが呼ばれる）
    TEST_ASSERT_TRUE(mockDisplay->drawTextCallCount > 0);
}

void test_input_display_view_impl_show_colon() {
    // コロン表示処理のテスト（1観点）
    auto mockDisplay = std::unique_ptr<MockDisplay>(new MockDisplay());
    InputDisplayViewImpl view(mockDisplay.get());
    
    // コロン表示を実行
    view.showColon();
    
    // 表示処理が呼ばれることを確認（drawTextが呼ばれる）
    TEST_ASSERT_TRUE(mockDisplay->drawTextCallCount > 0);
}

void test_input_display_view_impl_clear() {
    // クリア処理のテスト（1観点）
    auto mockDisplay = std::unique_ptr<MockDisplay>(new MockDisplay());
    InputDisplayViewImpl view(mockDisplay.get());
    
    // クリア処理を実行
    view.clear();
    
    // 処理が呼ばれることを確認
    TEST_ASSERT_EQUAL(1, mockDisplay->clearCallCount);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_input_display_view_impl_show_title);
    RUN_TEST(test_input_display_view_impl_show_hints);
    RUN_TEST(test_input_display_view_impl_show_preview);
    RUN_TEST(test_input_display_view_impl_show_digit);
    RUN_TEST(test_input_display_view_impl_show_colon);
    RUN_TEST(test_input_display_view_impl_clear);
    UNITY_END();
    return 0;
} 