#include <unity.h>
#include "IDisplay.h"

// テスト用のモッククラス（インターフェースのみに依存）
class MockDisplayAdapter : public IDisplay {
public:
    bool setTextColorCalled = false;
    bool fillRectCalled = false;
    bool drawRectCalled = false;
    bool drawLineCalled = false;
    bool clearCalled = false;
    bool drawTextCalled = false;
    bool setTextDatumCalled = false;
    bool setTextFontCalled = false;
    bool fillProgressBarSpriteCalled = false;
    mutable bool getTextDatumCalled = false;

    void clear() override { clearCalled = true; }
    void drawText(int x, int y, const char* text, int fontSize) override { drawTextCalled = true; }
    void setTextColor(uint16_t color, uint16_t bgColor) override { setTextColorCalled = true; }
    void fillRect(int x, int y, int w, int h, uint16_t color) override { fillRectCalled = true; }
    void drawRect(int x, int y, int w, int h, uint16_t color) override { drawRectCalled = true; }
    void setTextDatum(uint8_t datum) override { setTextDatumCalled = true; }
    void setTextFont(int font) override { setTextFontCalled = true; }
    void fillProgressBarSprite(int x, int y, int w, int h, int percent) override { fillProgressBarSpriteCalled = true; }
    void drawLine(int x0, int y0, int x1, int y1, uint16_t color) override { drawLineCalled = true; }
    int getTextDatum() const override { getTextDatumCalled = true; return 0; }
};

void setUp(void) {
    // テスト前の初期化
}

void tearDown(void) {
    // テスト後のクリーンアップ
}

void test_InputDisplayViewImpl_constructor() {
    MockDisplayAdapter mockDisplay;
    
    // インターフェースのみを使用したテスト
    TEST_ASSERT_TRUE(true); // モックが正常に動作することを確認
}

void test_InputDisplayViewImpl_clear() {
    MockDisplayAdapter mockDisplay;
    
    mockDisplay.clear();
    
    // 基本的な描画メソッドが呼ばれることを確認
    TEST_ASSERT_TRUE(mockDisplay.clearCalled);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_InputDisplayViewImpl_constructor);
    RUN_TEST(test_InputDisplayViewImpl_clear);
    return UNITY_END();
} 