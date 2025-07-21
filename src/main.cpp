#include "main_display.h"
#ifdef ARDUINO
#include <M5Stack.h>
#include <vector>
#include <ctime>
#include <M5Display.h>
#endif
#include "AlarmLogic.h"
extern void setFillRectImpl(void (*impl)(int, int, int, int, int));
extern void setFillProgressBarSpriteImpl(void (*impl)(int, int, int, int, int));

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
void M5FillRectImpl(int x, int y, int w, int h, int color) {
    M5.Lcd.fillRect(x, y, w, h, color);
}
TFT_eSprite progressSprite(&M5.Lcd);
void M5FillProgressBarSpriteImpl(int x, int y, int w, int h, int percent) {
    TFT_eSprite sprite(&M5.Lcd);
    sprite.createSprite(w, h);
    sprite.fillRect(0, 0, w, h, TFT_BLACK);
    sprite.drawRect(0, 0, w, h, AMBER_COLOR);
    int fillW = (w - 2) * percent / 100;
    if (fillW > 0) {
        sprite.fillRect(1, 1, fillW, h - 2, AMBER_COLOR);
    }
    sprite.pushSprite(x, y);
    sprite.deleteSprite();
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
    setFillRectImpl(M5FillRectImpl);
    setFillProgressBarSpriteImpl(M5FillProgressBarSpriteImpl);
    M5.Lcd.setTextColor(AMBER_COLOR, TFT_BLACK);

    // アラームリスト初期化
    alarmTimes.clear();
    time_t now = time(nullptr);
    AlarmLogic::initAlarms(alarmTimes, now);
#endif
    // drawGridLines(); // デバッグ用グリッド線
    drawMainDisplay();
}

void loop() {
#ifdef ARDUINO
    drawMainDisplay();
    delay(200); // 200msごとに画面更新
#endif
} 