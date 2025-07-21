#include "main_display.h"
#include "StateManager.h"
#include "MainDisplayState.h"
#include "InputDisplayState.h"
#include "InputLogic.h"
#include "InputDisplayViewImpl.h"
#include "MainDisplayViewImpl.h"
#include "TimeLogic.h"
#include "AlarmLogic.h"
#include "DisplayAdapter.h"
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
    M5.Lcd.drawRect(x, y, w, h, AMBER_COLOR);
    M5.Lcd.fillRect(x + 1, y + 1, w - 2, h - 2, TFT_BLACK);
    int fillW = (w - 2) * percent / 100;
    if (fillW > 0) {
        M5.Lcd.fillRect(x + 1, y + 1, fillW, h - 2, AMBER_COLOR);
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
#endif

// --- アラームリスト ---
#include <vector>
#include <ctime>
std::vector<time_t> alarmTimes;

// --- 状態管理クラスのグローバル生成 ---
StateManager stateManager;
InputLogic inputLogic;
DisplayAdapter displayAdapter;
InputDisplayViewImpl inputDisplayViewImpl(&displayAdapter);
MainDisplayViewImpl mainDisplayViewImpl(&displayAdapter);
TimeLogic timeLogic;
AlarmLogic alarmLogic;
InputDisplayState inputDisplayState(&inputLogic, &inputDisplayViewImpl);
MainDisplayState mainDisplayState(&stateManager, &inputDisplayState, &mainDisplayViewImpl, &timeLogic, &alarmLogic);

void setup() {
#ifdef ARDUINO
    M5.begin();
    M5.Lcd.setTextColor(AMBER_COLOR, TFT_BLACK);

    // アラームリスト初期化
    alarmTimes.clear();
    time_t now = time(nullptr);
    AlarmLogic::initAlarms(alarmTimes, now);
#endif
    // 状態遷移の初期状態をMainDisplayに
    stateManager.setState(&mainDisplayState);
}

void loop() {
#ifdef ARDUINO
    M5.update();
    // ボタンイベント伝播
    if (M5.BtnA.wasPressed()) stateManager.handleButtonA();
    if (M5.BtnB.wasPressed()) stateManager.handleButtonB();
    if (M5.BtnC.wasPressed()) stateManager.handleButtonC();
    // 長押し等も必要に応じて追加
    // 現在の状態の描画
    IState* current = stateManager.getCurrentState();
    if (current) current->onDraw();
    delay(50);
#endif
} 