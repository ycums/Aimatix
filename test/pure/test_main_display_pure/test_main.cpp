#include <unity.h>
#include "DisplayCommon.h"
#include "IDisplay.h"
#include "ui_constants.h"
#include <vector>
#include <string>

class MockDisplay : public IDisplay {
public:
    std::vector<std::string> log;
    void clear() override { log.push_back("clear"); }
    void drawText(int x, int y, const char* text, int fontSize) override {
        char buf[128];
        snprintf(buf, sizeof(buf), "drawText(%d,%d,%s,%d)", x, y, text, fontSize);
        log.push_back(buf);
    }
    void setTextColor(uint32_t color, uint32_t bgColor) override {
        log.push_back("setTextColor");
    }
    void fillRect(int x, int y, int w, int h, uint32_t color) override {
        char buf[128];
        snprintf(buf, sizeof(buf), "fillRect(%d,%d,%d,%d,%u)", x, y, w, h, color);
        log.push_back(buf);
    }
    void drawRect(int x, int y, int w, int h, uint32_t color) override {
        char buf[128];
        snprintf(buf, sizeof(buf), "drawRect(%d,%d,%d,%d,%u)", x, y, w, h, color);
        log.push_back(buf);
    }
    void setTextDatum(int datum) override {
        log.push_back("setTextDatum");
    }
    void setTextFont(int font) override {
        log.push_back("setTextFont");
    }
    void fillProgressBarSprite(int x, int y, int w, int h, int percent) override {
        char buf[128];
        snprintf(buf, sizeof(buf), "fillProgressBarSprite(%d,%d,%d,%d,%d)", x, y, w, h, percent);
        log.push_back(buf);
    }
};

void setUp(void) {}
void tearDown(void) {}

void test_drawTitleBar_and_buttonHints() {
    MockDisplay disp;
    drawTitleBar(&disp, "MAIN", 42, false);
    drawButtonHintsGrid(&disp, "A", "B", "C");
    // ログ内容をざっくり検証
    bool foundTitle = false, foundHint = false;
    for (const auto& s : disp.log) {
        if (s.find("drawText") != std::string::npos && s.find("MAIN") != std::string::npos) foundTitle = true;
        if (s.find("drawText") != std::string::npos && (s.find("A") != std::string::npos || s.find("B") != std::string::npos || s.find("C") != std::string::npos)) foundHint = true;
    }
    TEST_ASSERT(foundTitle);
    TEST_ASSERT(foundHint);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_drawTitleBar_and_buttonHints);
    UNITY_END();
    return 0;
} 