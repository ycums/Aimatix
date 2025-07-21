#include "main_display.h"
#ifdef ARDUINO
#include <M5Stack.h>
#include <vector>
#include <ctime>
#endif

#ifdef ARDUINO
// --- M5Stack用描画関数 ---
void M5RectImpl(int x, int y, int w, int h) {
    M5.Lcd.drawRect(x, y, w, h, AMBER_COLOR);
}
void M5StringImpl(const char* str, int x, int y) {
    M5.Lcd.drawString(str, x, y);
}
void M5ProgressBarImpl(int x, int y, int w, int h, int percent) {
    // x, y, w, hはそのまま四隅を表す
    M5.Lcd.drawRect(x, y, w, h, AMBER_COLOR); // 枠
    M5.Lcd.fillRect(x + 1, y + 1, w - 2, h - 2, TFT_BLACK); // 背景
    int fillW = (w - 2) * percent / 100;
    if (fillW > 0) {
        M5.Lcd.fillRect(x + 1, y + 1, fillW, h - 2, AMBER_COLOR); // 進捗
    }
}
void M5SetFontImpl(int font) {
    M5.Lcd.setTextFont(font);
    M5.Lcd.setTextColor(AMBER_COLOR, TFT_BLACK);
}
void M5SetTextDatumImpl(int datum) {
    M5.Lcd.setTextDatum(datum);
}
#endif

// --- アラームリスト ---
#include <vector>
#include <ctime>
std::vector<time_t> alarmTimes;

void setup() {
#ifdef ARDUINO
    M5.begin();
    setDrawRectImpl(M5RectImpl);
    setDrawStringImpl(M5StringImpl);
    setFillProgressBarImpl(M5ProgressBarImpl);
    setFontImpl(M5SetFontImpl);
    setTextDatumImpl(M5SetTextDatumImpl);
    M5.Lcd.setTextColor(AMBER_COLOR, TFT_BLACK);

    // アラームリスト初期化
    alarmTimes.clear();
    time_t now = time(nullptr);
    alarmTimes.push_back(now + 10);    // +10秒
    alarmTimes.push_back(now + 30);    // +30秒
    alarmTimes.push_back(now + 60);    // +1分
    alarmTimes.push_back(now + 120);   // +2分
#endif
    // drawGridLines(); // デバッグ用グリッド線
    drawMainDisplay();
}

void loop() {
    // 何もしない（雛形）
} 